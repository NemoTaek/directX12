#include "Stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
//#include "ModelClass.h"
//#include "ColorShaderClass.h"
//#include "ModelTextureClass.h"
//#include "TextureShaderClass.h"
//#include "ModelLightClass.h"
#include "Model3DClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
//#include "BitmapClass.h"
#include "TextClass.h"
#include "FrustumClass.h"
#include "ModelListClass.h"

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
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

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

	//m_ModelLight = new ModelLightClass;
	//if (!m_ModelLight) { return false; }
	//if (!m_ModelLight->Initialize(m_Direct3D->GetDevice(), L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	m_Model3D = new Model3DClass;
	if (!m_Model3D) { return false; }
	if (!m_Model3D->Initialize(m_Direct3D->GetDevice(), L"./data/sphere.txt", L"./Textures/checkboard.dds")) {
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
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	//// 2D �� ��ü ����
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap) { return false; }
	//// 2D �� ��ü �ʱ�ȭ
	//if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, L"./Textures/ice.dds", 256, 256)) {
	//	MessageBox(hwnd, L"Could not initialize the bitmap object", L"Error", MB_OK);
	//	return false;
	//}

	// �ؽ�Ʈ ��ü ����
	m_Text = new TextClass;
	if (!m_Text) { return false; }
	// �ؽ�Ʈ ��ü �ʱ�ȭ
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix)) {
		MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
		return false;
	}

	// ���� �� ����Ʈ ��ü ����
	m_ModelList = new ModelListClass;
	if (!m_ModelList) { return false; }
	// ���� �� ����Ʈ ��ü �ʱ�ȭ
	if (!m_ModelList->Initialize(25)) {
		MessageBox(hwnd, L"Could not initialize the model list object", L"Error", MB_OK);
		return false;
	}

	// ����ü ��ü ����
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }

	return true;
}

void GraphicsClass::Shutdown()
{
	// ����ü ��ü ��ȯ
	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

	// ���� �� ����Ʈ ��ü ��ȯ
	if (m_ModelList) {
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// �ؽ�Ʈ ��ü ��ȯ
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// ��Ʈ�� ��ü ��ȯ
	//if (m_Bitmap) {
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = 0;
	//}

	// light ��ü ��ȯ
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

	// ���̴� ��ü ��ȯ
	//if (m_TextureShader) {
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}

	// �� ��ü ��ȯ
	//if (m_ModelLight) {
	//	m_ModelLight->Shutdown();
	//	delete m_ModelLight;
	//	m_ModelLight = 0;
	//}

	// �� ��ü ��ȯ
	if (m_Model3D) {
		m_Model3D->Shutdown();
		delete m_Model3D;
		m_Model3D = 0;
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

//bool GraphicsClass::Frame(int mouseX, int mouseY, int keyCount, int fps, int cpu, float frameTime)
bool GraphicsClass::Frame(float rotationY)
{
	// �� ȸ���� �ڵ�
	/*
	static float rotation = 0.0f;

	// �� �������� ȸ���� ������Ʈ
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// �׷��� ������ ó��
	return Render(rotation);
	*/

	// Input �� �ڵ�
	//if (!m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetKeyboardInput(keyCount, m_Direct3D->GetDeviceContext()))	return false;

	// fps, cpu, timer �� �ڵ�
	//if(!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))	return false;

	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	// �������� �� ����
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f;
	XMFLOAT4 color;

	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// �� �����Ӹ��� �þ� ��Ŀ� �ٰ��Ͽ� ����ü�� ����
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// ������ �� ���� �� ���� �� �ʱ�ȭ
	int modelCount = m_ModelList->GetModelCount();
	int renderCount = 0;

	// ��� ���� Ž���ϰ�, ī�޶� �信 �������� �𵨸� ������
	for (int i = 0; i < modelCount; i++) {
		// �� ��ġ�� ���� ����
		m_ModelList->GetData(i, positionX, positionY, positionZ, color);
		
		// ���� ����ü �ȿ� �ִ��� Ȯ��
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius)) {
			// ���� ������ �� ��ġ�� �̵�
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			
			// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
			m_Model3D->Render(m_Direct3D->GetDeviceContext());

			// �� ���̴��� ����Ͽ� �� ������
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

			// ���� ��� ����
			m_Direct3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	// ���� ȸ���� �� �ֵ��� ȸ�� ������ ���� ��� ����
	//worldMatrix = XMMatrixRotationY(rotation);

	// 2D �������� ���� Z ���� OFF
	m_Direct3D->TurnZBufferOff();

	// ���� ���� on
	m_Direct3D->TurnOnAlphaBlending();

	// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
	//m_Model->Render(m_Direct3D->GetDeviceContext());
	//m_ModelTexture->Render(m_Direct3D->GetDeviceContext());
	//m_model3D->Render(m_Direct3D->GetDeviceContext());
	//if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 400, 300))	return false;

	// ���̴��� ����Ͽ� �� ������
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// �ؽ��� ���̴��� ����Ͽ� �� ������
	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;
	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture()))	return false;

	// �� ���̴��� ����Ͽ� �� ������
	//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_model3D->GetTexture(),
	//	m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;

	// �ؽ�Ʈ ���ڿ� ������
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))	return false;
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))	return false;

	// ���� ���� off
	m_Direct3D->TurnOffAlphaBlending();
	
	// 2D �������� �Ϸ�Ǿ����� Z ���� ON
	m_Direct3D->TurnZBufferOn();

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
} 