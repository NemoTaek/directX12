#include "Stdafx.h"
#include "SystemClass.h"

// Console �������� int main
// WinAPI �������� int wWinMain
// ó���� �����ϸ� ���� ���ٵ�, ������Ʈ �Ӽ� - ��Ŀ - �ý��� - �����ý����� �ܼ� -> â���� �����ؾ� ����ȴ�.
// �����ڵ� ������ �ʿ��ϸ� wWinMain, �ƽ�Ű�ڵ� ������ �ʿ��ϸ� WinMain ���� �����Ѵ�.
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR IpCmdLine, _In_ int nCmdShow)
{
	// System ��ü ����
	SystemClass* System = new SystemClass;
	if (!System) {
		return -1;
	}

	// System ��ü �ʱ�ȭ �� ����
	if (System->initialize()) {
		System->Run();
	}

	// System ��ü ���� �� �޸� ��ȯ
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}