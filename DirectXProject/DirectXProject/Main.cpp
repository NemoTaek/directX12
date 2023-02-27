#include "Stdafx.h"
#include "SystemClass.h"

// Console 진입점은 int main
// WinAPI 진입점은 int wWinMain
// 처음에 실행하면 오류 뜰텐데, 프로젝트 속성 - 링커 - 시스템 - 하위시스템을 콘솔 -> 창으로 변경해야 실행된다.
// 유니코드 지원이 필요하면 wWinMain, 아스키코드 지원이 필요하면 WinMain 으로 시작한다.
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR IpCmdLine, _In_ int nCmdShow)
{
	// System 객체 생성
	SystemClass* System = new SystemClass;
	if (!System) {
		return -1;
	}

	// System 객체 초기화 및 실행
	if (System->initialize()) {
		System->Run();
	}

	// System 객체 종료 및 메모리 반환
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}