#include "Stdafx.h"
#include "TextureClass.h"
#include "ModelTextureClass.h"

ModelTextureClass::ModelTextureClass() {}
ModelTextureClass::ModelTextureClass(const ModelTextureClass& other) {}
ModelTextureClass:: ~ModelTextureClass() {}

bool ModelTextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* textureFilename)
{
	if (!InitializeBuffers(device))	return false;
	return LoadTexture(device, deviceContext, textureFilename);
}

void ModelTextureClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelTextureClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelTextureClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* ModelTextureClass::GetTexture() { return m_Texture->GetTexture(); }

bool ModelTextureClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수
	m_vertexCount = 3;	// 삼각형
	m_vertexCount = 4;	// 사각형

	// 인덱스 배열의 인덱스 수
	m_indexCount = 3;	// 삼각형
	m_indexCount = 6;	// 사각형(정점 버퍼로 그릴 기본형이 삼각형이기 때문에 삼각형 2개를 놓으면 사각형이 된다)

	// 정점 배열 생성
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// 인덱스 배열 생성
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// 삼각형
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// 사각형
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

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
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelTextureClass::ShutdownBuffers()
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

void ModelTextureClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ModelTextureClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* filename)
{
	// 텍스쳐 객체 생성
	m_Texture = new TextureClass;
	if (!m_Texture)	return false;

	// 텍스쳐 객체 초기화
	return m_Texture->Initialize(device, deviceContext, filename);
}

void ModelTextureClass::ReleaseTexture()
{
	// 택스쳐 객체 릴리즈
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}