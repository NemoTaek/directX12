#include "Stdafx.h"
#include "TextureClass.h"
#include "ParticleSystemClass.h"

#include <fstream>
using namespace std;

ParticleSystemClass::ParticleSystemClass() {}
ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other) {}
ParticleSystemClass:: ~ParticleSystemClass() {}

bool ParticleSystemClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	if (!LoadTexture(device, textureFilename))	return false;
	if (!InitializeParticleSystem())	return false;
	if (!InitializeBuffers(device))	return false;
	return true;
}

void ParticleSystemClass::Shutdown()
{
	ShutdownBuffers();
	ShutdownParticleSystem();
	ReleaseTexture();
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	// 오래된 파티클 해제
	KillParticles();

	// 새 파티클 방출
	EmitParticles(frameTime);

	// 파티클 위치를 업데이트
	UpdateParticles(frameTime);

	// 동적 정점 버퍼를 각 파티클의 새 위치로 업데이트
	return UpdateBuffers(deviceContext);
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ParticleSystemClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture() { return m_texture->GetTexture(); }

bool ParticleSystemClass::InitializeParticleSystem()
{
	// 방출될 때 파티클이 위치할 수 있는 편차 값 설정
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// 파티클의 속도 및 속도 편차 값 설정
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// 파티클 크기 설정
	m_particleSize = 0.2f;

	// 파티클이 초당 생겨 날 개수 설정
	m_particlesPerSecond = 250.0f;

	// 최대 파티클 생성 개수 설정
	m_maxParticles = 5000;

	// 파티클 배열 생성 및 초기화
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)	return false;
	for (int i = 0; i < m_maxParticles; i++) m_particleList[i].active = false;

	m_currentParticleCount = 0;
	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	if (m_particleList) {
		delete[] m_particleList;
		m_particleList = 0;
	}
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = m_maxParticles * 6;
	m_indexCount = m_vertexCount;

	// 정점 배열 생성
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)	return false;

	// 인덱스 배열 생성
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// 정점 배열 초기화
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열 초기화
	for (int i = 0; i < m_indexCount; i++)	indices[i] = i;

	// 정점 버퍼 서술자 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// 인덱스 버퍼 서술자 설정
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))	return false;

	// 생성 후 정점 및 인덱스 버퍼 해제
	delete[] indices;
	indices = nullptr;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
{
	//  인덱스 버퍼 해제
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 정점셰이더 해제
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void ParticleSystemClass::EmitParticles(float frameTime)
{
	// 프레임 시간 증가
	m_accumulatedTime += frameTime;

	// 방출할 파티클
	bool emitParticle = false;

	// 새 파티클을 방출할 시간인지 체크
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)) {
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// 방출할 파티클이 있으면 프레임 당 하나씩 방출
	if (emitParticle && (m_currentParticleCount < (m_maxParticles - 1))) {
		m_currentParticleCount++;

		float positionX = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationX;
		float positionY = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationY;
		float positionZ = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationZ;

		float velocity = m_particleVelocity + (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleVelocityVariation;

		float red = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;
		float green = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;
		float blue = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;

		// 블렌딩을 위해 파티클을 뒤에서 앞으로 렌더링
		// Z position을 사용하여 파티클 배열이 정렬되기 때문에 이를 이용하여 파티클이 삽입 될 위치 파악
		int index = 0;
		bool found = false;
		while (!found) {
			if (!m_particleList[index].active || m_particleList[index].positionZ < positionZ) {
				found = true;
			}
			else {
				index++;
			}
		}

		// 위치가 결정되었으면 파티클 삽입
		int i = m_currentParticleCount;
		int j = i - 1;

		while (i != index) {
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{
	for (int i = 0; i < m_currentParticleCount; i++) {
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}
}

void ParticleSystemClass::KillParticles()
{
	// 특정 높이 범위를 벗어난 파티클들을 제거
	for (int i = 0; i < m_maxParticles; i++) {
		if (m_particleList[i].active && m_particleList[i].positionY < -3.0f) {
			m_particleList[i].active = false;
			m_currentParticleCount--;

			for (int j = i; j < m_maxParticles - 1; j++) {
				m_particleList[j].positionX = m_particleList[j+1].positionX;
				m_particleList[j].positionY = m_particleList[j+1].positionY;
				m_particleList[j].positionZ = m_particleList[j+1].positionZ;
				m_particleList[j].red = m_particleList[j+1].red;
				m_particleList[j].green = m_particleList[j+1].green;
				m_particleList[j].blue = m_particleList[j+1].blue;
				m_particleList[j].velocity = m_particleList[j+1].velocity;
				m_particleList[j].active = m_particleList[j+1].active;
			}
		}
	}
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	VertexType* verticesPtr;
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 매 프레임마다 호출되어 동적 파티클 시스템의 모든 갱신된 파티클들의 위치를 이용하여 동적 정점 버퍼를 재구성
	int index = 0;
	for (int i = 0; i < m_currentParticleCount; i++) {
		// 좌하
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 좌상
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 우하
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 우하
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 좌상
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 우상
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, const WCHAR* filename1)
{
	// 텍스쳐 객체 생성
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	if (!m_texture->Initialize(device, filename1))	return false;

	return true;
}

void ParticleSystemClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}