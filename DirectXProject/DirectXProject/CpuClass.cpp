#include "Stdafx.h"
#include "CpuClass.h"

CpuClass::CpuClass() {}
CpuClass::CpuClass(const CpuClass& other) {}
CpuClass::~CpuClass() {}

void CpuClass::Initialize()
{
	// CPU 사용을 폴링하는 쿼리 개체 생성
	// 쿼리는 각각의 cpu 사용량 대신 시스템의 모든 cpu들의 사용량 총 합을 구하는 작업
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)	m_canReadCpu = false;

	// 시스템의 모든 CPU를 폴링하도록 쿼리 개체 설정
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)	m_canReadCpu = false;

	// GetTickCount(): 시스템이 시작한(컴퓨터를 킨) 시점부터 함수를 호출한 시간까지 흘러간 시간을 ms 단위로 DWORD형을 리턴하는 함수
	// 이 함수는 32비트값이라 최대 49.7일간만 카운팅 할수 있어서 문제가 생김
	// 반환값이 64비트로 증가한 함수가 나타나 최대 5억년 정도까지 카운팅 할 수 있도록 가능해짐
	m_lastSampleTime = GetTickCount64();
	m_cpuUsage = 0;
}

void CpuClass::Shutdown()
{
	// CPU 사용량을 쿼리하는데 사용한 핸들을 해제
	if (m_canReadCpu) {
		PdhCloseQuery(m_queryHandle);
	}
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu) {
		// 1초가 지나면 CPU 사용량을 샘플링
		if ((m_lastSampleTime + 1000) < GetTickCount64()) {
			m_lastSampleTime = GetTickCount64();
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_cpuUsage = value.longValue;
		}
	}
}

int CpuClass::GetCpuPercentage()
{
	int usage;

	if (m_canReadCpu) {
		usage = static_cast<int>(m_cpuUsage);
	}
	else {
		usage = 0;
	}

	return usage;
}