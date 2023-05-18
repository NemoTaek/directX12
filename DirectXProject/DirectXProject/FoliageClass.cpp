#include "Stdafx.h"
#include "FoliageClass.h"
#include "TextureClass.h"
#include <time.h>

FoliageClass::FoliageClass() {}
FoliageClass::FoliageClass(const FoliageClass& other) {}
FoliageClass::~FoliageClass() {}

bool FoliageClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename, int foliageCount)
{
	// 단풍잎 개수 설정
	m_foliageCount = foliageCount;

	// 단풍 위치 생성
	if (!GeneratePositions())	return false;

	if (!InitializeBuffers(device))	return false;

	if (!LoadTexture(device, textureFilename))	return false;

	// 초기 바람 회전과 방향 설정
	m_windRotation = 0.0f;
	m_windDirection = 1;

	return true;
}

void FoliageClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	if (m_foliageArray) {
		delete[] m_foliageArray;
		m_foliageArray = 0;
	}
}

void FoliageClass::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

bool FoliageClass::Frame(XMFLOAT3 cameraPosition, ID3D11DeviceContext* deviceContext)
{
	// 바람 방향 업데이트
	if (m_windDirection == 1) {
		m_windRotation += 0.1f;
		if (m_windRotation > 10.0f) {
			m_windDirection = 2;
		}
	}
	else {
		m_windRotation -= 0.1f;
		if (m_windRotation < -10.0f) {
			m_windDirection = 1;
		}
	}

	// 업데이트 된 위치로 단풍 버퍼 로드
	XMFLOAT3 modelPosition = { 0.0f, 0.0f, 0.0f };
	float rotation = 0.0f;
	float windRotation = 0.0f;
	XMMATRIX rotateBillboardMatrix, rotateWindMatrix, translationMatrix, finalMatrix;

	for (int i = 0; i < m_foliageCount; i++) {
		modelPosition.x = m_foliageArray[i].x;
		modelPosition.y = -0.1f;
		modelPosition.z = m_foliageArray[i].z;

		// 아크탄젠트 함수로 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야 하는 회전 계산
		double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

		rotation = static_cast<float>(angle) * 0.0174532925f;

		// 빌보드의 X 회전 설정
		rotateBillboardMatrix = XMMatrixRotationY(rotation);

		// 단풍의 바람 회전
		windRotation = m_windRotation * 0.0174532925f;

		// 바람 회전 설정
		rotateWindMatrix = XMMatrixRotationX(windRotation);

		// 변환 행렬 설정
		translationMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

		// 최종 행렬을 만들고 인스턴스 배열에 저장
		finalMatrix = XMMatrixMultiply(rotateBillboardMatrix, rotateWindMatrix);
		m_instances[i].matrix = XMMatrixMultiply(finalMatrix, translationMatrix);
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;

	InstanceType* instancesPtr = static_cast<InstanceType*>(mappedResource.pData);
	memcpy(instancesPtr, static_cast<void*>(m_instances), (sizeof(InstanceType) * m_foliageCount));
	deviceContext->Unmap(m_instanceBuffer, 0);

	return true;
}

int FoliageClass::GetVertexCount() { return m_vertexCount; }
int FoliageClass::GetInstanceCount() { return m_instanceCount; }
ID3D11ShaderResourceView* FoliageClass::GetTexture() { return m_texture->GetTexture(); }

bool FoliageClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = 6;

	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// 좌하
	vertices[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	// 좌상
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);

	// 우하
	vertices[2].position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// 우하
	vertices[3].position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	// 좌상
	vertices[4].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[4].texture = XMFLOAT2(0.0f, 0.0f);

	// 우상
	vertices[5].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 0.0f);

	// 정점 버퍼 서술자 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// 생성 후 정점버퍼 해제
	delete[] vertices;
	vertices = nullptr;

	// 배열의 인스턴스 수 설정
	m_instanceCount = m_foliageCount;

	m_instances = new InstanceType[m_instanceCount];
	if (!m_instances)	return false;

	XMMATRIX matrix = XMMatrixIdentity();
	for (int i = 0; i < m_instanceCount; i++) {
		m_instances[i].matrix = matrix;
		m_instances[i].color = XMFLOAT3(m_foliageArray[i].r, m_foliageArray[i].g, m_foliageArray[i].b);
	}

	// 인스턴스 버퍼 서술자 설정
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = m_instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// 인스턴스 버퍼 생성
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))	return false;

	return true;
}

void FoliageClass::ShutdownBuffers()
{
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_instances) {
		delete[] m_instances;
		m_instances = 0;
	}
}

bool FoliageClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	if (!m_texture->Initialize(device, filename))	return false;

	return true;
}

void FoliageClass::ReleaseTexture()
{
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

bool FoliageClass::GeneratePositions()
{
	// 모든 단풍 정보를 저장할 배열 생성
	m_foliageArray = new FoliageType[m_foliageCount];
	if (!m_foliageArray)	return false;

	// 난수 생성
	srand(static_cast<int>(time(NULL)));

	// 각 단풍에 임의의 위치와 임의의 색상 설정
	for (int i = 0; i < m_foliageCount; i++) {
		m_foliageArray[i].x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 9.0f - 4.5f;
		m_foliageArray[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 9.0f - 4.5f;

		float red = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.0f;
		float green = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.0f;

		m_foliageArray[i].r = red + 1.0f;
		m_foliageArray[i].g = green + 0.5f;
		m_foliageArray[i].b = 0.0f;
	}

	return true;
}

void FoliageClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offset[2] = { 0, 0 };

	// 포인터의 배열을 정점 버퍼와 인스턴스 버퍼로 설정
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, stride, offset);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}