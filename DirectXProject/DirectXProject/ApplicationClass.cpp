#include "Stdafx.h"
#include "ApplicationClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "InputClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TextClass.h"
#include "FontShaderClass.h"
//#include "ColorShaderClass.h"
#include "TerrainClass.h"
#include "TerrainShaderClass.h"
#include "LightClass.h"
#include "TextureClass.h"
//#include "FrustumClass.h"
//#include "QuadTreeClass.h"
//#include "TextureShaderClass.h"
//#include "MiniMap.h"
#include "SkyDomeClass.h"
#include "SkyDomeShaderClass.h"
#include "SkyPlaneClass.h"
#include "SkyPlaneShaderClass.h"
//#include "DebugWindowClass.h"
//#include "RenderTextureClass.h"
//#include "DepthShaderClass.h"
//#include "ReflectionShaderClass.h"
//#include "WaterClass.h"
//#include "WaterShaderClass.h"

ApplicationClass::ApplicationClass() {}
ApplicationClass::ApplicationClass(const ApplicationClass& other) {}
ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_Direct3D = new D3DClass;
	if (!m_Direct3D) return false;
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera) { return false; }

	// 2D UI �������� ���� ī�޶�� �⺻ �� ��� �ʱ�ȭ
	XMMATRIX baseViewMatrix;
	XMFLOAT3 camera = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_Camera->SetPosition(camera);
	m_Camera->RenderBaseViewMatrix();
	m_Camera->GenerateBaseViewMatrix();
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// ī�޶� ��ġ ����
	//camera = XMFLOAT3(150.0f, 2.0f, 35.0f);
	//m_Camera->SetPosition(camera);

	m_Input = new InputClass;
	if (!m_Input) return false;
	if (!m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the input object", L"Error", MB_OK);
		return false;
	}

	m_Timer = new TimerClass;
	if (!m_Timer) return false;
	if (!m_Timer->Initialize()) {
		MessageBox(hwnd, L"Could not initialize the timer object", L"Error", MB_OK);
		return false;
	}

	m_Position = new PositionClass;
	if (!m_Position) return false;
	m_Position->SetPosition(XMFLOAT3(14.0f, 13.0f, 10.0f));
	m_Position->SetRotation(XMFLOAT3(25.0f, 0.0f, 0.0f));

	m_Fps = new FpsClass;
	if (!m_Fps) return false;
	m_Fps->Initialize();

	m_Cpu = new CpuClass;
	if (!m_Cpu) return false;
	m_Cpu->Initialize();

	m_Text = new TextClass;
	if (!m_Text) return false;
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix)) {
		MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
		return false;
	}

	m_FontShader = new FontShaderClass;
	if (!m_FontShader) return false;
	if (!m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
		return false;
	}

	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader) return false;
	//if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
	//	return false;
	//}

	m_Terrain = new TerrainClass;
	if (!m_Terrain) return false;
	if (!m_Terrain->Initialize(m_Direct3D->GetDevice(), "./Textures/hm.bmp", 10.0f)) {
		MessageBox(hwnd, L"Could not initialize the terrain object", L"Error", MB_OK);
		return false;
	}
	//int terrainWidth = 0;
	//int terrainHeight = 0;
	//m_Terrain->GetTerrainSize(terrainWidth, terrainHeight);

	// ���� ī�� ����
	char cardName[128] = { 0, };
	int memory = 0;
	m_Direct3D->GetVideoCardInfo(cardName, memory);
	if (!m_Text->SetVideoCardInfo(cardName, memory, m_Direct3D->GetDeviceContext())) {
		MessageBox(hwnd, L"Could not set video card info in the text object", L"Error", MB_OK);
		return false;
	}

	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader) return false;
	if (!m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the terrain shader object", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass;
	if (!m_Light) return false;
	//m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.5f, -0.75f, 0.0f);

	//m_Frustum = new FrustumClass;
	//if (!m_Frustum) return false;

	//m_QuadTree = new QuadTreeClass;
	//if (!m_QuadTree) return false;
	//if (!m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice())) {
	//	MessageBox(hwnd, L"Could not initialize the quad tree object", L"Error", MB_OK);
	//	return false;
	//}

	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader) { return false; }
	//if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_MiniMap = new MiniMapClass;
	//if (!m_MiniMap) { return false; }
	//if (!m_MiniMap->Initialize(m_Direct3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix, static_cast<float>(terrainWidth-1), static_cast<float>(terrainHeight - 1))) {
	//	MessageBox(hwnd, L"Could not initialize the mini-map object", L"Error", MB_OK);
	//	return false;
	//}

	m_SkyDome = new SkyDomeClass;
	if (!m_SkyDome) { return false; }
	if (!m_SkyDome->Initialize(m_Direct3D->GetDevice())) {
		MessageBox(hwnd, L"Could not initialize the sky dome object", L"Error", MB_OK);
		return false;
	}

	m_SkyDomeShader = new SkyDomeShaderClass;
	if (!m_SkyDomeShader) { return false; }
	if (!m_SkyDomeShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the sky dome shader object", L"Error", MB_OK);
		return false;
	}

	m_SkyPlane = new SkyPlaneClass;
	if (!m_SkyPlane) { return false; }
	if (!m_SkyPlane->Initialize(m_Direct3D->GetDevice(), L"./Textures/cloud001.dds", L"./Textures/perturb001.dds")) {
		MessageBox(hwnd, L"Could not initialize the sky plane object", L"Error", MB_OK);
		return false;
	}

	m_SkyPlaneShader = new SkyPlaneShaderClass;
	if (!m_SkyPlaneShader) { return false; }
	if (!m_SkyPlaneShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the sky plane shader object", L"Error", MB_OK);
		return false;
	}

	//m_DebugWindow = new DebugWindowClass;
	//if (!m_DebugWindow) { return false; }
	//if (!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 256, 256)) {
	//	MessageBox(hwnd, L"Could not initialize the debug window bitmap object", L"Error", MB_OK);
	//	return false;
	//}

	//m_RenderTexture = new RenderTextureClass;
	//if (!m_RenderTexture) { return false; }
	//if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
	//	MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
	//	return false;
	//}

	//m_DepthShader = new DepthShaderClass;
	//if (!m_DepthShader) { return false; }
	//if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the depth shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_RefractionTexture = new RenderTextureClass;
	//if (!m_RefractionTexture) { return false; }
	//if (!m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
	//	MessageBox(hwnd, L"Could not initialize the refraction render to texture object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ReflectionTexture = new RenderTextureClass;
	//if (!m_ReflectionTexture) { return false; }
	//if (!m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
	//	MessageBox(hwnd, L"Could not initialize the reflaction render to texture object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ReflectionShader = new ReflectionShaderClass;
	//if (!m_ReflectionShader) { return false; }
	//if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the reflection shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_Water = new WaterClass;
	//if (!m_Water) { return false; }
	//if (!m_Water->Initialize(m_Direct3D->GetDevice(), L"./Textures/waternormal.dds", 3.75f, 110.0f)) {
	//	MessageBox(hwnd, L"Could not initialize the water object", L"Error", MB_OK);
	//	return false;
	//}

	//m_WaterShader = new WaterShaderClass;
	//if (!m_WaterShader) { return false; }
	//if (!m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the water shader object", L"Error", MB_OK);
	//	return false;
	//}

	m_ColorTexture1 = new TextureClass;
	if (!m_ColorTexture1) { return false; }
	if (!m_ColorTexture1->Initialize(m_Direct3D->GetDevice(), L"./Textures/dirt001.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_ColorTexture2 = new TextureClass;
	if (!m_ColorTexture2) { return false; }
	if (!m_ColorTexture2->Initialize(m_Direct3D->GetDevice(), L"./Textures/dirt004.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_ColorTexture3 = new TextureClass;
	if (!m_ColorTexture3) { return false; }
	if (!m_ColorTexture3->Initialize(m_Direct3D->GetDevice(), L"./Textures/dirt002.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_ColorTexture4 = new TextureClass;
	if (!m_ColorTexture4) { return false; }
	if (!m_ColorTexture4->Initialize(m_Direct3D->GetDevice(), L"./Textures/stone001.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_AlphaTexture = new TextureClass;
	if (!m_AlphaTexture) { return false; }
	if (!m_AlphaTexture->Initialize(m_Direct3D->GetDevice(), L"./Textures/alpha001.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_NormalTexture1 = new TextureClass;
	if (!m_NormalTexture1) { return false; }
	if (!m_NormalTexture1->Initialize(m_Direct3D->GetDevice(), L"./Textures/normal001.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	m_NormalTexture2 = new TextureClass;
	if (!m_NormalTexture2) { return false; }
	if (!m_NormalTexture2->Initialize(m_Direct3D->GetDevice(), L"./Textures/normal002.dds")) {
		MessageBox(hwnd, L"Could not initialize the render to texture object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_NormalTexture2) {
		m_NormalTexture2->Shutdown();
		delete m_NormalTexture2;
		m_NormalTexture2 = 0;
	}

	if (m_NormalTexture1) {
		m_NormalTexture1->Shutdown();
		delete m_NormalTexture1;
		m_NormalTexture1 = 0;
	}

	if (m_AlphaTexture) {
		m_AlphaTexture->Shutdown();
		delete m_AlphaTexture;
		m_AlphaTexture = 0;
	}

	if (m_ColorTexture4) {
		m_ColorTexture4->Shutdown();
		delete m_ColorTexture4;
		m_ColorTexture4 = 0;
	}

	if (m_ColorTexture3) {
		m_ColorTexture3->Shutdown();
		delete m_ColorTexture3;
		m_ColorTexture3 = 0;
	}

	if (m_ColorTexture2) {
		m_ColorTexture2->Shutdown();
		delete m_ColorTexture2;
		m_ColorTexture2 = 0;
	}

	if (m_ColorTexture1) {
		m_ColorTexture1->Shutdown();
		delete m_ColorTexture1;
		m_ColorTexture1 = 0;
	}

	//if (m_WaterShader) {
	//	m_WaterShader->Shutdown();
	//	delete m_WaterShader;
	//	m_WaterShader = 0;
	//}

	//if (m_Water) {
	//	m_Water->Shutdown();
	//	delete m_Water;
	//	m_Water = 0;
	//}

	//if (m_ReflectionShader) {
	//	m_ReflectionShader->Shutdown();
	//	delete m_ReflectionShader;
	//	m_ReflectionShader = 0;
	//}

	//if (m_ReflectionTexture) {
	//	m_ReflectionTexture->Shutdown();
	//	delete m_ReflectionTexture;
	//	m_ReflectionTexture = 0;
	//}

	//if (m_RefractionTexture) {
	//	m_RefractionTexture->Shutdown();
	//	delete m_RefractionTexture;
	//	m_RefractionTexture = 0;
	//}

	//if (m_DepthShader) {
	//	m_DepthShader->Shutdown();
	//	delete m_DepthShader;
	//	m_DepthShader = 0;
	//}

	//if (m_RenderTexture) {
	//	m_RenderTexture->Shutdown();
	//	delete m_RenderTexture;
	//	m_RenderTexture = 0;
	//}

	//if (m_DebugWindow) {
	//	m_DebugWindow->Shutdown();
	//	delete m_DebugWindow;
	//	m_DebugWindow = 0;
	//}

	if (m_SkyPlaneShader) {
		m_SkyPlaneShader->Shutdown();
		delete m_SkyPlaneShader;
		m_SkyPlaneShader = 0;
	}

	if (m_SkyPlane) {
		m_SkyPlane->Shutdown();
		delete m_SkyPlane;
		m_SkyPlane = 0;
	}

	if (m_SkyDomeShader) {
		m_SkyDomeShader->Shutdown();
		delete m_SkyDomeShader;
		m_SkyDomeShader = 0;
	}

	if (m_SkyDome) {
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = 0;
	}

	//if (m_MiniMap) {
	//	m_MiniMap->Shutdown();
	//	delete m_MiniMap;
	//	m_MiniMap = 0;
	//}

	//if (m_TextureShader) {
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}

	//if (m_QuadTree) {
	//	m_QuadTree->Shutdown();
	//	delete m_QuadTree;
	//	m_QuadTree = 0;
	//}

	//if (m_Frustum) {
	//	delete m_Frustum;
	//	m_Frustum = 0;
	//}

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	if (m_TerrainShader) {
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	if (m_Terrain) {
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	//if (m_ColorShader) {
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = 0;
	//}

	if (m_FontShader) {
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_Cpu) {
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}

	if (m_Position) {
		delete m_Position;
		m_Position = 0;
	}

	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame()
{
	// ����� �Է�
	if (!m_Input->Frame())	return false;

	// ESCŰ �Է� Ȯ�� �� �����ٸ� ���� ó��
	if (m_Input->IsEscapePressed())	return false;

	// �ý��� ��� ������Ʈ
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	if (!m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext()))	return false;
	if (!m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext()))	return false;

	// ������ �Է� ó�� ����
	if (!HandleInput(m_Timer->GetTime()))	return false;

	/*
	// ī�޶� ���� ��ġ ������
	XMFLOAT3 position = m_Camera->GetPosition();

	// ī�޶� ��ġ �ٷ� �Ʒ��� �ִ� �ﰢ���� ���� ������
	// ��! 1��Ī �������� ĳ���Ͱ� �ȴ� ���� ����!
	float height = 0.0f;
	if (m_QuadTree->GetHeightAtPosition(position.x, position.z, height)) {
		// ī�޶� �Ʒ��� �ﰢ���� �ִ� ��� ī�޶� �ΰ��� ������ ��ġ
		m_Camera->SetPosition(XMFLOAT3(position.x, height + 2.0f, position.z));
	}
	*/

	// �ϴ� ��� ������ ó�� ����
	m_SkyPlane->Frame();

	// �� ������ ó�� ����
	//m_Water->Frame();

	// ���� �� �ݻ縦 �ؽ�ó�� ������
	//RenderRefractionToTexture();
	//RenderReflectionToTexture();

	// �׷��� ������
	if (!RenderGraphics())	return false;

	return true;
}

bool ApplicationClass::HandleInput(float frameTime)
{
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// ���ŵ� ��ġ�� ����ϱ� ���� ������ �ð� ����
	m_Position->SetFrameTime(frameTime);

	// �Է� ó��
	m_Position->TurnLeft(m_Input->IsLeftArrowPressed());
	m_Position->TurnRight(m_Input->IsRightArrowPressed());
	m_Position->LookUpward(m_Input->IsUpArrowPressed());
	m_Position->LookDownward(m_Input->IsDownArrowPressed());
	m_Position->MoveForward(m_Input->IsWPressed());
	m_Position->MoveLeft(m_Input->IsAPressed());
	m_Position->MoveBackward(m_Input->IsSPressed());
	m_Position->MoveRight(m_Input->IsDPressed());
	m_Position->MoveUpward(m_Input->IsPgUpPressed());
	m_Position->MoveDownward(m_Input->IsPgDnPressed());

	// ������ ��ġ�� ȸ���� ������ �� ī�޶� ��ġ ����
	m_Position->GetPosition(position);
	m_Position->GetRotation(rotation);
	m_Camera->SetPosition(position);
	m_Camera->SetRotation(rotation);

	// �ؽ�Ʈ�� ��ġ �� ȸ�� �� ������Ʈ
	if (!m_Text->SetCameraPosition(position, m_Direct3D->GetDeviceContext()))	return false;
	if (!m_Text->SetCameraRotation(rotation, m_Direct3D->GetDeviceContext()))	return false;

	// �̴ϸʿ��� ī�޶� ��ġ ������Ʈ
	//m_MiniMap->PositionUpdate(position.x, position.z);

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	// ���� ����� �ؽ�ó�� ������
	//if (!RenderSceneToTexture())	return false;

	// Scene�� �׸��� ���� ���� ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix, reflectionViewMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// ������� �ϴ� ���� �ڵ�
	// ĳ���ʹ� �׻� �ϴ� �Ʒ��� �ֱ� ������ ī�޶� ��ġ�� �߽����� ����
	// ���� �ø��� ����, Z���۸� ��Ȱ��ȭ �ϰ�, �ϴ� ���� �������� �� �ǵ����� ������ ��ħ
	// ���� Z���۸� Off �ؾ� �Ÿ��� ������� ������ ����� �� ���ۿ� �ϴ� �� ��ü�� �׸� �� ����
	// �׷��� ����ۿ� �ϴõ��� �׸� �� ������ ������ �ϴ� ������ ��ħ
	XMFLOAT3 cameraPosition = m_Camera->GetPosition();
	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	m_Direct3D->TurnOffCulling();
	m_Direct3D->TurnZBufferOff();

	m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	if (!m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor()))	return false;

	m_Direct3D->TurnOnCulling();

	// �ϴ� ���(����)�� �ϴ� �� ����� ȥ�յǵ��� 2�� ������ �����ϵ��� ����
	m_Direct3D->EnableSecondBlendState();

	m_SkyPlane->Render(m_Direct3D->GetDeviceContext());
	if (!m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_SkyPlane->GetCloudTexture(), m_SkyPlane->GetPerturbTexture(), 
		m_SkyPlane->GetTranslation(), m_SkyPlane->GetScale(), m_SkyPlane->GetBrightness()))	return false;

	// 2�� ���� off
	m_Direct3D->TurnOffAlphaBlending();

	m_Direct3D->TurnZBufferOn();

	// ���� ��� �缳��
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� ���� ������
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// ���͸����� ����� ���� ���� ������
	// ���⼭�� ���̴��� �Ű������� �� �� �ȿ��� �� ó�� �����ϱ� ������ ���⼭�� ���� ���̴� ó���� ���� ����
	//m_Terrain->RenderMaterials(m_Direct3D->GetDeviceContext(), m_TerrainShader, worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());

	// ����ü ����
	//m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// �÷� ���̴��� ����Ͽ� �� ������
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// ���� ���̴��� ����Ͽ� �� ������
	//if (!m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetFlatTexture(), m_Terrain->GetSlopeTexture(), m_Terrain->GetScarpTexture()))	return false;
	//if (!m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture()))	return false;
	//if (!m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 2.0f))	return false;
	if (!m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetDirection(), m_ColorTexture1->GetTexture(), m_ColorTexture2->GetTexture(), m_ColorTexture3->GetTexture(), m_ColorTexture4->GetTexture(), m_AlphaTexture->GetTexture(), m_NormalTexture1->GetTexture(), m_NormalTexture2->GetTexture()))	return false;

	// ���� �ִ� ��ġ�� �̵� �� ������
	//worldMatrix = XMMatrixTranslation(240.0f, m_Water->GetWaterHeight(), 250.0f);
	//m_Water->Render(m_Direct3D->GetDeviceContext());
	//m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_Water->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix,
	//	m_RefractionTexture->GetShaderResourceView(), m_ReflectionTexture->GetShaderResourceView(), m_Water->GetTexture(), m_Camera->GetPosition(), m_Water->GetNormalMapTiling(),
	//	m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(), m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShiness());
	//m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� Ʈ�� �� ���� ���̴��� ����Ͽ� ���� ������
	//m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

	// �þ߿� ������ �� �ﰢ�� �� ���
	//if (!m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext()))	return false;

	// 2D �������� ���� Z ���� OFF
	m_Direct3D->TurnZBufferOff();

	// �̴ϸ� ������
	//if (!m_MiniMap->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix, m_TextureShader))	return false;

	// ����� â
	//if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 100, 100))	return false;

	// �ؽ�ó ���̴��� �ؽ�ó ���ҽ��� �������� ����Ͽ� ��Ʈ�� �� ������
	//m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());

	// ���� ���� on
	m_Direct3D->TurnOnAlphaBlending();

	// �ؽ�Ʈ UI ������
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix))	return false;

	// ���� ���� off
	m_Direct3D->TurnOffAlphaBlending();

	// 2D �������� �Ϸ�Ǿ����� Z ���� ON
	m_Direct3D->TurnZBufferOn();

	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
}

/*
bool ApplicationClass::RenderSceneToTexture()
{
	// ������ ����� RTT�� ����
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT �ʱ�ȭ
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ���� ���� ������
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// ���� ���̴��� ����Ͽ� ���� ������
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� ����
	m_Direct3D->ResetViewport();

	return true;
}
*/

/*
void ApplicationClass::RenderRefractionToTexture()
{
	// �� �� ���̿� ���� Ŭ���� ���� �����Ͽ� ������ ����� ���� ���� �ִ� ��� ���� Ŭ����
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_Water->GetWaterHeight() + 0.1f);

	// ������ ����� RTT�� ����
	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT �ʱ�ȭ
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->Render();

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ���� ���� ������
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// �ݻ� ���̴��� ����Ͽ� ���� �������Ͽ� ����ȿ�� ����
	m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 2.0f, clipPlane);

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� ����
	m_Direct3D->ResetViewport();
}

void ApplicationClass::RenderReflectionToTexture()
{
	// �� �ؿ� �ִ� ��ü�� ���̸� �������� Ŭ���θ� ����
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, 1.0f, 0.0f, -m_Water->GetWaterHeight());

	// ������ ����� RTT�� ����
	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT �ʱ�ȭ
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ��� ����
	m_Camera->RenderReflection(m_Water->GetWaterHeight());

	// ī�޶� �� Direct3D ��ü���� ����, ��, ���� ����� �����´�
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ī�޶� ��ġ�� ���� ��, �ݻ�� ī�޶� ��ġ�� �����´�
	XMFLOAT3 cameraPosition = m_Camera->GetPosition();
	cameraPosition.y = -cameraPosition.y + (m_Water->GetWaterHeight() * 2.0f);

	// �ݻ�� �ϴ� ǳ�� ������
	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	m_Direct3D->TurnOffCulling();
	m_Direct3D->TurnZBufferOff();

	m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	m_Direct3D->TurnOnCulling();
	m_Direct3D->EnableSecondBlendState();

	m_SkyPlane->Render(m_Direct3D->GetDeviceContext());
	m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_SkyPlane->GetCloudTexture(), m_SkyPlane->GetPerturbTexture(),
		m_SkyPlane->GetTranslation(), m_SkyPlane->GetScale(), m_SkyPlane->GetBrightness());

	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� ���� ������
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// �ݻ� ���̴��� ����Ͽ� ���� �������Ͽ� ����ȿ�� ����
	m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 2.0f, clipPlane);

	// ������ ����� �ٽ� ���� ����۷� ����
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� ����
	m_Direct3D->ResetViewport();
}
*/