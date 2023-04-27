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
	// ������ ��ƼŬ ����
	KillParticles();

	// �� ��ƼŬ ����
	EmitParticles(frameTime);

	// ��ƼŬ ��ġ�� ������Ʈ
	UpdateParticles(frameTime);

	// ���� ���� ���۸� �� ��ƼŬ�� �� ��ġ�� ������Ʈ
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
	// ����� �� ��ƼŬ�� ��ġ�� �� �ִ� ���� �� ����
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// ��ƼŬ�� �ӵ� �� �ӵ� ���� �� ����
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// ��ƼŬ ũ�� ����
	m_particleSize = 0.2f;

	// ��ƼŬ�� �ʴ� ���� �� ���� ����
	m_particlesPerSecond = 250.0f;

	// �ִ� ��ƼŬ ���� ���� ����
	m_maxParticles = 5000;

	// ��ƼŬ �迭 ���� �� �ʱ�ȭ
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

	// ���� �迭 ����
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)	return false;

	// �ε��� �迭 ����
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// ���� �迭 �ʱ�ȭ
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭 �ʱ�ȭ
	for (int i = 0; i < m_indexCount; i++)	indices[i] = i;

	// ���� ���� ������ ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// �ε��� ���� ������ ����
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))	return false;

	// ���� �� ���� �� �ε��� ���� ����
	delete[] indices;
	indices = nullptr;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
{
	//  �ε��� ���� ����
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// �������̴� ����
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void ParticleSystemClass::EmitParticles(float frameTime)
{
	// ������ �ð� ����
	m_accumulatedTime += frameTime;

	// ������ ��ƼŬ
	bool emitParticle = false;

	// �� ��ƼŬ�� ������ �ð����� üũ
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)) {
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// ������ ��ƼŬ�� ������ ������ �� �ϳ��� ����
	if (emitParticle && (m_currentParticleCount < (m_maxParticles - 1))) {
		m_currentParticleCount++;

		float positionX = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationX;
		float positionY = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationY;
		float positionZ = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleDeviationZ;

		float velocity = m_particleVelocity + (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) * m_particleVelocityVariation;

		float red = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;
		float green = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;
		float blue = (((static_cast<float>(rand())) - (static_cast<float>(rand()))) / RAND_MAX) + 0.5f;

		// ������ ���� ��ƼŬ�� �ڿ��� ������ ������
		// Z position�� ����Ͽ� ��ƼŬ �迭�� ���ĵǱ� ������ �̸� �̿��Ͽ� ��ƼŬ�� ���� �� ��ġ �ľ�
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

		// ��ġ�� �����Ǿ����� ��ƼŬ ����
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
	// Ư�� ���� ������ ��� ��ƼŬ���� ����
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

	// �� �����Ӹ��� ȣ��Ǿ� ���� ��ƼŬ �ý����� ��� ���ŵ� ��ƼŬ���� ��ġ�� �̿��Ͽ� ���� ���� ���۸� �籸��
	int index = 0;
	for (int i = 0; i < m_currentParticleCount; i++) {
		// ����
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// �»�
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ����
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ����
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// �»�
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// ���
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
	// ���� ������ ������ ������ ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� �� �ε��� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, const WCHAR* filename1)
{
	// �ؽ��� ��ü ����
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