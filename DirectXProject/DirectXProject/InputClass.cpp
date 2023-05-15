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

	// Direct Input 인터페이스 초기화
	if (FAILED(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_directInput), nullptr)))	return false;

	// 키보드의 Direct Input 인터페이스 생성
	if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr)))	return false;

	// 키보드 데이터 포맷 설정
	if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))	return false;

	// 다른 프로그램과 공유할 수 있도록 키보드의 협력 수준 설정
	// DISCL_EXCLUSIVE 플래그로 다른 프로그램들과 공유하지 않는다(배제 상태)고 설정
	if (FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))	return false;

	// 키보드 할당
	if (FAILED (m_keyboard->Acquire()))	return false;

	// 마우스도 마찬가지로 진행
	if (FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr)))	return false;
	if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)))	return false;
	// 마우스는 비 배제 상태로 설정
	// 따라서 마우스가 포커스를 잃었는지 아닌지 확인이 필요
	if (FAILED(m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))	return false;
	if (FAILED(m_mouse->Acquire()))	return false;

	return true;
}

void InputClass::Shutdown()
{
	// 마우스 객체 반환
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = nullptr;
	}

	// 키보드 객체 반환
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = nullptr;
	}

	// m_directInput 객체 반환
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = nullptr;
	}
}

bool InputClass::Frame()
{
	// 키보드, 마우스의 현재 상태를 읽어옴
	if (!ReadKeyboard())	return false;
	if (!ReadMouse())	return false;

	// 키보드와 마우스의 변경상태 처리
	ProcessInput();
	return true;
}

bool InputClass::IsEscapePressed()
{
	// esc를 누르고 있는지 비트값을 계산하여 확인
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
		// 키보드가 포커스를 잃었거나 획득되지 않은 경우 다시 획득
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
		// 마우스가 포커스를 잃었거나 획득되지 않은 경우 다시 획득
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
	// 프레임동안 마우스 위치의 변경을 기반으로 마우스 커서의 위치를 업데이트
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 마우스 위치가 화면 너비 또는 높이를 벗어나지 않는지 확인
	if (m_mouseX < 0)	m_mouseX = 0;
	if (m_mouseY < 0)	m_mouseY = 0;
	if (m_mouseX > m_screenWidth)	m_mouseX = m_screenWidth;
	if (m_mouseY > m_screenHeight)	m_mouseY = m_screenHeight;

	// 위 화살표 누르면 카운트 증가, 아래 화살표 누르면 카운트 감소
	if (IsUpArrowPressed())		m_count++;
	if (IsDownArrowPressed())	m_count--;
}

bool InputClass::IsUpArrowPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_UP] & 0x80)	return true;
	return false;
}

bool InputClass::IsDownArrowPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_DOWN] & 0x80)	return true;
	return false;
}

bool InputClass::IsLeftArrowPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_LEFT] & 0x80)	return true;
	return false;
}

bool InputClass::IsRightArrowPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_RIGHT] & 0x80)	return true;
	return false;
}

bool InputClass::IsWPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_W] & 0x80)	return true;
	return false;
}

bool InputClass::IsAPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_A] & 0x80)	return true;
	return false;
}

bool InputClass::IsSPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_S] & 0x80)	return true;
	return false;
}

bool InputClass::IsDPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_D] & 0x80)	return true;
	return false;
}

bool InputClass::IsPgUpPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_PGUP] & 0x80)	return true;
	return false;
}

bool InputClass::IsPgDnPressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_PGDN] & 0x80)	return true;
	return false;
}

bool InputClass::IsSpacePressed()
{
	// 위 화살표를 누르고 있는지 비트값을 계산하여 확인
	if (m_keyboardState[DIK_SPACE] & 0x80)	return true;
	return false;
}