#include "Stdafx.h"
#include "TextureClass.h"
#include "BitmapClass.h"

#include <fstream>
using namespace std;

BitmapClass::BitmapClass() {}
BitmapClass::BitmapClass(const BitmapClass& other) {}
BitmapClass:: ~BitmapClass() {}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	// 화면 크기를 멤버 변수에 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 렌더링 할 비트맵 픽셀의 크기를 저장
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 이전 렌더링 위치를 초기화
	m_previousPosX = -1;
	m_previousPosY = -1;

	if (!InitializeBuffers(device))	return false;

	if (!LoadTexture(device, textureFilename))	return false;

	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 화면의 다른 위치로 렌더링하기 위해 동적 정점 버퍼를 빌드
	if (!UpdateBuffers(deviceContext, positionX, positionY))	return false;

	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* BitmapClass::GetTexture() { return m_texture->GetTexture(); }

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture)	return false;

	return m_texture->Initialize(device, filename);
}


void BitmapClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열 및 인덱스 배열 크기 설정
	m_vertexCount = 6;
	m_indexCount = 6;

	// 정점 배열 생성 및 초기화
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열 생성 및 초기화
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;
	for (int i = 0; i < m_indexCount; i++)	indices[i] = i;

	// 동점 버퍼 서술자 설정
	// 동적 정점 버퍼를 위해 Usage를 DEFAULT에서 DYNAMIC으로 수정하고, CPUAccessFlags를 WRITE로 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 동점 버퍼 생성
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// 인덱스 버퍼 서술자 설정
	// 인덱스 버퍼는 정점의 위치가 바뀌어도 언제나 같은 정점을 가리키기 때문에 동적 버퍼로 만들 필요는 없음
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

void BitmapClass::ShutdownBuffers()
{
	//  인덱스 버퍼 해제
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 정점 버퍼 해제
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	// 정점 버퍼 자원에 자료를 올릴 수 있도록 포인터 생성
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	VertexType* verticesPtr;

	// 비트맵을 렌더링 할 위치가 변경될 때만 업데이트 수행
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))	return true;

	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 비트맵 좌표 계산
	// 정중앙을 (0, 0)으로 두어 상하좌우 계산
	left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);
	right = left + static_cast<float>(m_bitmapWidth);
	top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);
	bottom = top - static_cast<float>(m_bitmapHeight);

	// 정점 배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	vertices[0].position = XMFLOAT3(left, top, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[3].position = XMFLOAT3(left, top, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[4].position = XMFLOAT3(right, top, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 정점 버퍼 매핑
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;

	// 정점 버퍼에 데이터 복사
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼 메모리 해제
	deviceContext->Unmap(m_vertexBuffer, 0);
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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