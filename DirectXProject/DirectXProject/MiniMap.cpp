#include "Stdafx.h"
#include "BitmapClass.h"
#include "TextureShaderClass.h"
#include "MiniMap.h"

MiniMapClass::MiniMapClass() {}
MiniMapClass::MiniMapClass(const MiniMapClass& other) {}
MiniMapClass::~MiniMapClass() {}

bool MiniMapClass::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX viewMatrix, float terrainWidth, float terrainHeight)
{
	// ȭ�鿡�� �̴ϸ��� ��ġ ����
	m_mapLocationX = 150;
	m_mapLocationY = 75;

	// �̴ϸ� ũ�� ����
	m_mapSizeX = 150.0f;
	m_mapSizeY = 150.0f;

	// ��Ʈ������ ���� ũ�� ����
	m_viewMatrix = viewMatrix;
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// �̴ϸ� ��ü ����
	m_miniMapBitmap = new BitmapClass;
	if (!m_miniMapBitmap)	return false;
	if (!m_miniMapBitmap->Initialize(device, screenWidth, screenHeight, L"./Textures/colorm01.dds", 150, 150)) {
		MessageBox(hwnd, L"Could not initialize the mini-map bitmap object", L"Error", MB_OK);
		return false;
	}

	// �̴ϸ� �׵θ� ��ü ����
	m_border = new BitmapClass;
	if (!m_border)	return false;
	if (!m_border->Initialize(device, screenWidth, screenHeight, L"./Textures/border01.dds", 154, 154)) {
		MessageBox(hwnd, L"Could not initialize the border object", L"Error", MB_OK);
		return false;
	}

	// �̴ϸ� ����Ʈ ��ü ����
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
	// �׵θ� ��Ʈ�� ������ �ε��� ���۸� �׷��� ���������ο� ��ġ�Ͽ� ������ �غ�
	if (!m_border->Render(deviceContext, m_mapLocationX - 2, m_mapLocationY - 2))	return false;

	// �ؽ�ó ���̴��� ����Ͽ� �׵θ� ������
	textureShader->Render(deviceContext, m_border->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_border->GetTexture());

	// �̴ϸ� ��Ʈ�� ������ �ε��� ���۸� �׷��� ���������ο� ��ġ�Ͽ� ������ �غ�
	if (!m_miniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY))	return false;

	// �ؽ�ó ���̴��� ����Ͽ� �̴ϸ� ������
	textureShader->Render(deviceContext, m_miniMapBitmap->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_miniMapBitmap->GetTexture());

	// ����Ʈ ��Ʈ�� ������ �ε��� ���۸� �׷��� ���������ο� ��ġ�Ͽ� ������ �غ�
	if (!m_point->Render(deviceContext, m_pointLocationX, m_pointLocationY))	return false;

	// �ؽ�ó ���̴��� ����Ͽ� ����Ʈ ������
	textureShader->Render(deviceContext, m_point->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_point->GetTexture());

	return true;
}

void MiniMapClass::PositionUpdate(float positionX, float positionZ)
{
	// ī�޶� ���� ��輱�� �����ĵ� ����Ʈ�� �̴ϸ� �׵θ��� ������ �ʴ��� Ȯ��
	if (positionX < 0)	positionX = 0;
	if (positionZ < 0)	positionZ = 0;
	if (positionX > m_terrainWidth)		positionX = m_terrainWidth;
	if (positionZ > m_terrainHeight)	positionZ = m_terrainHeight;

	// �̴ϸʿ��� ī�޶��� ��ġ�� ������� ���
	float percentX = positionX / m_terrainWidth;
	float percentY = 1.0f - (positionZ / m_terrainHeight);

	// �̴ϸʿ��� ����Ʈ�� �ȼ� ��ġ ����
	m_pointLocationX = m_mapLocationX + static_cast<int>(percentX * m_mapSizeX);
	m_pointLocationY = m_mapLocationY + static_cast<int>(percentY * m_mapSizeY);

	m_pointLocationX -= 1;
	m_pointLocationY -= 1;
}