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
	// �� �����Ӹ��� �ҷ����� 1�� ����
	m_count++;

	// 1�ʰ� ������ ������ ���� ����
	if (timeGetTime() >= (m_startTime + 1000)) {
		m_fps = m_count;
		m_count = 0;
		// timeGetTime(): �����찡 ���۵ǰ� ���ݱ��� �帥 �ð��� ms ������ DWORD���� �����ϴ� �Լ�
		m_startTime = timeGetTime();
	}
}

int FpsClass::GetFps() { return m_fps; }