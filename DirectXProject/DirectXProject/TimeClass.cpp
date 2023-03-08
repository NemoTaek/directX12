#include "Stdafx.h"
#include "TimerClass.h"

TimerClass::TimerClass() {}
TimerClass::TimerClass(const TimerClass& other) {}
TimerClass::~TimerClass() {}

bool TimerClass::Initialize()
{
	// 시스템이 고성능 타이머를 지원하는지 확인
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));
	if (m_frequency == 0)	return false;

	// 카운터에서 매 ms 마다 틱하는 횟수를 계산
	m_ticksPerMs = static_cast<float>(m_frequency / 1000);
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime = 0;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

	// 프레임 시간차 계산
	float timeDifference = static_cast<float>(currentTime - m_startTime);

	// 프레임 시간 계산
	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;
}

float TimerClass::GetTime() { return m_frameTime; }