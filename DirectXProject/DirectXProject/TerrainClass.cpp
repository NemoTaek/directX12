#include "Stdafx.h"
#include "TerrainClass.h"
#include <stdio.h>

TerrainClass::TerrainClass() {}
TerrainClass::TerrainClass(const TerrainClass& other) {}
TerrainClass::~TerrainClass() {}

bool TerrainClass::Initialize(ID3D11Device* device, const char* heightMapFilename)
{
	// ������ �ʺ�� ���� �� �ε�
	if (!LoadHeightMap(heightMapFilename))	return false;

	// ���� ���� ���� ����ȭ
	NormalizeHeightMap();

	// ���� �������� ���� ���
	if (!CalculateNormals())	return false;

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
	// ���� �� ���� ����
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, heightMapFilename, "rb") != 0)	return false;

	// ��Ʈ�� ���� ��� �о��
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// ���� ũ�� ����
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// ��Ʈ�� �̹��� ������ ũ�� ���
	int imageSize = m_terrainWidth * m_terrainHeight * 3;

	// ��Ʈ�� �̹��� �����Ϳ� �޸� �Ҵ�
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// ��Ʈ�� �������� ���� �κ����� �̵�
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// ��Ʈ�� �̹��� ������ �о��
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// ���� �� �����͸� ������ ����ü ����
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)	return false;

	// �̹��� ������ ������ ��ġ �ʱ�ȭ
	int bufferPosition = 0;

	// �̹��� �����͸� ���� ������ �о��
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			unsigned char height = bitmapImage[bufferPosition];

			// ���� �� �迭�� ���� ��ġ�� ���� �ε���
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
			// ���� �� �迭�� ���� ��ġ�� ���� �ε���
			int index = (m_terrainHeight * i) + j;
			m_heightMap[index].y /= 15.0f;
		}
	}
}

bool TerrainClass::CalculateNormals()
{
	// 3���� ������ 2���� ����
	float vertex1[3] = { 0.0f, 0.0f, 0.0f };
	float vertex2[3] = { 0.0f, 0.0f, 0.0f };
	float vertex3[3] = { 0.0f, 0.0f, 0.0f };
	float vector1[3] = { 0.0f, 0.0f, 0.0f };
	float vector2[3] = { 0.0f, 0.0f, 0.0f };

	// ���� ���͸� ������ �ӽ� �迭
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)	return false;

	// �� �����Ϳ� ���� �ε��� �ʱ�ȭ
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;

	// ���� �� �迭�� ���� ��ġ�� ���� �ε���
	int index = 0;

	// ��� ���� ���캸�� ���� ���� ��� 
	for (int i = 0; i < m_terrainHeight - 1; i++) {
		for (int j = 0; j < m_terrainWidth - 1; j++) {
			index1 = (m_terrainHeight * i) + j;
			index2 = (m_terrainHeight * i) + (j + 1);
			index3 = (m_terrainHeight * (i + 1)) + j;

			// �鿡�� 3���� �������� ������
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// �鿡�� 2���� ���� ���
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];

			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = ((m_terrainHeight - 1) * i) + j;
			
			// ������ ���ϱ� ���� �� ������ ���� ���
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// ��հ��� ���ϱ� ���� ������ �� ��ǥ���� ��, ����, ����
	int count = 0;
	float sum[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;

	// ��� ���� ���캸�� �� ���� ��� ���
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;
			count = 0;

			// ���� ��
			if ((j - 1) >= 0 && (i - 1) >= 0) {
				index = ((m_terrainHeight - 1) * (i - 1)) + (j - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ���� ��
			if (j < (m_terrainWidth - 1) && (i - 1) >= 0) {
				index = ((m_terrainHeight - 1) * (i - 1)) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// �»� ��
			if ((j - 1) >= 0 && i < (m_terrainHeight - 1)) {
				index = ((m_terrainHeight - 1) * i) + (j - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ���� ��
			if (j < (m_terrainWidth - 1) && i < (m_terrainHeight - 1)) {
				index = ((m_terrainHeight - 1) * i) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ��� ���
			sum[0] = (sum[0] / static_cast<float>(count));
			sum[1] = (sum[1] / static_cast<float>(count));
			sum[2] = (sum[2] / static_cast<float>(count));

			// ���� ���� ���
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// ���� �� �迭�� ���� ��ġ�� ���� �ε���
			index = (m_terrainHeight * i) + j;

			// ���� ������ ���� ���� ������ ǥ��ȭ�Ͽ� ���� �� �迭�� ����
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	delete[] normals;
	normals = 0;

	return true;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
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

			// �»�
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftTop].nx, m_heightMap[indexLeftTop].ny, m_heightMap[indexLeftTop].nz);
			indices[index] = index;
			index++;

			// ���
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			indices[index] = index;
			index++;

			// ���
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[indexRightBottom].nx, m_heightMap[indexRightBottom].ny, m_heightMap[indexRightBottom].nz);
			indices[index] = index;
			index++;
		}
	}

	// ���� ���� ������ ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
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

void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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