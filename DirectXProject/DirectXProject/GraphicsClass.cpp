#include "Stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
//#include "ModelClass.h"
//#include "ColorShaderClass.h"
//#include "ModelTextureClass.h"
//#include "TextureShaderClass.h"
//#include "ModelLightClass.h"
#include "Model3DClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
//#include "BitmapClass.h"
#include "TextClass.h"
#include "FrustumClass.h"
#include "ModelListClass.h"

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
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//// 모델 객체 생성
	//m_Model = new ModelClass;
	//if (!m_Model) { return false; }
	//// 모델 객체 초기화
	//if (!m_Model->Initialize(m_Direct3D->GetDevice())) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ModelTexture = new ModelTextureClass;
	//if (!m_ModelTexture) { return false; }
	//// 텍스쳐 모델 객체 초기화
	//if (!m_ModelTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model texture object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ModelLight = new ModelLightClass;
	//if (!m_ModelLight) { return false; }
	//if (!m_ModelLight->Initialize(m_Direct3D->GetDevice(), L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	m_Model3D = new Model3DClass;
	if (!m_Model3D) { return false; }
	if (!m_Model3D->Initialize(m_Direct3D->GetDevice(), L"./data/sphere.txt", L"./Textures/checkboard.dds")) {
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
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

	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader) { return false; }
	//// 텍스쳐 셰이더 객체 초기화
	//if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
	//	return false;
	//}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader) { return false; }
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass;
	if (!m_Light) { return false; }
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	//// 2D 모델 객체 생성
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap) { return false; }
	//// 2D 모델 객체 초기화
	//if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, L"./Textures/ice.dds", 256, 256)) {
	//	MessageBox(hwnd, L"Could not initialize the bitmap object", L"Error", MB_OK);
	//	return false;
	//}

	// 텍스트 객체 생성
	m_Text = new TextClass;
	if (!m_Text) { return false; }
	// 텍스트 객체 초기화
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix)) {
		MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
		return false;
	}

	// 랜덤 모델 리스트 객체 생성
	m_ModelList = new ModelListClass;
	if (!m_ModelList) { return false; }
	// 랜덤 모델 리스트 객체 초기화
	if (!m_ModelList->Initialize(25)) {
		MessageBox(hwnd, L"Could not initialize the model list object", L"Error", MB_OK);
		return false;
	}

	// 절두체 객체 생성
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }

	return true;
}

void GraphicsClass::Shutdown()
{
	// 절두체 객체 반환
	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

	// 랜덤 모델 리스트 객체 반환
	if (m_ModelList) {
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// 텍스트 객체 반환
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// 비트맵 객체 반환
	//if (m_Bitmap) {
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = 0;
	//}

	// light 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 셰이더 객체 반환
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// 셰이더 객체 반환
	//if (m_TextureShader) {
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}

	// 모델 객체 반환
	//if (m_ModelLight) {
	//	m_ModelLight->Shutdown();
	//	delete m_ModelLight;
	//	m_ModelLight = 0;
	//}

	// 모델 객체 반환
	if (m_Model3D) {
		m_Model3D->Shutdown();
		delete m_Model3D;
		m_Model3D = 0;
	}

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

//bool GraphicsClass::Frame(int mouseX, int mouseY, int keyCount, int fps, int cpu, float frameTime)
bool GraphicsClass::Frame(float rotationY)
{
	// 모델 회전용 코드
	/*
	static float rotation = 0.0f;

	// 각 프레임의 회전을 업데이트
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// 그래픽 렌더링 처리
	return Render(rotation);
	*/

	// Input 용 코드
	//if (!m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetKeyboardInput(keyCount, m_Direct3D->GetDeviceContext()))	return false;

	// fps, cpu, timer 용 코드
	//if(!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))	return false;

	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	// 렌더링할 모델 세팅
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f;
	XMFLOAT4 color;

	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 매 프레임마다 시야 행렬에 근거하여 절두체를 생성
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 랜더링 될 모델의 수 세팅 및 초기화
	int modelCount = m_ModelList->GetModelCount();
	int renderCount = 0;

	// 모든 모델을 탐색하고, 카메라 뷰에 보여지는 모델만 렌더링
	for (int i = 0; i < modelCount; i++) {
		// 모델 위치와 색상 세팅
		m_ModelList->GetData(i, positionX, positionY, positionZ, color);
		
		// 모델이 절두체 안에 있는지 확인
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius)) {
			// 모델을 렌더링 할 위치로 이동
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			
			// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
			m_Model3D->Render(m_Direct3D->GetDeviceContext());

			// 빛 셰이더를 사용하여 모델 렌더링
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

			// 세계 행렬 리셋
			m_Direct3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	// 모델이 회전할 수 있도록 회전 값으로 세계 행렬 세팅
	//worldMatrix = XMMatrixRotationY(rotation);

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 알파 블랜딩 on
	m_Direct3D->TurnOnAlphaBlending();

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	//m_Model->Render(m_Direct3D->GetDeviceContext());
	//m_ModelTexture->Render(m_Direct3D->GetDeviceContext());
	//m_model3D->Render(m_Direct3D->GetDeviceContext());
	//if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 400, 300))	return false;

	// 셰이더를 사용하여 모델 렌더링
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

	// 텍스쳐 셰이더를 사용하여 모델 렌더링
	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;
	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture()))	return false;

	// 빛 셰이더를 사용하여 모델 렌더링
	//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_model3D->GetTexture(),
	//	m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;

	// 텍스트 문자열 렌더링
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))	return false;
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))	return false;

	// 알파 블랜딩 off
	m_Direct3D->TurnOffAlphaBlending();
	
	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
} 