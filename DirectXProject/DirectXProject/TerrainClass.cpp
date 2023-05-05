#include "Stdafx.h"
#include "TerrainClass.h"
#include <stdio.h>

TerrainClass::TerrainClass() {}
TerrainClass::TerrainClass(const TerrainClass& other) {}
TerrainClass::~TerrainClass() {}

bool TerrainClass::Initialize(ID3D11Device* device, const char* heightMapFilename)
{
	// 지형의 너비와 높이 맵 로드
	if (!LoadHeightMap(heightMapFilename))	return false;

	// 높이 맵의 높이 정규화
	NormalizeHeightMap();

	return InitializeBuffers(device);
}

void TerrainClass::Shutdown()
{
	ShutdownBuffers();
	ShutdownHeightMap();
}

void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int TerrainClass::GetIndexCount() { return m_indexCount; }

bool TerrainClass::LoadHeightMap(const char* heightMapFilename)
{
	// 높이 맵 파일 오픈
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, heightMapFilename, "rb") != 0)	return false;

	// 비트맵 파일 헤더 읽어옴
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// 지형 크기 저장
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// 비트맵 이미지 데이터 크기 계산
	int imageSize = m_terrainWidth * m_terrainHeight * 3;

	// 비트맵 이미지 데이터에 메모리 할당
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// 비트맵 데이터의 시작 부분으로 이동
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터 읽어옴
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// 높이 맵 데이터를 저장할 구조체 생성
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)	return false;

	// 이미지 데이터 버퍼의 위치 초기화
	int bufferPosition = 0;

	// 이미지 데이터를 높이 맵으로 읽어옴
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			unsigned char height = bitmapImage[bufferPosition];
			int index = (m_terrainHeight * i) + j;
			m_heightMap[index].x = static_cast<float>(j);
			m_heightMap[index].y = static_cast<float>(height);
			m_heightMap[index].z = static_cast<float>(i);
			bufferPosition += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void TerrainClass::NormalizeHeightMap()
{
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			int index = (m_terrainHeight * i) + j;
			m_heightMap[index].y /= 15.0f;
		}
	}
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;
	m_indexCount = m_vertexCount;

	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	int index = 0;

	for (int i = 0; i < (m_terrainHeight - 1); i++) {
		for (int j = 0; j < (m_terrainWidth - 1); j++) {
			int indexLeftBottom = (m_terrainHeight * i) + j;
			int indexRightBottom = (m_terrainHeight * i) + (j + 1);
			int indexLeftTop = (m_terrainHeight * (i + 1)) + j;
			int indexRightTop = (m_terrainHeight * (i + 1)) + (j + 1);

			// LINE 1
			// 좌상
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 우상
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 2
			// 우상
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 좌하
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 3
			// 좌하
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 좌상
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 4
			// 좌하
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 우상
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 5
			// 우상
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 우하
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 6
			// 우하
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// 좌하
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;
		}
	}

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

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap) {
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

void TerrainClass::ShutdownBuffers()
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

void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}