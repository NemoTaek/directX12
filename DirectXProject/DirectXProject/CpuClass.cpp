#include "Stdafx.h"
#include "CpuClass.h"

CpuClass::CpuClass() {}
CpuClass::CpuClass(const CpuClass& other) {}
CpuClass::~CpuClass() {}

void CpuClass::Initialize()
{
	// CPU ����� �����ϴ� ���� ��ü ����
	// ������ ������ cpu ��뷮 ��� �ý����� ��� cpu���� ��뷮 �� ���� ���ϴ� �۾�
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)	m_canReadCpu = false;

	// �ý����� ��� CPU�� �����ϵ��� ���� ��ü ����
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)	m_canReadCpu = false;

	// GetTickCount(): �ý����� ������(��ǻ�͸� Ų) �������� �Լ��� ȣ���� �ð����� �귯�� �ð��� ms ������ DWORD���� �����ϴ� �Լ�
	// �� �Լ��� 32��Ʈ���̶� �ִ� 49.7�ϰ��� ī���� �Ҽ� �־ ������ ����
	// ��ȯ���� 64��Ʈ�� ������ �Լ��� ��Ÿ�� �ִ� 5��� �������� ī���� �� �� �ֵ��� ��������
	m_lastSampleTime = GetTickCount64();
	m_cpuUsage = 0;
}

void CpuClass::Shutdown()
{
	// CPU ��뷮�� �����ϴµ� ����� �ڵ��� ����
	if (m_canReadCpu) {
		PdhCloseQuery(m_queryHandle);
	}
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu) {
		// 1�ʰ� ������ CPU ��뷮�� ���ø�
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