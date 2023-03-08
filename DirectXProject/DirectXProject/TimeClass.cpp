#include "Stdafx.h"
#include "TimerClass.h"

TimerClass::TimerClass() {}
TimerClass::TimerClass(const TimerClass& other) {}
TimerClass::~TimerClass() {}

bool TimerClass::Initialize()
{
	// �ý����� ���� Ÿ�̸Ӹ� �����ϴ��� Ȯ��
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));
	if (m_frequency == 0)	return false;

	// ī���Ϳ��� �� ms ���� ƽ�ϴ� Ƚ���� ���
	m_ticksPerMs = static_cast<float>(m_frequency / 1000);
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime = 0;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

	// ������ �ð��� ���
	float timeDifference = static_cast<float>(currentTime - m_startTime);

	// ������ �ð� ���
	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;
}

float TimerClass::GetTime() { return m_frameTime; }