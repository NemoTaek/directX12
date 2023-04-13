#include "Stdafx.h"
#include "InputClass.h"
#include "GraphicsClass.h"
//#include "SoundClass.h"
//#include "FpsClass.h"
//#include "CpuClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "SystemClass.h"

SystemClass::SystemClass() {}
SystemClass::SystemClass(const SystemClass& other) {}
SystemClass::~SystemClass() {}

bool SystemClass::initialize()
{
	// 윈도우 창 가로 세로
	int screenWidth = 0;
	int screenHeight = 0;

	// 윈도우 생성 초기화
	InitializeWindows(screenWidth, screenHeight);

	// m_Input 객체 생성
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}

	// m_Input 객체 초기화
	if (!(m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight)))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object", L"Error", MB_OK);
		return false;
	}

	// m_Graphics 객체 생성
	// 그래픽 랜더링 처리 하기 위한 객체
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}

	// m_Graphics 객체 초기화
	if (!(m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd)))
	{
		MessageBox(m_hwnd, L"Could not initialize the graphic object", L"Error", MB_OK);
		return false;
	}

	/*
	// m_Sound 객체 생성
	m_Sound = new SoundClass;
	if (!m_Sound) {
		return false;
	}

	// m_Sound 객체 초기화
	if (!(m_Sound->Initialize(m_hwnd)))
	{
		MessageBox(m_hwnd, L"Could not initialize the Direct Sound", L"Error", MB_OK);
		return false;
	}

	// m_Fps 객체 생성
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}

	// m_Fps 객체 초기화
	m_Fps->Initialize();

	// m_Cpu 객체 생성
	m_Cpu = new CpuClass;
	if (!m_Cpu) {
		return false;
	}

	// m_Cpu 객체 초기화
	m_Cpu->Initialize();
	*/

	// m_Timer 객체 생성
	m_Timer = new TimerClass;
	if (!m_Timer) {
		return false;
	}

	// m_Timer 객체 초기화
	if (!(m_Timer->Initialize()))
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer Object", L"Error", MB_OK);
		return false;
	}

	// m_Position 객체 생성
	m_Position = new PositionClass;
	if (!m_Position) {
		return false;
	}

	// m_Position 객체 초기화
	if (!(m_Position))
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// m_Position 객체 반환
	if (m_Position) {
		delete m_Position;
		m_Position = 0;
	}

	// m_Timer 객체 반환
	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}

	/*
	// m_Cpu 객체 반환
	if (m_Cpu) {
		delete m_Cpu;
		m_Cpu = 0;
	}

	// m_Fps 객체 반환
	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}

	// m_Sound 객체 반환
	if (m_Sound) {
		delete m_Sound;
		m_Sound = 0;
	}
	*/

	// m_Input 객체 반환
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}

	// m_Graphics 객체 반환
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// 윈도우 종료
	ShutdownWindows();
}

void SystemClass::Run()
{
	// 메세지 구조체 생성 및 초기화
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 사용자로부터 종료 메세지를 받을 때까지 반복
	while (true) {
		// 윈도우 메세지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// 메세지가 없으면 즉시 제어권 반환
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 종료 메세지를 받을 경우 break
		if (msg.message == WM_QUIT)	break;
		else {
			if (!Frame()) {
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}

		// ESC키 입력 확인 후 눌렀다면 종료 처리
		if (m_Input->IsEscapePressed())	break;
	}
}

bool SystemClass::Frame()
{
	// ESC로 종료
	//if (m_Input->IsKeyDown(VK_ESCAPE))	return false;
	//return m_Graphics->Frame();

	// 시스템 통계 업데이트
	m_Timer->Frame();
	//m_Fps->Frame();
	//m_Cpu->Frame();
	if (!m_Input->Frame())	return false;

	// 업데이트 된 위치를 계산하기 위한 프레임 시간 설정
	m_Position->SetFrameTime(m_Timer->GetTime());

	// Direct Input 처리
	int mouseX = 0;
	int mouseY = 0;
	int keyCount = 0;

	// input 객체의 프레임 처리 수행
	if (!m_Input->Frame())	return false;

	// input 객체에서 마우스 위치 읽어옴
	//m_Input->GetMouseLocation(mouseX, mouseY);
	//m_Input->GetKeyCount(keyCount);
	bool keydownLeft = m_Input->IsLeftArrowPressed();
	m_Position->TurnLeft(keydownLeft);
	bool keydownRight = m_Input->IsRightArrowPressed();
	m_Position->TurnRight(keydownRight);
	float rotationY = 0.0f;
	m_Position->GetRotation(rotationY);

	// graphic 객체의 프레임 처리 수행
	if (!m_Graphics->Frame())	return false;

	return m_Graphics->Render();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// 외부 포인터를 객체로 지정
	ApplicationHandle = this;

	// 프로그램의 인스턴스
	m_hinstance = GetModuleHandle(NULL);

	// 프로그램 명
	m_applicationName = L"DirectXProject";

	// windows 클래스 설정
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows 클래스 등록
	RegisterClassEx(&wc);

	// 모니터 화면 해상도
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// 풀스크린 모드면 해상도를 desktop 해상도로 지정하고, 색상을 32bit로 지정
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else {
		screenWidth = 800;
		screenHeight = 600;
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 윈도우 생성 후 핸들 설정
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 윈도우를 화면에 표시하고 포커스 설정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서 표시설정
	ShowCursor(true);
}

void SystemClass::ShutdownWindows()
{
	if (FULL_SCREEN)	ChangeDisplaySettings(NULL, 0);

	// 창 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 프로그램 인스턴스 제거
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 외부포인터 참조 초기화
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 윈도우가 종료되거나 닫힌건지 확인
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 그 외는 시스템 클래스의 메세지 처리로 넘김
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}