#include "Stdafx.h"
#include "InputClass.h"

#define DIRECTINPUT_VERSION 0x0800

InputClass::InputClass() {}
InputClass::InputClass(const InputClass& other) {}
InputClass::~InputClass() {}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Direct Input �������̽� �ʱ�ȭ
	if (FAILED(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_directInput), nullptr)))	return false;

	// Ű������ Direct Input �������̽� ����
	if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr)))	return false;

	// Ű���� ������ ���� ����
	if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))	return false;

	// �ٸ� ���α׷��� ������ �� �ֵ��� Ű������ ���� ���� ����
	// DISCL_EXCLUSIVE �÷��׷� �ٸ� ���α׷���� �������� �ʴ´�(���� ����)�� ����
	if (FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))	return false;

	// Ű���� �Ҵ�
	if (FAILED (m_keyboard->Acquire()))	return false;

	// ���콺�� ���������� ����
	if (FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr)))	return false;
	if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)))	return false;
	// ���콺�� �� ���� ���·� ����
	// ���� ���콺�� ��Ŀ���� �Ҿ����� �ƴ��� Ȯ���� �ʿ�
	if (FAILED(m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))	return false;
	if (FAILED(m_mouse->Acquire()))	return false;

	return true;
}

void InputClass::Shutdown()
{
	// ���콺 ��ü ��ȯ
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = nullptr;
	}

	// Ű���� ��ü ��ȯ
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = nullptr;
	}

	// m_directInput ��ü ��ȯ
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = nullptr;
	}
}

bool InputClass::Frame()
{
	// Ű����, ���콺�� ���� ���¸� �о��
	if (!ReadKeyboard())	return false;
	if (!ReadMouse())	return false;

	// Ű����� ���콺�� ������� ó��
	ProcessInput();
	return true;
}

bool InputClass::IsEscapePressed()
{
	// esc�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_ESCAPE] & 0x80)	return true;
	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

void InputClass::GetKeyCount(int& keyCount)
{
	keyCount = m_count;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), reinterpret_cast<LPVOID>(&m_keyboardState));
	if (FAILED(result)) {
		// Ű���尡 ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� �ٽ� ȹ��
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<LPVOID>(&m_mouseState));
	if (FAILED(result)) {
		// ���콺�� ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� �ٽ� ȹ��
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			m_mouse->Acquire();
		}
		else {
			return false;
		}
	}
	return true;
}

void InputClass::ProcessInput()
{
	// �����ӵ��� ���콺 ��ġ�� ������ ������� ���콺 Ŀ���� ��ġ�� ������Ʈ
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// ���콺 ��ġ�� ȭ�� �ʺ� �Ǵ� ���̸� ����� �ʴ��� Ȯ��
	if (m_mouseX < 0)	m_mouseX = 0;
	if (m_mouseY < 0)	m_mouseY = 0;
	if (m_mouseX > m_screenWidth)	m_mouseX = m_screenWidth;
	if (m_mouseY > m_screenHeight)	m_mouseY = m_screenHeight;

	// �� ȭ��ǥ ������ ī��Ʈ ����, �Ʒ� ȭ��ǥ ������ ī��Ʈ ����
	if (IsUpArrowPressed())		m_count++;
	if (IsDownArrowPressed())	m_count--;
}

bool InputClass::IsUpArrowPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_UP] & 0x80)	return true;
	return false;
}

bool InputClass::IsDownArrowPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_DOWN] & 0x80)	return true;
	return false;
}

bool InputClass::IsLeftArrowPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_LEFT] & 0x80)	return true;
	return false;
}

bool InputClass::IsRightArrowPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_RIGHT] & 0x80)	return true;
	return false;
}

bool InputClass::IsWPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_W] & 0x80)	return true;
	return false;
}

bool InputClass::IsAPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_A] & 0x80)	return true;
	return false;
}

bool InputClass::IsSPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_S] & 0x80)	return true;
	return false;
}

bool InputClass::IsDPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_D] & 0x80)	return true;
	return false;
}

bool InputClass::IsPgUpPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_PGUP] & 0x80)	return true;
	return false;
}

bool InputClass::IsPgDnPressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_PGDN] & 0x80)	return true;
	return false;
}

bool InputClass::IsSpacePressed()
{
	// �� ȭ��ǥ�� ������ �ִ��� ��Ʈ���� ����Ͽ� Ȯ��
	if (m_keyboardState[DIK_SPACE] & 0x80)	return true;
	return false;
}