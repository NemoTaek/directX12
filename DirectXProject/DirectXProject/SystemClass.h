#pragma once

// WinAPI ������ ���α׷��� Ŭ����ȭ

class InputClass;
class GraphicsClass;
class SoundClass;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool initialize();
	void Shutdown();
	void Run();

	// ���� ���α׷��� ����Ǵ� ���� ���۵� ������ �ý��� �޽��� ó��
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// �Է°� �׷����� ó���� �� ��ü�� ���� ������
	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
	SoundClass* m_Sound = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;