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
#include "FrustumClass.h"
#include "QuadTreeClass.h"

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
	// 카메라 위치 설정
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
	if (!m_Terrain->Initialize(m_Direct3D->GetDevice(), "./Textures/heightmap01.bmp", L"./Textures/ice.dds")) {
		MessageBox(hwnd, L"Could not initialize the terrain object", L"Error", MB_OK);
		return false;
	}

	// 비디오 카드 정보
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

	m_Frustum = new FrustumClass;
	if (!m_Frustum) return false;

	m_QuadTree = new QuadTreeClass;
	if (!m_QuadTree) return false;
	if (!m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice())) {
		MessageBox(hwnd, L"Could not initialize the quad tree object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_QuadTree) {
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

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
	// 사용자 입력
	if (!m_Input->Frame())	return false;

	// ESC키 입력 확인 후 눌렀다면 종료 처리
	if (m_Input->IsEscapePressed())	return false;

	// 시스템 통계 업데이트
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	if (!m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext()))	return false;
	if (!m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext()))	return false;

	// 프레임 입력 처리 수행
	if (!HandleInput(m_Timer->GetTime()))	return false;

	// 카메라 현재 위치 가져옴
	XMFLOAT3 position = m_Camera->GetPosition();

	// 카메라 위치 바로 아래에 있는 삼각형의 높이 가져옴
	// 와! 1인칭 시점으로 캐릭터가 걷는 느낌 난다!
	float height = 0.0f;
	if (m_QuadTree->GetHeightAtPosition(position.x, position.z, height)) {
		// 카메라 아래에 삼각형이 있는 경우 카메라를 두개의 단위로 배치
		m_Camera->SetPosition(XMFLOAT3(position.x, height + 2.0f, position.z));
	}

	// 그래픽 렌더링
	if (!RenderGraphics())	return false;

	return true;
}

bool ApplicationClass::HandleInput(float frameTime)
{
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 갱신된 위치를 계산하기 위한 프레임 시간 설정
	m_Position->SetFrameTime(frameTime);

	// 입력 처리
	m_Position->TurnLeft(m_Input->IsLeftArrowPressed());
	m_Position->TurnRight(m_Input->IsRightArrowPressed());
	m_Position->MoveForward(m_Input->IsUpArrowPressed());
	m_Position->MoveBackward(m_Input->IsDownArrowPressed());
	m_Position->MoveUpward(m_Input->IsAPressed());
	m_Position->MoveDownward(m_Input->IsZPressed());
	m_Position->LookUpward(m_Input->IsPageUpPressed());
	m_Position->LookDownward(m_Input->IsPageDownPressed());

	// 시점의 위치와 회전을 가져온 후 카메라 위치 설정
	m_Position->GetPosition(position);
	m_Position->GetRotation(rotation);
	m_Camera->SetPosition(position);
	m_Camera->SetRotation(rotation);

	// 텍스트의 위치 및 회전 값 업데이트
	if (!m_Text->SetCameraPosition(position, m_Direct3D->GetDeviceContext()))	return false;
	if (!m_Text->SetCameraRotation(rotation, m_Direct3D->GetDeviceContext()))	return false;

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 지형 버퍼 렌더링
	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// 절두체 생성
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 컬러 셰이더를 사용하여 모델 렌더링
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// 지형 셰이더를 사용하여 모델 렌더링
	//if (!m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture()))	return false;
	if (!m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture()))	return false;

	// 쿼드 트리 및 지형 셰이더를 사용하여 지형 렌더링
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

	// 시야에 렌더링 된 삼각형 수 출력
	if (!m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext()))	return false;

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 알파 블랜딩 on
	m_Direct3D->TurnOnAlphaBlending();

	// 텍스트 UI 렌더링
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix))	return false;

	// 알파 블랜딩 off
	m_Direct3D->TurnOffAlphaBlending();

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}