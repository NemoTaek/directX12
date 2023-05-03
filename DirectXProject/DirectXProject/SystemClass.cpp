#include "Stdafx.h"
#include "ApplicationClass.h"
#include "SystemClass.h"

SystemClass::SystemClass() {}
SystemClass::SystemClass(const SystemClass& other) {}
SystemClass::~SystemClass() {}

bool SystemClass::initialize()
{
	// ������ â ���� ����
	int screenWidth = 0;
	int screenHeight = 0;

	// ������ ���� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	m_Application = new ApplicationClass;
	if (!m_Application) return false;
	if (!m_Application->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight)) {
		MessageBox(m_hwnd, L"Could not initialize the application object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// m_Application ��ü ��ȯ
	if (m_Application) {
		delete m_Application;
		m_Application = 0;
	}

	// ������ ����
	ShutdownWindows();
}

void SystemClass::Run()
{
	// �޼��� ����ü ���� �� �ʱ�ȭ
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// ����ڷκ��� ���� �޼����� ���� ������ �ݺ�
	while (true) {
		// ������ �޼��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// ���� �޼����� ���� ��� break
			if (msg.message == WM_QUIT)	break;

			// �޼����� ������ ��� ����� ��ȯ
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		else {
			if (!Frame()) {
				//MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}

		// ESCŰ �Է� Ȯ�� �� �����ٸ� ���� ó��
		//if (m_Input->IsEscapePressed())	break;
	}
}

bool SystemClass::Frame()
{
	//XMFLOAT3 position, rotation;

	//// �ý��� ��� ������Ʈ
	//m_Timer->Frame();
	////m_Fps->Frame();
	////m_Cpu->Frame();
	//if (!m_Input->Frame())	return false;

	//// ������Ʈ �� ��ġ�� ����ϱ� ���� ������ �ð� ����
	//m_Position->SetFrameTime(m_Timer->GetTime());

	//// Direct Input ó��
	//int mouseX = 0;
	//int mouseY = 0;
	//int keyCount = 0;

	//// input ��ü�� ������ ó�� ����
	//if (!m_Input->Frame())	return false;

	//// input ��ü���� ���콺 ��ġ�� Ű���� �Է��� �о��
	////m_Input->GetMouseLocation(mouseX, mouseY);
	////m_Input->GetKeyCount(keyCount);
	//bool keydownLeft = m_Input->IsLeftArrowPressed();
	//m_Position->MoveLeft(keydownLeft);
	//bool keydownRight = m_Input->IsRightArrowPressed();
	//m_Position->MoveRight(keydownRight);

	//// ���� ��ġ ����
	//m_Position->GetPosition(position);
	//m_Position->GetRotation(rotation);

	// graphic ��ü�� ������ ó�� ����
	//if (!m_Graphics->Frame())	return false;
	//if (!m_Graphics->Frame(position))	return false;
	//if (!m_Graphics->Frame(position, rotation))	return false;

	// �ð��� ���� ó���Ǿ�� �ϴ� ������ ����Ǿ�� �ϸ� �Ʒ��� ����
	 //if (!m_Graphics->Frame(m_Timer->GetTime()))	return false;

	//return m_Graphics->Render();
	return m_Application->Frame();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// �ܺ� �����͸� ��ü�� ����
	ApplicationHandle = this;

	// ���α׷��� �ν��Ͻ�
	m_hinstance = GetModuleHandle(NULL);

	// ���α׷� ��
	m_applicationName = L"DirectXProject";

	// windows Ŭ���� ����
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

	// windows Ŭ���� ���
	RegisterClassEx(&wc);

	// ����� ȭ�� �ػ�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �ػ󵵸� desktop �ػ󵵷� �����ϰ�, ������ 32bit�� ����
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

	// ������ ���� �� �ڵ� ����
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ�� ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���콺 Ŀ�� ǥ�ü���
	ShowCursor(true);
}

void SystemClass::ShutdownWindows()
{
	if (FULL_SCREEN)	ChangeDisplaySettings(NULL, 0);

	// â ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� �ν��Ͻ� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �ܺ������� ���� �ʱ�ȭ
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// �����찡 ����ǰų� �������� Ȯ��
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

		// �� �ܴ� �ý��� Ŭ������ �޼��� ó���� �ѱ�
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}