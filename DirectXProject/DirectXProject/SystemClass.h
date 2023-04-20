#pragma once

// WinAPI 윈도우 프로그래밍 클래스화

class InputClass;
class GraphicsClass;
class SoundClass;
//class FpsClass;
//class CpuClass;
class TimerClass;
class PositionClass;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool initialize();
	void Shutdown();
	void Run();

	// 응용 프로그램이 실행되는 동안 전송될 윈도우 시스템 메시지 처리
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// 입력과 그래픽을 처리할 두 객체에 대한 포인터
	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
	SoundClass* m_Sound = nullptr;
	//FpsClass* m_Fps = nullptr;
	//CpuClass* m_Cpu = nullptr;
	TimerClass* m_Timer = nullptr;
	PositionClass* m_Position = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;