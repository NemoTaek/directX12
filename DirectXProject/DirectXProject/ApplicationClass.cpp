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
//#include "FrustumClass.h"
//#include "QuadTreeClass.h"
//#include "TextureShaderClass.h"
//#include "MiniMap.h"
#include "SkyDomeClass.h"
#include "SkyDomeShaderClass.h"
#include "SkyPlaneClass.h"
#include "SkyPlaneShaderClass.h"

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
	// ī�޶� ��ġ ����
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(XMFLOAT3(50.0f, 2.0f, -7.0f));
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

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
	m_Position->SetPosition(XMFLOAT3(50.0f, 2.0f, -7.0f));

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
	if (!m_Terrain->Initialize(m_Direct3D->GetDevice(), "./Textures/heightmap01.bmp", L"./Textures/dirt01.dds", "./Textures/colorm01.bmp")) {
		MessageBox(hwnd, L"Could not initialize the terrain object", L"Error", MB_OK);
		return false;
	}
	int terrainWidth = 0;
	int terrainHeight = 0;
	m_Terrain->GetTerrainSize(terrainWidth, terrainHeight);

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
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, 0.0f);

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
	if (!m_SkyPlane->Initialize(m_Direct3D->GetDevice(), L"./Textures/cloud001.dds", L"./Textures/cloud002.dds")) {
		MessageBox(hwnd, L"Could not initialize the sky plane object", L"Error", MB_OK);
		return false;
	}

	m_SkyPlaneShader = new SkyPlaneShaderClass;
	if (!m_SkyPlaneShader) { return false; }
	if (!m_SkyPlaneShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the sky plane shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
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
	m_Position->MoveForward(m_Input->IsUpArrowPressed());
	m_Position->MoveBackward(m_Input->IsDownArrowPressed());
	m_Position->MoveUpward(m_Input->IsAPressed());
	m_Position->MoveDownward(m_Input->IsZPressed());
	m_Position->LookUpward(m_Input->IsPageUpPressed());
	m_Position->LookDownward(m_Input->IsPageDownPressed());

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
	if (!m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_SkyPlane->GetCloudTexture1(), m_SkyPlane->GetCloudTexture2(), 
		m_SkyPlane->GetTranslation(0), m_SkyPlane->GetTranslation(1), m_SkyPlane->GetTranslation(2), m_SkyPlane->GetTranslation(3), m_SkyPlane->GetBrightness()))	return false;

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
	if (!m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture()))	return false;
	//if (!m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture()))	return false;

	// ���� Ʈ�� �� ���� ���̴��� ����Ͽ� ���� ������
	//m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

	// �þ߿� ������ �� �ﰢ�� �� ���
	//if (!m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext()))	return false;

	// 2D �������� ���� Z ���� OFF
	m_Direct3D->TurnZBufferOff();

	// �̴ϸ� ������
	//if (!m_MiniMap->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix, m_TextureShader))	return false;

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