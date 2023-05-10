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
	// 지형의 너비와 높이 맵 로드
	if (!LoadHeightMap(heightMapFilename))	return false;

	// 높이 맵의 높이 정규화
	NormalizeHeightMap();

	// 지형 데이터의 법선 계산
	if (!CalculateNormals())	return false;

	// 텍스처 좌표를 계산
	//CalculateTextureCoordinates();

	// 지형 텍스처 로드
	//if (!LoadTexture(device, textureFilename))	return false;

	// 컬러 맵을 지형에 로드
	if (!LoadColorMap(colorMapFilename))	return false;

	// 지형에 대한 머터리얼 그룹 로드
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

			// 높이 맵 배열의 정점 위치에 대한 인덱스
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
			// 높이 맵 배열의 정점 위치에 대한 인덱스
			int index = (m_terrainHeight * i) + j;
			m_heightMap[index].y /= 15.0f;
		}
	}
}

bool TerrainClass::CalculateNormals()
{
	// 3개의 정점과 2개의 벡터
	float vertex1[3] = { 0.0f, 0.0f, 0.0f };
	float vertex2[3] = { 0.0f, 0.0f, 0.0f };
	float vertex3[3] = { 0.0f, 0.0f, 0.0f };
	float vector1[3] = { 0.0f, 0.0f, 0.0f };
	float vector2[3] = { 0.0f, 0.0f, 0.0f };

	// 법선 벡터를 저장할 임시 배열
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)	return false;

	// 모델 데이터에 대한 인덱스 초기화
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;

	// 높이 맵 배열의 정점 위치에 대한 인덱스
	int index = 0;

	// 모든 면을 살펴보고 법선 벡터 계산 
	for (int i = 0; i < m_terrainHeight - 1; i++) {
		for (int j = 0; j < m_terrainWidth - 1; j++) {
			index1 = (m_terrainHeight * i) + j;
			index2 = (m_terrainHeight * i) + (j + 1);
			index3 = (m_terrainHeight * (i + 1)) + j;

			// 면에서 3개의 꼭지점을 가져옴
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 면에서 2개의 벡터 계산
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];

			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = ((m_terrainHeight - 1) * i) + j;
			
			// 법선을 구하기 위해 두 벡터의 외적 계산
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// 평균값을 구하기 위한 법선의 각 좌표값의 합, 개수, 길이
	int count = 0;
	float sum[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;

	// 모든 면을 살펴보고 각 면의 평균 계산
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;
			count = 0;

			// 좌하 면
			if ((j - 1) >= 0 && (i - 1) >= 0) {
				index = ((m_terrainHeight - 1) * (i - 1)) + (j - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 우하 면
			if (j < (m_terrainWidth - 1) && (i - 1) >= 0) {
				index = ((m_terrainHeight - 1) * (i - 1)) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 좌상 면
			if ((j - 1) >= 0 && i < (m_terrainHeight - 1)) {
				index = ((m_terrainHeight - 1) * i) + (j - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 우하 면
			if (j < (m_terrainWidth - 1) && i < (m_terrainHeight - 1)) {
				index = ((m_terrainHeight - 1) * i) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 평균 계산
			sum[0] = (sum[0] / static_cast<float>(count));
			sum[1] = (sum[1] / static_cast<float>(count));
			sum[2] = (sum[2] / static_cast<float>(count));

			// 법선 길이 계산
			length = static_cast<float>(sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2])));

			// 높이 맵 배열의 정점 위치에 대한 인덱스
			index = (m_terrainHeight * i) + j;

			// 현재 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장
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
	// 텍스처 좌표를 얼마나 증가시킬지 계산
	float incrementValue = static_cast<float>(TEXTURE_REPEAT) / static_cast<float>(m_terrainWidth);

	// 텍스처를 얼마나 반복시킬지 계산
	int incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;
	int tuCount = 0;
	int tvCount = 0;

	// 전체 높이 맵을 반복하고 각 정점의 tu, tv 좌표 계산
	for (int i = 0; i < m_terrainHeight; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			// 높이 맵에 텍스처 좌표 저장
			m_heightMap[(m_terrainHeight * i) + j].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * i) + j].tv = tvCoordinate;

			// tu 텍스처 좌표 및 개수 증가
			tuCoordinate += incrementValue;
			tuCount++;

			// 마지막 차례(오른 쪽 끝)면 초기화
			if (tuCount == incrementCount) {
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv 텍스처 좌표 및 개수 증가
		tvCoordinate -= incrementValue;
		tvCount++;

		// 마지막 차례(최상단)면 초기화
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
	// 컬러 맵 파일 오픈
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, colorMapfilename, "rb") != 0)	return false;

	// 비트맵 파일 헤더 읽어옴
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// 컬러 맵 크기 저장
	int colorMapWidth = bitmapInfoHeader.biWidth;
	int colorMapHeight = bitmapInfoHeader.biHeight;
	if (colorMapWidth != m_terrainWidth || colorMapHeight != m_terrainHeight)	return false;

	// 비트맵 이미지 데이터 크기 계산
	int imageSize = colorMapWidth * colorMapHeight * 3;

	// 비트맵 이미지 데이터에 메모리 할당
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// 비트맵 데이터의 시작 부분으로 이동
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터 읽어옴
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// 이미지 데이터 버퍼의 위치 초기화
	int bufferPosition = 0;

	// 이미지 데이터를 높이 맵 구조의 컬러 맵 부분으로 읽어옴
	for (int i = 0; i < colorMapHeight; i++) {
		for (int j = 0; j < colorMapWidth; j++) {
			// 높이 맵 배열의 정점 위치에 대한 인덱스
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
	// 텍스처 정보 텍스트 파일 오픈
	fin.open(filename);
	if (fin.fail())	return false;

	// 텍스처 카운트 값까지 읽음
	char input = 0;
	fin.get(input);
	while (input != ':')	fin.get(input);

	// 텍스처 수
	fin >> m_textureCount;

	// 텍스처 오브젝트 배열 생성
	m_texture = new TextureClass[m_textureCount];
	if (!m_texture)	return false;

	// 각 텍스처 로드
	for (int i = 0; i < m_textureCount; i++) {
		fin.get(input);
		while (input != ':')	fin.get(input);
		fin >> inputFilename;

		// 문자 파일 이름을 WCHAR로 변환
		if (mbstowcs_s(&stringLength, textureFilename, 128, inputFilename, 128) != 0)	return false;

		// 텍스처 또는 알파맵 로드
		if (!m_texture[i].Initialize(device, textureFilename))	return false;
	}

	// 머터리얼 카운트
	fin.get(input);
	while (input != ':')	fin.get(input);
	fin >> m_materialCount;

	// 머터리얼 그룹 배열 생성
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

	// 머터리얼 인덱스 맵 로드
	if (!LoadMaterialMap(materialMapFilename))	return false;

	// 머터리얼 정점 버퍼 로드
	if (!LoadMaterialBuffers(device))	return false;

	return true;
}

bool TerrainClass::LoadMaterialMap(const char* materialMapFilename)
{
	// 높이 맵 파일 오픈
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, materialMapFilename, "rb") != 0)	return false;

	// 비트맵 파일 헤더 읽어옴
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)	return false;

	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)	return false;

	// 지형 크기 저장
	int materialMapWidth = bitmapInfoHeader.biWidth;
	int materialMapHeight = bitmapInfoHeader.biHeight;
	if (materialMapWidth != m_terrainWidth || materialMapHeight != m_terrainHeight)	return false;

	// 비트맵 이미지 데이터 크기 계산
	int imageSize = materialMapWidth * materialMapHeight * 3;

	// 비트맵 이미지 데이터에 메모리 할당
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)	return false;

	// 비트맵 데이터의 시작 부분으로 이동
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터 읽어옴
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)	return false;

	if (fclose(filePtr) != 0)	return false;

	// 이미지 데이터 버퍼의 위치 초기화
	int bufferPosition = 0;

	// 이미지 데이터를 높이 맵으로 읽어옴
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

			// 현재 정점이 속하는 재료 그룹 찾기
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

			// 정점 및 인덱스 배열의 인덱스 위치를 개수로 설정
			int vIndex = m_material[index].vertexCount;

			// 좌상
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftTop].nx, m_heightMap[indexLeftTop].ny, m_heightMap[indexLeftTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftTop].r, m_heightMap[indexLeftTop].g, m_heightMap[indexLeftTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// 우상
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(1.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// 좌하
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 1.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// 좌하
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(0.0f, 1.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// 우상
			m_material[index].vertices[vIndex].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_material[index].vertices[vIndex].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_material[index].vertices[vIndex].texture = XMFLOAT2(1.0f, 0.0f);
			m_material[index].vertices[vIndex].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			m_material[index].indices[vIndex] = vIndex;
			vIndex++;

			// 우하
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
		// 정점 버퍼 서술자 설정
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_material[i].vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = m_material[i].vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// 정점 버퍼 생성
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_material[i].vertexBuffer)))	return false;

		// 인덱스 버퍼 서술자 설정
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_material[i].indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// 인덱스 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = m_material[i].indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// 인덱스 버퍼 생성
		if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_material[i].indexBuffer)))	return false;

		// 생성 후 정점 및 인덱스 버퍼 해제
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

			// 좌상
			tu = m_heightMap[indexLeftTop].tu;
			tv = m_heightMap[indexLeftTop].tv;
			if (tv == 1.0f) tv = 0.0f;	// 상단 가장자리를 덮도록 텍스처 좌표 수정

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftTop].x, m_heightMap[indexLeftTop].y, m_heightMap[indexLeftTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftTop].nx, m_heightMap[indexLeftTop].ny, m_heightMap[indexLeftTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftTop].r, m_heightMap[indexLeftTop].g, m_heightMap[indexLeftTop].b, 1.0f);
			indices[index] = index;
			index++;

			// 우상
			tu = m_heightMap[indexRightTop].tu;
			tv = m_heightMap[indexRightTop].tv;
			if (tu == 0.0f) tu = 1.0f;	// 우측 가장자리를 덮도록 텍스처 좌표 수정
			if (tv == 1.0f) tv = 0.0f;	// 상단 가장자리를 덮도록 텍스처 좌표 수정

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			indices[index] = index;
			index++;

			// 좌하
			tu = m_heightMap[indexLeftBottom].tu;
			tv = m_heightMap[indexLeftBottom].tv;

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			indices[index] = index;
			index++;

			// 좌하
			tu = m_heightMap[indexLeftBottom].tu;
			tv = m_heightMap[indexLeftBottom].tv;

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexLeftBottom].x, m_heightMap[indexLeftBottom].y, m_heightMap[indexLeftBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexLeftBottom].nx, m_heightMap[indexLeftBottom].ny, m_heightMap[indexLeftBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexLeftBottom].r, m_heightMap[indexLeftBottom].g, m_heightMap[indexLeftBottom].b, 1.0f);
			indices[index] = index;
			index++;

			// 우상
			tu = m_heightMap[indexRightTop].tu;
			tv = m_heightMap[indexRightTop].tv;
			if (tu == 0.0f) tu = 1.0f;	// 우측 가장자리를 덮도록 텍스처 좌표 수정
			if (tv == 1.0f) tv = 0.0f;	// 상단 가장자리를 덮도록 텍스처 좌표 수정

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightTop].x, m_heightMap[indexRightTop].y, m_heightMap[indexRightTop].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightTop].nx, m_heightMap[indexRightTop].ny, m_heightMap[indexRightTop].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightTop].r, m_heightMap[indexRightTop].g, m_heightMap[indexRightTop].b, 1.0f);
			indices[index] = index;
			index++;

			// 우하
			tu = m_heightMap[indexRightBottom].tu;
			tv = m_heightMap[indexRightBottom].tv;
			if (tu == 0.0f) tu = 1.0f;	// 우측 가장자리를 덮도록 텍스처 좌표 수정

			m_vertices[index].position = XMFLOAT3(m_heightMap[indexRightBottom].x, m_heightMap[indexRightBottom].y, m_heightMap[indexRightBottom].z);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[indexRightBottom].nx, m_heightMap[indexRightBottom].ny, m_heightMap[indexRightBottom].nz);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].color = XMFLOAT4(m_heightMap[indexRightBottom].r, m_heightMap[indexRightBottom].g, m_heightMap[indexRightBottom].b, 1.0f);
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool TerrainClass::RenderMaterials(ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	if (!shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection, NULL))	return false;

	for (int i = 0; i < m_materialCount; i++) {
		// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
		deviceContext->IASetVertexBuffers(0, 1, &m_material[i].vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(m_material[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// 머터리얼 그룹에 텍스처 인덱스가 있으면 알파블렌딩 처리 적용
		if (m_material[i].textureIndex2 != -1) {
			if (!shader->SetShaderTextures(deviceContext, m_texture[m_material[i].textureIndex1].GetTexture(), m_texture[m_material[i].textureIndex2].GetTexture(), 
				m_texture[m_material[i].alphaIndex].GetTexture(), true))	return false;
		}
		else {
			if (!shader->SetShaderTextures(deviceContext, m_texture[m_material[i].textureIndex1].GetTexture(), NULL, NULL, false))	return false;
		}

		shader->RenderShader(deviceContext, m_material[i].indexCount);
	}

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}