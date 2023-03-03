#include "Stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
//#include "ModelClass.h"
//#include "ColorShaderClass.h"
#include "ModelTextureClass.h"
#include "TextureShaderClass.h"

#include <iostream>
using namespace std;

GraphicsClass::GraphicsClass() {}
GraphicsClass::GraphicsClass(const GraphicsClass& other) {}
GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	//m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	m_Direct3D = new D3DClass;
	if (!m_Direct3D) return false;

	// Direct3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 그래픽카드 정보 파일 입출력
	FILE* fp = nullptr;
	fopen_s(&fp, "비디오 카드 정보.txt", "wt");
	if (fp != nullptr)
	{
		char cardName[255] = {};
		int memory = 0;
		m_Direct3D->GetVideoCardInfo(cardName, memory);
		fprintf_s(fp, "그래픽 카드 이름: %s \n", cardName);
		fprintf_s(fp, "그래픽 카드 메모리 크기: %d", memory);
		fclose(fp);
	}

	// 카메라 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera) { return false; }
	// 카메라 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//// 모델 객체 생성
	//m_Model = new ModelClass;
	//if (!m_Model) { return false; }
	//// 모델 객체 초기화
	//if (!m_Model->Initialize(m_Direct3D->GetDevice())) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	// 텍스쳐 모델 객체 생성
	m_ModelTexture = new ModelTextureClass;
	if (!m_ModelTexture) { return false; }
	// 텍스쳐 모델 객체 초기화
	if (!m_ModelTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"./Textures/checkboard.dds")) {
		MessageBox(hwnd, L"Could not initialize the model texture object", L"Error", MB_OK);
		return false;
	}

	//// 셰이더 객체 생성
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader) { return false; }
	//// 셰이더 객체 초기화
	//if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
	//	return false;
	//}

	// 텍스쳐 셰이더 객체 생성
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) { return false; }
	// 텍스쳐 셰이더 객체 초기화
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// 텍스쳐 셰이더 객체 반환
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 넥스쳐 모델 객체 반환
	if (m_ModelTexture) {
		m_ModelTexture->Shutdown();
		delete m_ModelTexture;
		m_ModelTexture = 0;
	}

	//// 셰이더 객체 반환
	//if (m_ColorShader) {
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = 0;
	//}

	//// 모델 객체 반환
	//if (m_Model) {
	//	m_Model->Shutdown();
	//	delete m_Model;
	//	m_Model = 0;
	//}

	// 카메라 객체 반환
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	// 그래픽 렌더링 처리
	return Render();
}

bool GraphicsClass::Render()
{
	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	/*m_Model->Render(m_Direct3D->GetDeviceContext());*/
	m_ModelTexture->Render(m_Direct3D->GetDeviceContext());

	//// 셰이더를 사용하여 모델 렌더링
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// 텍스쳐 셰이더를 사용하여 모델 렌더링
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
} 