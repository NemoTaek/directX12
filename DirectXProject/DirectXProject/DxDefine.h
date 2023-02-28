#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

// warning C4316 처리용 include
// warning C4316: 'D3DClass': 힙에 할당된 개체가 16에 맞추어져 있지 않을 수도 있습니다.
// XMMATRIX 정의에 __declspec(align(16))으로 16바이트 정렬을 사용한다.
// 여기서 힙에 할당할 때 경고가 발생한다.
// Visual Studio에 내장되어 사용되는 new 연산자의 경우 8byte align으로 할당되므로 16byte align이 되도록 새로 만들어 사용 가능하도록 보장해주어야 한다.
// 이를 포함하지 않으면 CPU에서 SSE 명령어 연산을 처리할 때 정렬되는 데이터를 안전하게 보장받을 수 없다.
#include "AlignedAllocationPolicy.h"