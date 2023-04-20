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
//#include "ExampleWaterShaderClass.h"
//#include "BitmapClass.h"
//#include "TextClass.h"
//#include "FrustumClass.h"
//#include "ModelListClass.h"
//#include "BumpMapShaderClass.h"
//#include "RenderTextureClass.h"
//#include "DebugWindowClass.h"
//#include "FogShaderClass.h"
//#include "TransparentShaderClass.h"
//#include "ReflectionShaderClass.h"
//#include "FadeShaderClass.h"
//#include "RefractionShaderClass.h"

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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
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
	if (!m_Model3D->Initialize(m_Direct3D->GetDevice(), L"./data/plane01.txt", L"./Textures/stone01.dds")) {
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//m_BumpMapShader = new BumpMapShaderClass;
	//if (!m_BumpMapShader) { return false; }
	//if (!m_BumpMapShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the bump map shader object", L"Error", MB_OK);
	//	return false;
	//}

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

	//m_Light = new LightClass;
	//if (!m_Light) { return false; }
	//m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(0.0f, -1.0f, 0.5f);
	//m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetSpecularPower(16.0f);

	m_Light1 = new LightClass;
	if (!m_Light1) { return false; }
	m_Light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light1->SetPosition(-3.0f, 1.0f, 3.0f);
	m_Light2 = new LightClass;
	if (!m_Light2) { return false; }
	m_Light2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light2->SetPosition(3.0f, 1.0f, 3.0f);
	m_Light3 = new LightClass;
	if (!m_Light3) { return false; }
	m_Light3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Light3->SetPosition(-3.0f, 1.0f, -3.0f);
	m_Light4 = new LightClass;
	if (!m_Light4) { return false; }
	m_Light4->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_Light4->SetPosition(3.0f, 1.0f, -3.0f);
	m_Light5 = new LightClass;
	if (!m_Light5) { return false; }
	m_Light5->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light5->SetPosition(0.0f, 1.0f, 0.0f);

	// 2D �� ��ü ����
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap) { return false; }
	//// 2D �� ��ü �ʱ�ȭ
	//if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight)) {
	//	MessageBox(hwnd, L"Could not initialize the bitmap object", L"Error", MB_OK);
	//	return false;
	//}

	//// �ؽ�Ʈ ��ü ����
	//m_Text = new TextClass;
	//if (!m_Text) { return false; }
	//// �ؽ�Ʈ ��ü �ʱ�ȭ
	//if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix)) {
	//	MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
	//	return false;
	//}

	//// ���� �� ����Ʈ ��ü ����
	//m_ModelList = new ModelListClass;
	//if (!m_ModelList) { return false; }
	//// ���� �� ����Ʈ ��ü �ʱ�ȭ
	//if (!m_ModelList->Initialize(25)) {
	//	MessageBox(hwnd, L"Could not initialize the model list object", L"Error", MB_OK);
	//	return false;
	//}

	//// ����ü ��ü ����
	//m_Frustum = new FrustumClass;
	//if (!m_Frustum) { return false; }

	//m_RenderTexture = new RenderTextureClass;
	//if (!m_RenderTexture) { return false; }
	//if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
	//	return false;
	//}

	//m_DebugWindow = new DebugWindowClass;
	//if (!m_DebugWindow) { return false; }
	//if (!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100)) {
	//	MessageBox(hwnd, L"Could not initialize the debug window object", L"Error", MB_OK);
	//	return false;
	//}

	//m_FogShader = new FogShaderClass;
	//if (!m_FogShader) { return false; }
	//if (!m_FogShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the fog shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ReflectionShader = new ReflectionShaderClass;
	//if (!m_ReflectionShader) { return false; }
	//if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the transparent shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_fadeInTime = 3000.0f;	// ���̵� �� ȿ�� �ð� ����
	//m_accumulatedTime = 0;	// ���� �ð�
	//m_fadePercentage = 0;	// ���̵� �ۼ�Ʈ�� �����Ͽ� ���̵� ȿ�� �������� ����
	//m_fadeDone = false;		// ���̵� ȿ�� �Ϸ� ����
	//m_FadeShader = new FadeShaderClass;
	//if (!m_FadeShader) { return false; }
	//if (!m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the fade shader object", L"Error", MB_OK);
	//	return false;
	//}

	/*
	// �� ȿ���� ���� Ŭ����
	m_GroundModel = new Model3DClass;
	if (!m_GroundModel->Initialize(m_Direct3D->GetDevice(), L"./data/ground.txt", L"./Textures/ground01.dds")) {
		MessageBox(hwnd, L"Could not initialize the ground model object", L"Error", MB_OK);
		return false;
	}
	m_WallModel = new Model3DClass;
	if (!m_WallModel->Initialize(m_Direct3D->GetDevice(), L"./data/wall.txt", L"./Textures/wall01.dds")) {
		MessageBox(hwnd, L"Could not initialize the wall model object", L"Error", MB_OK);
		return false;
	}
	m_BathModel = new Model3DClass;
	if (!m_BathModel->Initialize(m_Direct3D->GetDevice(), L"./data/bath.txt", L"./Textures/marble01.dds")) {
		MessageBox(hwnd, L"Could not initialize the bath model object", L"Error", MB_OK);
		return false;
	}
	m_WaterModel = new Model3DClass;
	if (!m_WaterModel->Initialize(m_Direct3D->GetDevice(), L"./data/water.txt", L"./Textures/water01.dds")) {
		MessageBox(hwnd, L"Could not initialize the water model object", L"Error", MB_OK);
		return false;
	}
	m_ExampleWaterShader = new ExampleWaterShaderClass;
	if (!m_ExampleWaterShader) { return false; }
	if (!m_ExampleWaterShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
		return false;
	}
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture) { return false; }
	if (!m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the reflection texture object", L"Error", MB_OK);
		return false;
	}
	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture) { return false; }
	if (!m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the refraction texture object", L"Error", MB_OK);
		return false;
	}
	m_WaterShader = new ReflectionShaderClass;
	if (!m_WaterShader) { return false; }
	if (!m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the water shader object", L"Error", MB_OK);
		return false;
	}
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader) { return false; }
	if (!m_RefractionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object", L"Error", MB_OK);
		return false;
	}
	m_waterHeight = 2.75f;
	m_waterTranslation = 0.0f;
	*/

	return true;
}

void GraphicsClass::Shutdown()
{
	/*
	if (m_RefractionShader) {
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}
	if (m_WaterShader) {
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}
	if (m_RefractionTexture) {
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}
	if (m_ExampleWaterShader) {
		m_ExampleWaterShader->Shutdown();
		delete m_ExampleWaterShader;
		m_ExampleWaterShader = 0;
	}
	if (m_WaterModel) {
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}
	if (m_BathModel) {
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}
	if (m_WallModel) {
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}
	if (m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}
	*/

	//if (m_FadeShader) {
	//	m_FadeShader->Shutdown();
	//	delete m_FadeShader;
	//	m_FadeShader = 0;
	//}

	//if (m_ReflectionShader) {
	//	m_ReflectionShader->Shutdown();
	//	delete m_ReflectionShader;
	//	m_ReflectionShader = 0;
	//}

	//if (m_FogShader)
	//{
	//	delete m_FogShader;
	//	m_FogShader = 0;
	//}

	//if (m_DebugWindow)
	//{
	//	delete m_DebugWindow;
	//	m_DebugWindow = 0;
	//}

	//if (m_RenderTexture)
	//{
	//	delete m_RenderTexture;
	//	m_RenderTexture = 0;
	//}

	//// ����ü ��ü ��ȯ
	//if (m_Frustum) {
	//	delete m_Frustum;
	//	m_Frustum = 0;
	//}

	//// ���� �� ����Ʈ ��ü ��ȯ
	//if (m_ModelList) {
	//	m_ModelList->Shutdown();
	//	delete m_ModelList;
	//	m_ModelList = 0;
	//}

	//// �ؽ�Ʈ ��ü ��ȯ
	//if (m_Text) {
	//	m_Text->Shutdown();
	//	delete m_Text;
	//	m_Text = 0;
	//}

	// ��Ʈ�� ��ü ��ȯ
	//if (m_Bitmap) {
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = 0;
	//}

	// light ��ü ��ȯ
	if (m_Light5)
	{
		delete m_Light5;
		m_Light5 = 0;
	}
	if (m_Light4)
	{
		delete m_Light4;
		m_Light4 = 0;
	}
	if (m_Light3)
	{
		delete m_Light3;
		m_Light3 = 0;
	}
	if (m_Light2)
	{
		delete m_Light2;
		m_Light2 = 0;
	}
	if (m_Light1)
	{
		delete m_Light1;
		m_Light1 = 0;
	}
	//if (m_Light)
	//{
	//	delete m_Light;
	//	m_Light = 0;
	//}

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

	//// ���� �� ���̴� ��ü ��ȯ
	//if (m_BumpMapShader) {
	//	m_BumpMapShader->Shutdown();
	//	delete m_BumpMapShader;
	//	m_BumpMapShader = 0;
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
bool GraphicsClass::Frame()
{
	// Input �� �ڵ�
	//if (!m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetKeyboardInput(keyCount, m_Direct3D->GetDeviceContext()))	return false;

	// fps, cpu, timer �� �ڵ�
	//if(!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))	return false;

	//m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	//m_waterTranslation += 0.001f;
	//if (m_waterTranslation > 1.0f)	m_waterTranslation -= 1.0f;
	//m_Camera->SetPosition(-11.0f, 6.0f, -10.0f);
	//m_Camera->SetRotation(0.0f, 45.0f, 0.0f);

	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);

	return true;
}
bool GraphicsClass::Frame(float frameTime)
{
	//if (!m_fadeDone) {
	//	// ���� �ð��� ������ �ð� �߰�
	//	m_accumulatedTime += frameTime;

	//	// ���̵��� ���̶��
	//	if (m_accumulatedTime < m_fadeInTime) {
	//		// ���̵� �ۼ�Ʈ�� ������� ��� ������Ʈ
	//		m_fadePercentage = m_accumulatedTime / m_fadeInTime;
	//	}
	//	else {
	//		// �������� �Ϸ� ����
	//		m_fadeDone = true;
	//		m_fadePercentage = 1.0f;
	//	}
	//}

	return true;
}

bool GraphicsClass::Render()
{
	//if (m_fadeDone) {
	//	// �� ������ ��� ������
	//	if (!RenderScene())	return false;
	//}
	//else {
	//	// ��ü ����� ���� �ؽ�ó�� ������
	//	if (!RenderToTexture()) return false;
	//	if (!RenderFadingScene()) return false;
	//}

	//if (!RenderRefractionToTexture()) return false;
	//if (!RenderReflectionToTexture()) return false;
	//if (!RenderWaterScene()) return false;

	// �� ������ ��� ������
	if (!RenderScene())	return false;

	return true;
}

/*
bool GraphicsClass::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ������ ����� RTT�� ����
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// RTT �ʱ�ȭ
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// �� ȸ���� �ڵ�
	static float rotation = 0.0f;

	// �� �������� ȸ���� ������Ʈ
	rotation += (float)XM_PI * 0.0025f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// ���� ȸ���� �� �ֵ��� ȸ�� ������ ���� ��� ����
	worldMatrix = XMMatrixRotationY(rotation);

	// �ؽ�ó �̵� ��ġ ����
	static float textureTranslation = 0.0f;
	textureTranslation += 0.005f;
	if (textureTranslation > 1.0f) textureTranslation -= 1.0f;

	// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
	m_Model3D->Render(m_Direct3D->GetDeviceContext());

	// �ؽ��� ���̴��� �ݻ� �� ����� ����Ͽ� �� ������
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), textureTranslation);

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
*/

bool GraphicsClass::RenderScene()
{
	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	XMFLOAT4 diffuseColor[5];
	XMFLOAT4 lightPosition[5];

	diffuseColor[0] = m_Light1->GetDiffuseColor();
	diffuseColor[1] = m_Light2->GetDiffuseColor();
	diffuseColor[2] = m_Light3->GetDiffuseColor();
	diffuseColor[3] = m_Light4->GetDiffuseColor();
	diffuseColor[4] = m_Light5->GetDiffuseColor();
	lightPosition[0] = m_Light1->GetPosition();
	lightPosition[1] = m_Light2->GetPosition();
	lightPosition[2] = m_Light3->GetPosition();
	lightPosition[3] = m_Light4->GetPosition();
	lightPosition[4] = m_Light5->GetPosition();

	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// �ؽ��� ���̴��� �̿��Ͽ� �� ������
	m_Model3D->Render(m_Direct3D->GetDeviceContext());
	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), diffuseColor, lightPosition))	return false;

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
}

/*
bool GraphicsClass::RenderFadingScene()
{
	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� ���� Z ���� OFF
	m_Direct3D->TurnZBufferOff();

	// �ؽ��� ���̴��� �̿��Ͽ� �� ������
	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 0))	return false;
	if (!m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_fadePercentage))	return false;

	// 2D �������� �Ϸ�Ǿ����� Z ���� ON
	m_Direct3D->TurnZBufferOn();

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
}
*/

/*
bool GraphicsClass::RenderRefractionToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	if(!m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane))	return false;

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
bool GraphicsClass::RenderReflectionToTexture()
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->RenderReflection(m_waterHeight);
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);
	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
bool GraphicsClass::RenderWaterScene()
{
	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);
	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	reflectionMatrix = m_Camera->GetReflectionViewMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);
	m_WaterModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, 
		m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(),m_waterTranslation, 0.1f))	return false;

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
}
*/

/* ---------------------------------------- �ڵ� ����--------------------------------------------

// �������� �� ����
//float positionX = 0;
//float positionY = 0;
//float positionZ = 0;
//float radius = 1.0f;
//XMFLOAT4 color;
//float fogColor = 0.5f;
//float fogStart = 0.0f;
//float fogEnd = 5.0f;
//float blendAmount = 0.5f;

// Ŭ���� ��� ����
// �� ���� Y�� ���� 0 �Ʒ��� �͵��� �׸��� �ʰ� �Ѵ�
//XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);

// �ؽ�ó �̵� ��ġ ����
//static float textureTranslation = 0.0f;
//textureTranslation += 0.005f;
//if (textureTranslation > 1.0f) textureTranslation -= 1.0f;

// Scene�� �׸��� ���� ���� ����
m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
//m_Direct3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

// �� ���ۿ� ������
//if (!RenderScene())	return false;

// ī�޶��� ��ġ�� ���� �� ��� ����
m_Camera->Render();

// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
m_Direct3D->GetWorldMatrix(worldMatrix);
m_Camera->GetViewMatrix(viewMatrix);
m_Direct3D->GetProjectionMatrix(projectionMatrix);
m_Direct3D->GetOrthoMatrix(orthoMatrix);

// ī�޶� �ݻ������ ���
m_Camera->RenderReflection(-1.5f);

// �Ϲ� �� ��� ��� �ݻ� �� ����� �����´�
reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
m_Direct3D->GetWorldMatrix(worldMatrix);
m_Direct3D->GetProjectionMatrix(projectionMatrix);

// �� ȸ���� �ڵ�
static float rotation = 0.0f;

// �� �������� ȸ���� ������Ʈ
rotation += (float)XM_PI * 0.0025f;
if (rotation > 360.0f)	rotation -= 360.0f;

// ���� ȸ���� �� �ֵ��� ȸ�� ������ ���� ��� ����
worldMatrix = XMMatrixRotationY(rotation);


// �� �����Ӹ��� �þ� ��Ŀ� �ٰ��Ͽ� ����ü�� ����
//m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

// ������ �� ���� �� ���� �� �ʱ�ȭ
//int modelCount = m_ModelList->GetModelCount();
//int renderCount = 0;

// ��� ���� Ž���ϰ�, ī�޶� �信 �������� �𵨸� ������
//for (int i = 0; i < modelCount; i++) {
//	// �� ��ġ�� ���� ����
//	m_ModelList->GetData(i, positionX, positionY, positionZ, color);
//	
//	// ���� ����ü �ȿ� �ִ��� Ȯ��
//	if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius)) {
//		// ���� ������ �� ��ġ�� �̵�
//		worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
//		
//		// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
//		m_Model3D->Render(m_Direct3D->GetDeviceContext());

//		// �� ���̴��� ����Ͽ� �� ������
//		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(),
//			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

//		// ���� ��� ����
//		m_Direct3D->GetWorldMatrix(worldMatrix);

//		renderCount++;
//	}
//}

// 2D �������� ���� Z ���� OFF
//m_Direct3D->TurnZBufferOff();

// ���� ���� on
//m_Direct3D->TurnOnAlphaBlending();

// ���� ������ �ε��� ���۸� �׷��� ���������ο� ���� �������� �غ�
//m_Model->Render(m_Direct3D->GetDeviceContext());
//m_ModelTexture->Render(m_Direct3D->GetDeviceContext());
//m_Model3D->Render(m_Direct3D->GetDeviceContext());
//if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 400, 300))	return false;
//if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))	return false;

// ���̴��� ����Ͽ� �� ������
//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

// �ؽ��� ���̴��� ����Ͽ� �� ������
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTextureArray()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView()))	return false;
//if (!m_FogShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), fogStart, fogEnd))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), textureTranslation))	return false;

// �� ���̴��� ����Ͽ� �� ������
//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_model3D->GetTexture(),
//	m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;

// �ؽ�Ʈ ���ڿ� ������
//if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))	return false;
//if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))	return false;

// ���� ������ ������Ʈ�� �߰��ϱ� ����
// x������ 1��ŭ, z������ -1��ŭ �̵�
// �� �� ���ĺ����� on �ϰ� 2��° ���� ������
//worldMatrix = XMMatrixTranslation(0.5f, 0.5f, -0.1f);
//m_Direct3D->TurnOnAlphaBlending();
//m_Model3D2->Render(m_Direct3D->GetDeviceContext());
//if (!m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D2->GetTexture(), blendAmount))	return false;

// ���� ���� off
//m_Direct3D->TurnOffAlphaBlending();

// 2D �������� �Ϸ�Ǿ����� Z ���� ON
//m_Direct3D->TurnZBufferOn();

// ���� �� ���̴��� ����Ͽ� �� ������
//if (!m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTextureArray(),
//	m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;

// ������ ������ ȭ�鿡 ���
//m_Direct3D->EndScene();


*/