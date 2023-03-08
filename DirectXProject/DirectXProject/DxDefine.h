#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")	// direct Input��
#pragma comment(lib, "dinput8.lib")		// direct Input��
#pragma comment(lib, "dxguid.lib")		// direct input, sound��
#pragma comment(lib, "dsound.lib")		// direct sound��
#pragma comment(lib, "winmm.lib")		// direct sound��, fps��
#pragma comment(lib, "pdh.lib")			// cpu��

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dinput.h>						// direct Input��
#include <mmsystem.h>					// direct sound��, fps��
#include <dsound.h>						// direct sound��
#include <Pdh.h>						// cpu��

#include "DDSTextureLoader.h"
using namespace DirectX;

// warning C4316 ó���� include
// warning C4316: 'D3DClass': ���� �Ҵ�� ��ü�� 16�� ���߾��� ���� ���� ���� �ֽ��ϴ�.
// XMMATRIX ���ǿ� __declspec(align(16))���� 16����Ʈ ������ ����Ѵ�.
// ���⼭ ���� �Ҵ��� �� ��� �߻��Ѵ�.
// Visual Studio�� ����Ǿ� ���Ǵ� new �������� ��� 8byte align���� �Ҵ�ǹǷ� 16byte align�� �ǵ��� ���� ����� ��� �����ϵ��� �������־�� �Ѵ�.
// �̸� �������� ������ CPU���� SSE ��ɾ� ������ ó���� �� ���ĵǴ� �����͸� �����ϰ� ������� �� ����.
#include "AlignedAllocationPolicy.h"