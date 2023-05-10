#include "Stdafx.h"
#include "TerrainClass.h"
#include "TerrainShaderClass.h"
#include "TextureClass.h"
#include <stdlib.h>
#include <fstream>
using namespace std;

TerrainClass::TerrainClass() {}
TerrainClass::TerrainClass(const TerrainClass& other) {}
TerrainClass::~TerrainClass() {}

bool TerrainClass::Initialize(ID3D11Device* device, const char* heightMapFilename, const char* materialsFilename, const char* materialMapFilename, const char* colorMapFilename)
{
	// ������ �ʺ�� ���� �� �ε�
	if (!LoadHeightMap(heightMapFilename))	return false;

	// ���� ���� ���� ����ȭ
	NormalizeHeightMap();

	// ���� �������� ���� ���
	if (!CalculateNormals())	return false;

	// �ؽ�ó ��ǥ�� ���
	//CalculateTextureCoordinates();

	// ���� �ؽ�ó �ε�
	//if (!LoadTexture(device, textureFilename))	return false;

	// �÷� ���� ������ �ε�
	if (!LoadColorMap(colorMapFilename))	return false;

	// ������ ���� ���͸��� �׷� �ε�
	if (!LoadMaterialFile(materialsFilename, materialMapFilename, device))	return false;

	return InitializeBuffers(device);
}

void TerrainClass::Shutdown()
{
	//ReleaseTexture();
	//ShutdownBuffers();
	ReleaseMaterials();
	ShutdownHeightMap();
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

int TerrainClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* TerrainClass::GetTexture() { return m_texture->GetTexture(); }

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
			length = static_cast<float>(sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2])));

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

void TerrainClass::CalculateTextureCoordinates()
{
	// �ؽ�ó ��ǥ�� �󸶳� ������ų�� ���
	float incrementValue = static_cast<float>(TEXTURE_REPEAT) / static_cast<float>(m_terrainWidth);

	// �ؽ�ó�� �󸶳� �ݺ���ų�� ���
	int incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;
	int tuCount = 0;
	int tvCount = 0;

	// ��ü ���� ���� �ݺ��ϰ� �� ������ tu, tv ��ǥ ���
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			// ���� �ʿ� �ؽ�ó ��ǥ ����
			m_heightMap[(m_terrainHeight * i) + j].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * i) + j].tv = tvCoordinate;

			// tu �ؽ�ó ��ǥ �� ���� ����
			tuCoordinate += incrementValue;
			tuCount++;

			// ������ ����(���� �� ��)�� �ʱ�ȭ
			if (tuCount == incrementCount) {
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv �ؽ�ó ��ǥ �� ���� ����
		tvCoordinate -= incrementValue;
		tvCount++;

		// ������ ����(�ֻ��)�� �ʱ�ȭ
		if (tvCount == incrementCount) {
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

bool TerrainClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	return m_texture->Initialize(device, filename);
}

bool TerrainClass::LoadColorMap(const char* colorMapfilename)
{
	// �÷� �� ���� ����
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, colorMapfilename, "rb") != 0)	return false;

	// ��Ʈ�� ���� ��� �о��
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// �÷� �� ũ�� ����
	int colorMapWidth = bitmapInfoHeader.biWidth;
	int colorMapHeight = bitmapInfoHeader.biHeight;
	if (colorMapWidth != m_terrainWidth || colorMapHeight != m_terrainHeight)	return false;

	// ��Ʈ�� �̹��� ������ ũ�� ���
	int imageSize = colorMapWidth * colorMapHeight * 3;

	// ��Ʈ�� �̹��� �����Ϳ� �޸� �Ҵ�
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// ��Ʈ�� �������� ���� �κ����� �̵�
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// ��Ʈ�� �̹��� ������ �о��
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// �̹��� ������ ������ ��ġ �ʱ�ȭ
	int bufferPosition = 0;

	// �̹��� �����͸� ���� �� ������ �÷� �� �κ����� �о��
	for (int i = 0; i < colorMapHeight; i++) {
		for (int j = 0; j < colorMapWidth; j++) {
			// ���� �� �迭�� ���� ��ġ�� ���� �ε���
			int index = (colorMapHeight * i) + j;

			m_heightMap[index].r = static_cast<float>(bitmapImage[bufferPosition + 2]) / 255.0f;
			m_heightMap[index].g = static_cast<float>(bitmapImage[bufferPosition + 1]) / 255.0f;
			m_heightMap[index].b = static_cast<float>(bitmapImage[bufferPosition]) / 255.0f;

			bufferPosition += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool TerrainClass::LoadMaterialFile(const char* filename, const char* materialMapFilename, ID3D11Device* device)
{
	size_t stringLength = 0;
	char inputFilename[128] = { 0, };
	wchar_t textureFilename[128] = { 0, };

	ifstream fin;
	// �ؽ�ó ���� �ؽ�Ʈ ���� ����
	fin.open(filename);
	if (fin.fail())	return false;

	// �ؽ�ó ī��Ʈ ������ ����
	char input = 0;
	fin.get(input);
	while (input != ':')	fin.get(input);

	// �ؽ�ó ��
	fin >> m_textureCount;

	// �ؽ�ó ������Ʈ �迭 ����
	m_texture = new TextureClass[m_textureCount];
	if (!m_texture)	return false;

	// �� �ؽ�ó �ε�
	for (int i = 0; i < m_textureCount; i++) {
		fin.get(input);
		while (input != ':')	fin.get(input);
		fin >> inputFilename;

		// ���� ���� �̸��� WCHAR�� ��ȯ
		if (mbstowcs_s(&stringLength, textureFilename, 128, inputFilename, 128) != 0)	return false;

		// �ؽ�ó �Ǵ� ���ĸ� �ε�
		if (!m_texture[i].Initialize(device, textureFilename))	return false;
	}

	// ���͸��� ī��Ʈ
	fin.get(input);
	while (input != ':')	fin.get(input);
	fin >> m_materialCount;

	// ���͸��� �׷� �迭 ����
	m_material = new MaterialGroupType[m_materialCount];
	if (!m_material)	return false;

	for (int i = 0; i < m_materialCount; i++) {
		m_material[i].vertexBuffer = 0;
		m_material[i].indexBuffer = 0;
		m_material[i].vertices = 0;
		m_material[i].indices = 0;
	}

	for (int i = 0; i < m_materialCount; i++) {
		fin.get(input);
		while (input != ':')	fin.get(input);
		fin >> m_material[i].textureIndex1 >> m_material[i].textureIndex2 >> m_material[i].alphaIndex;
		fin >> m_material[i].red >> m_material[i].green >> m_material[i].blue;
	}

	fin.close();

	// ���͸��� �ε��� �� �ε�
	if (!LoadMaterialMap(materialMapFilename))	return false;

	// ���͸��� ���� ���� �ε�
	if (!LoadMaterialBuffers(device))	return false;

	return true;
}

bool TerrainClass::LoadMaterialMap(const char* materialMapFilename)
{
	// ���� �� ���� ����
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, materialMapFilename, "rb") != 0)	return false;

	// ��Ʈ�� ���� ��� �о��
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// ���� ũ�� ����
	int materialMapWidth = bitmapInfoHeader.biWidth;
	int materialMapHeight = bitmapInfoHeader.biHeight;
	if (materialMapWidth != m_terrainWidth || materialMapHeight != m_terrainHeight)	return false;

	// ��Ʈ�� �̹��� ������ ũ�� ���
	int imageSize = materialMapWidth * materialMapHeight * 3;

	// ��Ʈ�� �̹��� �����Ϳ� �޸� �Ҵ�
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// ��Ʈ�� �������� ���� �κ����� �̵�
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// ��Ʈ�� �̹��� ������ �о��
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// �̹��� ������ ������ ��ġ �ʱ�ȭ
	int bufferPosition = 0;

	// �̹��� �����͸� ���� ������ �о��
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			int index = (m_terrainHeight * i) + j;

			m_heightMap[index].rIndex = static_cast<int>(bitmapImage[bufferPosition + 2]);
			m_heightMap[index].gIndex = static_cast<int>(bitmapImage[bufferPosition + 1]);
			m_heightMap[index].bIndex = static_cast<int>(bitmapImage[bufferPosition]);

			bufferPosition += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool TerrainClass::LoadMaterialBuffers(ID3D11Device* device)
{
	int maxVertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	int maxIndexCount = maxVertexCount;

	for (int i = 0; i < m_materialCount; i++) {
		m_material[i].vertices = new VertexType[maxVertexCount];
		if (!m_material[i].vertices)	return false;

		m_material[i].indices = new unsigned long[maxIndexCount];
		if (!m_material[i].indices)	return false;

		m_material[i].vertexCount = 0;
		m_material[i].indexCount = 0;
	}

	for (int i = 0; i < (m_terrainHeight - 1); i++) {
		for (int j = 0; j < (m_terrainWidth - 1); j++) {
			int indexLeftBottom = (m_terrainHeight * i) + j;
			int indexRightBottom = (m_terrainHeight * i) + (j + 1);
			int indexLeftTop = (m_terrainHeight * (i + 1)) + j;
			int indexRightTop = (m_terrainHeight * (i + 1)) + (j + 1);

			int redIndex = m_heightMap[indexLeftTop].rIndex;
			int greenIndex = m_heightMap[indexLeftTop].gIndex;
			int blueIndex = m_heightMap[indexLeftTop].bIndex;

			// ���� ������ ���ϴ� ��� �׷� ã��
			int index = 0;
			int found = false;
			while (!found) {
				if (redIndex == m_material[index].red && redIndex == m_material[index].green && redIndex == m_material[index].blue) {
					found = true;
				}
				else {
					index++;
				}
			}

			// ���� �� �ε��� �迭�� �ε��� ��ġ�� ������ ����
			int vIndex = m_material[index].vertexCount;

			// �»�
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftTop].nx, m_heightMap[indexLeftTop].ny, m_heightMap[indexLeftTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftTop].r, m_heightMap[indexLeftTop].g, m_heightMap[indexLeftTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// ���
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(1.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// ����
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 1.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// ����
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 1.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// ���
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(1.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// ����
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexRightBottom].nx, m_heightMap[indexRightBottom].ny, m_heightMap[indexRightBottom].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(1.0f, 1.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexRightBottom].r, m_heightMap[indexRightBottom].g, m_heightMap[indexRightBottom].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			m_material[index].vertexCount += 6;
			m_material[index].indexCount += 6;
		}
	}

	for (int i = 0; i < m_materialCount; i++) {
		// ���� ���� ������ ����
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_material[i].vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = m_material[i].vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// ���� ���� ����
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_material[i].vertexBuffer)))	return false;

		// �ε��� ���� ������ ����
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_material[i].indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// �ε��� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = m_material[i].indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// �ε��� ���� ����
		if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_material[i].indexBuffer)))	return false;

		// ���� �� ���� �� �ε��� ���� ����
		delete[] m_material[i].vertices;
		m_material[i].vertices = 0;
		delete[] m_material[i].indices;
		m_material[i].indices = 0;
	}

	return true;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	float tu = 0.0f;
	float tv = 0.0f;

	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	m_indexCount = m_vertexCount;

	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)	return false;

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
			tu = m_heightMap[indexLeftTop].tu;
			tv = m_heightMap[indexLeftTop].tv;
			if (tv == 1.0f) tv = 0.0f;	// ��� �����ڸ��� ������ �ؽ�ó ��ǥ ����

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftTop].nx, m_heightMap[indexLeftTop].ny, m_heightMap[indexLeftTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftTop].r, m_heightMap[indexLeftTop].g, m_heightMap[indexLeftTop].b, 1.0f);
			indices[index] = index;
			index++;

			// ���
			tu = m_heightMap[indexRightTop].tu;
			tv = m_heightMap[indexRightTop].tv;
			if (tu == 0.0f) tu = 1.0f;	// ���� �����ڸ��� ������ �ؽ�ó ��ǥ ����
			if (tv == 1.0f) tv = 0.0f;	// ��� �����ڸ��� ������ �ؽ�ó ��ǥ ����

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			indices[index] = index;
			index++;

			// ����
			tu = m_heightMap[indexLeftBottom].tu;
			tv = m_heightMap[indexLeftBottom].tv;

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			indices[index] = index;
			index++;

			// ����
			tu = m_heightMap[indexLeftBottom].tu;
			tv = m_heightMap[indexLeftBottom].tv;

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			indices[index] = index;
			index++;

			// ���
			tu = m_heightMap[indexRightTop].tu;
			tv = m_heightMap[indexRightTop].tv;
			if (tu == 0.0f) tu = 1.0f;	// ���� �����ڸ��� ������ �ؽ�ó ��ǥ ����
			if (tv == 1.0f) tv = 0.0f;	// ��� �����ڸ��� ������ �ؽ�ó ��ǥ ����

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			indices[index] = index;
			index++;

			// ����
			tu = m_heightMap[indexRightBottom].tu;
			tv = m_heightMap[indexRightBottom].tv;
			if (tu == 0.0f) tu = 1.0f;	// ���� �����ڸ��� ������ �ؽ�ó ��ǥ ����

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightBottom].nx, m_heightMap[indexRightBottom].ny, m_heightMap[indexRightBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightBottom].r, m_heightMap[indexRightBottom].g, m_heightMap[indexRightBottom].b, 1.0f);
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
	delete[] m_vertices;
	m_vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

int TerrainClass::GetVertexCount() { return m_vertexCount; }

void TerrainClass::CopyVertexArray(void* vertexList) { memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount); }

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap) {
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

void TerrainClass::ReleaseTexture()
{
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

void TerrainClass::ReleaseMaterials()
{
	if (m_material) {
		for (int i = 0; i < m_materialCount; i++) {
			if (m_material[i].vertexBuffer) {
				m_material[i].vertexBuffer->Release();
				m_material[i].vertexBuffer = 0;
			}
			if (m_material[i].indexBuffer) {
				m_material[i].indexBuffer->Release();
				m_material[i].indexBuffer = 0;
			}
			if (m_material[i].vertices) {
				delete[] m_material[i].vertices;
				m_material[i].vertices = 0;
			}
			if (m_material[i].indices) {
				delete[] m_material[i].indices;
				m_material[i].indices = 0;
			}
		}
		delete[] m_material;
		m_material = 0;
	}

	if (m_texture) {
		for (int i = 0; i < m_textureCount; i++) {
			m_texture[i].Shutdown();
		}

		delete [] m_texture;
		m_texture = 0;
	}
}

void TerrainClass::ShutdownBuffers()
{
	//if (m_vertices) {
	//	delete[] m_vertices;
	//	m_vertices = 0;
	//}

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

bool TerrainClass::RenderMaterials(ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection)
{
	// ���� ������ ������ ������ ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	if (!shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection, NULL))	return false;

	for (int i = 0; i < m_materialCount; i++) {
		// ������ �� �� �ֵ��� �Է� ��������� ���� �� �ε��� ���۸� Ȱ������ ����
		deviceContext->IASetVertexBuffers(0, 1, &m_material[i].vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(m_material[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// ���͸��� �׷쿡 �ؽ�ó �ε����� ������ ���ĺ��� ó�� ����
		if (m_material[i].textureIndex2 != -1) {
			if (!shader->SetShaderTextures(deviceContext, m_texture[m_material[i].textureIndex1].GetTexture(), m_texture[m_material[i].textureIndex2].GetTexture(), 
				m_texture[m_material[i].alphaIndex].GetTexture(), true))	return false;
		}
		else {
			if (!shader->SetShaderTextures(deviceContext, m_texture[m_material[i].textureIndex1].GetTexture(), NULL, NULL, false))	return false;
		}

		shader->RenderShader(deviceContext, m_material[i].indexCount);
	}

	// ���� ���۷� �׸� �⺻�� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}