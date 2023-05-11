#include "Stdafx.h"
#include "BitmapClass.h"
#include "TextureShaderClass.h"
#include "MiniMap.h"

MiniMapClass::MiniMapClass() {}
MiniMapClass::MiniMapClass(const MiniMapClass& other) {}
MiniMapClass::~MiniMapClass() {}

bool MiniMapClass::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX viewMatrix, float terrainWidth, float terrainHeight)
{
	// 화면에서 미니맵의 위치 설정
	m_mapLocationX = 150;
	m_mapLocationY = 75;

	// 미니맵 크기 설정
	m_mapSizeX = 150.0f;
	m_mapSizeY = 150.0f;

	// 매트릭스와 지형 크기 설정
	m_viewMatrix = viewMatrix;
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// 미니맵 객체 생성
	m_miniMapBitmap = new BitmapClass;
	if (!m_miniMapBitmap)	return false;
	if (!m_miniMapBitmap->Initialize(device, screenWidth, screenHeight, L"./Textures/colorm01.dds", 150, 150)) {
		MessageBox(hwnd, L"Could not initialize the mini-map bitmap object", L"Error", MB_OK);
		return false;
	}

	// 미니맵 테두리 객체 생성
	m_border = new BitmapClass;
	if (!m_border)	return false;
	if (!m_border->Initialize(device, screenWidth, screenHeight, L"./Textures/border01.dds", 154, 154)) {
		MessageBox(hwnd, L"Could not initialize the border object", L"Error", MB_OK);
		return false;
	}

	// 미니맵 포인트 객체 생성
	m_point = new BitmapClass;
	if (!m_point)	return false;
	if (!m_point->Initialize(device, screenWidth, screenHeight, L"./Textures/point01.dds", 3, 3)) {
		MessageBox(hwnd, L"Could not initialize the point object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void MiniMapClass::Shutdown()
{
	if (m_point) {
		m_point->Shutdown();
		delete m_point;
		m_point = 0;
	}

	if (m_border) {
		m_border->Shutdown();
		delete m_border;
		m_border = 0;
	}

	if (m_miniMapBitmap) {
		m_miniMapBitmap->Shutdown();
		delete m_miniMapBitmap;
		m_miniMapBitmap = 0;
	}
}

bool MiniMapClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix, TextureShaderClass* textureShader)
{
	// 테두리 비트맵 정점과 인덱스 버퍼를 그래픽 파이프라인에 배치하여 렌더링 준비
	if (!m_border->Render(deviceContext, m_mapLocationX - 2, m_mapLocationY - 2))	return false;

	// 텍스처 셰이더를 사용하여 테두리 렌더링
	textureShader->Render(deviceContext, m_border->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_border->GetTexture());

	// 미니맵 비트맵 정점과 인덱스 버퍼를 그래픽 파이프라인에 배치하여 렌더링 준비
	if (!m_miniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY))	return false;

	// 텍스처 셰이더를 사용하여 미니맵 렌더링
	textureShader->Render(deviceContext, m_miniMapBitmap->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_miniMapBitmap->GetTexture());

	// 포인트 비트맵 정점과 인덱스 버퍼를 그래픽 파이프라인에 배치하여 렌더링 준비
	if (!m_point->Render(deviceContext, m_pointLocationX, m_pointLocationY))	return false;

	// 텍스처 셰이더를 사용하여 포인트 렌더링
	textureShader->Render(deviceContext, m_point->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_point->GetTexture());

	return true;
}

void MiniMapClass::PositionUpdate(float positionX, float positionZ)
{
	// 카메라가 지형 경계선을 지나쳐도 포인트가 미니맵 테두리를 떠나지 않는지 확인
	if (positionX < 0)	positionX = 0;
	if (positionZ < 0)	positionZ = 0;
	if (positionX > m_terrainWidth)		positionX = m_terrainWidth;
	if (positionZ > m_terrainHeight)	positionZ = m_terrainHeight;

	// 미니맵에서 카메라의 위치를 백분율로 계산
	float percentX = positionX / m_terrainWidth;
	float percentY = 1.0f - (positionZ / m_terrainHeight);

	// 미니맵에서 포인트의 픽셀 위치 결정
	m_pointLocationX = m_mapLocationX + static_cast<int>(percentX * m_mapSizeX);
	m_pointLocationY = m_mapLocationY + static_cast<int>(percentY * m_mapSizeY);

	m_pointLocationX -= 1;
	m_pointLocationY -= 1;
}