#include "Stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
//#include "ModelClass.h"
//#include "ColorShaderClass.h"
//#include "ModelTextureClass.h"
//#include "TextureShaderClass.h"
#include "ModelLightClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"

#include <iostream>
using namespace std;

GraphicsClass::GraphicsClass() {}
GraphicsClass::GraphicsClass(const GraphicsClass& other) {}
GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D ��ü ����
	//m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	m_Direct3D = new D3DClass;
	if (!m_Direct3D) return false;

	// Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// �׷���ī�� ���� ���� �����
	FILE* fp = nullptr;
	fopen_s(&fp, "���� ī�� ����.txt", "wt");
	if (fp != nullptr)
	{
		char cardName[255] = {};
		int memory = 0;
		m_Direct3D->GetVideoCardInfo(cardName, memory);
		fprintf_s(fp, "�׷��� ī�� �̸�: %s \n", cardName);
		fprintf_s(fp, "�׷��� ī�� �޸� ũ��: %d", memory);
		fclose(fp);
	}

	// ī�޶� ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera) { return false; }
	// ī�޶� ��ġ ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//// �� ��ü ����
	//m_Model = new ModelClass;
	//if (!m_Model) { return false; }
	//// �� ��ü �ʱ�ȭ
	//if (!m_Model->Initialize(m_Direct3D->GetDevice())) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ModelTexture = new ModelTextureClass;
	//if (!m_ModelTexture) { return false; }
	//// �ؽ��� �� ��ü �ʱ�ȭ
	//if (!m_ModelTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model texture object", L"Error", MB_OK);
	//	return false;
	//}

	m_ModelLight = new ModelLightClass;
	if (!m_ModelLight) { return false; }
	if (!m_ModelLight->Initialize(m_Direct3D->GetDevice(), L"./Textures/checkboard.dds")) {
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//// ���̴� ��ü ����
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader) { return false; }
	//// ���̴� ��ü �ʱ�ȭ
	//if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader) { return false; }
	//// �ؽ��� ���̴� ��ü �ʱ�ȭ
	//if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
	//	return false;
	//}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader) { return false; }
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass;
	if (!m_Light) { return false; }
	m_Light->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	// light ��ü ����
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ���̴� ��ü ��ȯ
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// �� ��ü ��ȯ
	if (m_ModelLight) {
		m_ModelLight->Shutdown();
		delete m_ModelLight;
		m_ModelLight = 0;
	}

	// ī�޶� ��ü ��ȯ
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D ��ü ��ȯ
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;

	// �� �������� ȸ���� ������Ʈ
	rotation += (float)XM_PI * 0.01f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// �׷��� ������ ó��
	return Render(rotation);
}

bool GraphicsClass::Render(float rotation)
{
	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ���� ȸ���� �� �ֵ��� ȸ�� ������ ���� ��� ����
	worldMatrix = XMMatrixRotationY(rotation);

	// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
	//m_Model->Render(m_Direct3D->GetDeviceContext());
	//m_ModelTexture->Render(m_Direct3D->GetDeviceContext());
	m_ModelLight->Render(m_Direct3D->GetDeviceContext());

	//// ���̴��� ����Ͽ� �� ������
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// �ؽ��� ���̴��� ����Ͽ� �� ������
	// if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;

	// �� ���̴��� ����Ͽ� �� ������
	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_ModelLight->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelLight->GetTexture(), 
		m_Light->GetDirection(), m_Light->GetDiffuseColor()))	return false;

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
} 