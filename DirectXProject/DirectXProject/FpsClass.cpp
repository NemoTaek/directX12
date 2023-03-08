#include "Stdafx.h"
#include "FpsClass.h"

FpsClass::FpsClass() {}
FpsClass::FpsClass(const FpsClass& other) {}
FpsClass::~FpsClass() {}

void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
}

void FpsClass::Frame()
{
	// 매 프레임마다 불려져서 1씩 증가
	m_count++;

	// 1초가 지나면 프레임 수를 저장
	if (timeGetTime() >= (m_startTime + 1000)) {
		m_fps = m_count;
		m_count = 0;
		// timeGetTime(): 윈도우가 시작되고 지금까지 흐른 시간을 ms 단위로 DWORD형을 리턴하는 함수
		m_startTime = timeGetTime();
	}
}

int FpsClass::GetFps() { return m_fps; }