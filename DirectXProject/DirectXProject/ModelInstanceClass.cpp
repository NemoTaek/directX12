#include "Stdafx.h"
#include "TextureClass.h"
#include "ModelInstanceClass.h"

ModelInstanceClass::ModelInstanceClass() {}
ModelInstanceClass::ModelInstanceClass(const ModelInstanceClass& other) {}
ModelInstanceClass:: ~ModelInstanceClass() {}

bool ModelInstanceClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	if (!InitializeBuffers(device))	return false;
	return LoadTexture(device, textureFilename);
}

void ModelInstanceClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelInstanceClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelInstanceClass::GetVertexCount() { return m_vertexCount; }
int ModelInstanceClass::GetInstanceCount() { return m_instanceCount; }
ID3D11ShaderResourceView* ModelInstanceClass::GetTexture() { return m_texture->GetTexture(); }

bool ModelInstanceClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수
	m_vertexCount = 3;	// 삼각형

	// 정점 배열 생성
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// 삼각형
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

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

	// 생성 후 정점 및 인덱스 버퍼 해제
	delete[] vertices;
	vertices = nullptr;

	// 인스턴스 배열의 수
	m_instanceCount = 4;	// 4개 복사

	// 인스턴스 배열 생성
	InstanceType* instances = new InstanceType[m_instanceCount];
	if (!instances)	return false;

	// 삼각형
	instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	instances[1].position = XMFLOAT3(-1.5f, 1.5f, 5.0f);
	instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);

	// 인스턴스 버퍼 서술자 설정
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))	return false;

	// 생성 후 인스턴스 버퍼 해제
	delete[] instances;
	instances = nullptr;

	return true;
}

void ModelInstanceClass::ShutdownBuffers()
{
	// 인스턴스 버퍼 해제
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = nullptr;
	}

	// 정점셰이더 해제
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void ModelInstanceClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offset[2] = { 0, 0 };

	// 포인터의 배열을 정점 버퍼와 인스턴스 버퍼로 설정
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, stride, offset);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelInstanceClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐 객체 생성
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	if (!m_texture->Initialize(device, filename))	return false;

	return true;
}

void ModelInstanceClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}