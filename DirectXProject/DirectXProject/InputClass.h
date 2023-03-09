#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	void GetKeyCount(int&);
	bool IsUpArrowPressed();
	bool IsDownArrowPressed();
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	// Direct Input�� 8������ ������ ������Ʈ �����̱� ������ 8�������� ���
	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_mouseX = 0;
	int m_mouseY = 0;
	int m_count = 0;
};
