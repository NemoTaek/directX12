#include "Stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
//#include "ViewPointClass.h"
//#include "TextureClass.h"
//#include "TextureShaderClass.h"
#include "Model3DClass.h"
//#include "LightShaderClass.h"
#include "LightClass.h"
//#include "ExampleWaterShaderClass.h"
//#include "BitmapClass.h"
//#include "TextClass.h"
//#include "FrustumClass.h"
//#include "ModelListClass.h"
//#include "BumpMapShaderClass.h"
//#include "RenderTextureClass.h"
//#include "DebugWindowClass.h"
//#include "FogShaderClass.h"
//#include "TransparentShaderClass.h"
//#include "ReflectionShaderClass.h"
//#include "FadeShaderClass.h"
//#include "RefractionShaderClass.h"
//#include "FireShaderClass.h"
//#include "DepthShaderClass.h"
//#include "HorizontalBlurShaderClass.h"
//#include "VerticalBlurShaderClass.h"
//#include "OrthoWindowClass.h"
//#include "ModelInstanceClass.h"
//#include "TessellationShaderClass.h"
//#include "ModelTessellationClass.h"
//#include "ParticleSystemClass.h"
//#include "ProjectionShaderClass.h"
#include "ShaderManagerClass.h"

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
	m_Camera->SetRotation(70.0f, 0.0f, 0.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//m_ViewPoint = new ViewPointClass;
	//if (!m_ViewPoint) { return false; }
	//m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
	//m_ViewPoint->SetLookAt(0.0f, 0.0f, 0.0f);
	//m_ViewPoint->SetProjectionParameters(static_cast<float>((XM_PI) / 2.0f), 1.0f, 0.1f, 100.0f);
	//m_ViewPoint->GenerateViewMatrix();
	//m_ViewPoint->GenerateProjectionMatrix();

	m_Model3D = new Model3DClass;
	if (!m_Model3D) { return false; }
	if (!m_Model3D->Initialize(m_Direct3D->GetDevice(), L"./data/cube.txt", L"./Textures/checkboard.dds")) {
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//m_BumpMapShader = new BumpMapShaderClass;
	//if (!m_BumpMapShader) { return false; }
	//if (!m_BumpMapShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the bump map shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ProjectionTexture = new TextureClass;
	//if (!m_ProjectionTexture) { return false; }
	//if (!m_ProjectionTexture->Initialize(m_Direct3D->GetDevice(),L"./Textures/grate.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the projection model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_TextureShader = new TextureShaderClass;
	//if (!m_TextureShader) { return false; }
	//// 텍스쳐 셰이더 객체 초기화
	//if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_LightShader = new LightShaderClass;
	//if (!m_LightShader) { return false; }
	//if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
	//	return false;
	//}

	m_Light = new LightClass;
	if (!m_Light) { return false; }
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	//m_Light->SetPosition(2.0f, 5.0f, -2.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);

	//m_Light1 = new LightClass;
	//if (!m_Light1) { return false; }
	//m_Light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	//m_Light1->SetPosition(-3.0f, 1.0f, 3.0f);
	//m_Light2 = new LightClass;
	//if (!m_Light2) { return false; }
	//m_Light2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	//m_Light2->SetPosition(3.0f, 1.0f, 3.0f);
	//m_Light3 = new LightClass;
	//if (!m_Light3) { return false; }
	//m_Light3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	//m_Light3->SetPosition(-3.0f, 1.0f, -3.0f);
	//m_Light4 = new LightClass;
	//if (!m_Light4) { return false; }
	//m_Light4->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	//m_Light4->SetPosition(3.0f, 1.0f, -3.0f);
	//m_Light5 = new LightClass;
	//if (!m_Light5) { return false; }
	//m_Light5->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light5->SetPosition(0.0f, 1.0f, 0.0f);

	// 2D 모델 객체 생성
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap) { return false; }
	//// 2D 모델 객체 초기화
	//if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight)) {
	//	MessageBox(hwnd, L"Could not initialize the bitmap object", L"Error", MB_OK);
	//	return false;
	//}

	//// 텍스트 객체 생성
	//m_Text = new TextClass;
	//if (!m_Text) { return false; }
	//// 텍스트 객체 초기화
	//if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix)) {
	//	MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
	//	return false;
	//}

	//// 랜덤 모델 리스트 객체 생성
	//m_ModelList = new ModelListClass;
	//if (!m_ModelList) { return false; }
	//// 랜덤 모델 리스트 객체 초기화
	//if (!m_ModelList->Initialize(25)) {
	//	MessageBox(hwnd, L"Could not initialize the model list object", L"Error", MB_OK);
	//	return false;
	//}

	//// 절두체 객체 생성
	//m_Frustum = new FrustumClass;
	//if (!m_Frustum) { return false; }

	//m_RenderTexture = new RenderTextureClass;
	//if (!m_RenderTexture) { return false; }
	//if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR)) {
	//	return false;
	//}

	//m_RefractionShader = new RefractionShaderClass;
	//if (!m_RefractionShader) { return false; }
	//if (!m_RefractionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the refraction shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_DebugWindow = new DebugWindowClass;
	//if (!m_DebugWindow) { return false; }
	//if (!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100)) {
	//	MessageBox(hwnd, L"Could not initialize the debug window object", L"Error", MB_OK);
	//	return false;
	//}

	//m_FogShader = new FogShaderClass;
	//if (!m_FogShader) { return false; }
	//if (!m_FogShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the fog shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ReflectionShader = new ReflectionShaderClass;
	//if (!m_ReflectionShader) { return false; }
	//if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the transparent shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_fadeInTime = 3000.0f;	// 페이드 인 효과 시간 설정
	//m_accumulatedTime = 0;	// 누적 시간
	//m_fadePercentage = 0;	// 페이드 퍼센트를 설정하여 페이드 효과 시작지점 설정
	//m_fadeDone = false;		// 페이드 효과 완료 유무
	//m_FadeShader = new FadeShaderClass;
	//if (!m_FadeShader) { return false; }
	//if (!m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the fade shader object", L"Error", MB_OK);
	//	return false;
	//}

	/*
	// 물 효과를 위한 클래스
	m_GroundModel = new Model3DClass;
	if (!m_GroundModel->Initialize(m_Direct3D->GetDevice(), L"./data/ground.txt", L"./Textures/ground01.dds")) {
		MessageBox(hwnd, L"Could not initialize the ground model object", L"Error", MB_OK);
		return false;
	}
	m_WallModel = new Model3DClass;
	if (!m_WallModel->Initialize(m_Direct3D->GetDevice(), L"./data/wall.txt", L"./Textures/wall01.dds")) {
		MessageBox(hwnd, L"Could not initialize the wall model object", L"Error", MB_OK);
		return false;
	}
	m_BathModel = new Model3DClass;
	if (!m_BathModel->Initialize(m_Direct3D->GetDevice(), L"./data/bath.txt", L"./Textures/marble01.dds")) {
		MessageBox(hwnd, L"Could not initialize the bath model object", L"Error", MB_OK);
		return false;
	}
	m_WaterModel = new Model3DClass;
	if (!m_WaterModel->Initialize(m_Direct3D->GetDevice(), L"./data/water.txt", L"./Textures/water01.dds")) {
		MessageBox(hwnd, L"Could not initialize the water model object", L"Error", MB_OK);
		return false;
	}
	m_ExampleWaterShader = new ExampleWaterShaderClass;
	if (!m_ExampleWaterShader) { return false; }
	if (!m_ExampleWaterShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
		return false;
	}
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture) { return false; }
	if (!m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the reflection texture object", L"Error", MB_OK);
		return false;
	}
	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture) { return false; }
	if (!m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the refraction texture object", L"Error", MB_OK);
		return false;
	}
	m_WaterShader = new ReflectionShaderClass;
	if (!m_WaterShader) { return false; }
	if (!m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the water shader object", L"Error", MB_OK);
		return false;
	}
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader) { return false; }
	if (!m_RefractionShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the refraction shader object", L"Error", MB_OK);
		return false;
	}
	m_waterHeight = 2.75f;
	m_waterTranslation = 0.0f;
	*/

	/*
	// 유리, 얼음 효과
	m_Model3D2 = new Model3DClass;
	if (!m_Model3D2) { return false; }
#define GLASS true	// true: 유리 , false: 얼음
#if GLASS
	if (!m_Model3D2->Initialize(m_Direct3D->GetDevice(), L"./data/square.txt", L"./Textures/glass01.dds", L"./Textures/bump03.dds"))
#else
	if (!m_Model3D2->Initialize(m_Direct3D->GetDevice(), L"./data/square.txt", L"./Textures/ice01.dds", L"./Textures/icebump01.dds"))
#endif GLASS
	{
		MessageBox(hwnd, L"Could not initialize the model2 object", L"Error", MB_OK);
		return false;
	}
	*/

	//m_FireShader = new FireShaderClass;
	//if (!m_FireShader) { return false; }
	//if (!m_FireShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the fire shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_Billboard = new Model3DClass;
	//if (!m_Billboard) { return false; }
	//if (!m_Billboard->Initialize(m_Direct3D->GetDevice(), L"./data/square.txt", L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_DepthShader = new DepthShaderClass;
	//if (!m_DepthShader) { return false; }
	//if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the depth shader object", L"Error", MB_OK);
	//	return false;
	//}

	/*
	// 블러 효과
	int downSampleWidth = screenWidth / 2;
	int downSampleHeight = screenHeight / 2;

	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader) { return false; }
	if (!m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader) { return false; }
	if (!m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) { return false; }
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_DownSampleTexture = new RenderTextureClass;
	if (!m_DownSampleTexture) { return false; }
	if (!m_DownSampleTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture) { return false; }
	if (!m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture) { return false; }
	if (!m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_UpSampleTexture = new RenderTextureClass;
	if (!m_UpSampleTexture) { return false; }
	if (!m_UpSampleTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)) {
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow) { return false; }
	if (!m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight)) {
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}

	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow) { return false; }
	if (!m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight)) {
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}
	*/

	//m_ModelInstance = new ModelInstanceClass;
	//if (!m_ModelInstance) { return false; }
	//if (!m_ModelInstance->Initialize(m_Direct3D->GetDevice(), L"./Textures/checkboard.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ModelTessellation = new ModelTessellationClass;
	//if (!m_ModelTessellation) { return false; }
	//if (!m_ModelTessellation->Initialize(m_Direct3D->GetDevice())) {
	//	MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
	//	return false;
	//}

	//m_TessellationShader = new TessellationShaderClass;
	//if (!m_TessellationShader) { return false; }
	//if (!m_TessellationShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the tessellation shader object", L"Error", MB_OK);
	//	return false;
	//}

	//m_ParticleSystem = new ParticleSystemClass;
	//if (!m_ParticleSystem) { return false; }
	//if (!m_ParticleSystem->Initialize(m_Direct3D->GetDevice(), L"./Textures/star.dds")) {
	//	MessageBox(hwnd, L"Could not initialize the particle object", L"Error", MB_OK);
	//	return false;
	//}

	m_Model2 = new Model3DClass;
	if (!m_Model2->Initialize(m_Direct3D->GetDevice(), L"./data/cube.txt", L"./Textures/ice.dds")) {
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager) { return false; }
	if (!m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the shader manager object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// 셰이더 객체 반환
	if (m_ShaderManager) {
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Model2) {
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	//// 텍스트 객체 반환
	//if (m_Text) {
	//	m_Text->Shutdown();
	//	delete m_Text;
	//	m_Text = 0;
	//}

	// 비트맵 객체 반환
	//if (m_Bitmap) {
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = 0;
	//}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	 // 셰이더 객체 반환
	//if (m_LightShader) {
	//	m_LightShader->Shutdown();
	//	delete m_LightShader;
	//	m_LightShader = 0;
	//}

	// 셰이더 객체 반환
	//if (m_TextureShader) {
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}

	//if (m_ProjectionTexture) {
	//	m_ProjectionTexture->Shutdown();
	//	delete m_ProjectionTexture;
	//	m_ProjectionTexture = 0;
	//}

	// 모델 객체 반환
	//if (m_ModelLight) {
	//	m_ModelLight->Shutdown();
	//	delete m_ModelLight;
	//	m_ModelLight = 0;
	//}

	//// 범프 맵 쉐이더 객체 반환
	//if (m_BumpMapShader) {
	//	m_BumpMapShader->Shutdown();
	//	delete m_BumpMapShader;
	//	m_BumpMapShader = 0;
	//}

	// 모델 객체 반환
	if (m_Model3D) {
		m_Model3D->Shutdown();
		delete m_Model3D;
		m_Model3D = 0;
	}

	//if (m_ViewPoint) {
	//	delete m_ViewPoint;
	//	m_ViewPoint = 0;
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

//bool GraphicsClass::Frame(int mouseX, int mouseY, int keyCount, int fps, int cpu, float frameTime)
bool GraphicsClass::Frame()
{
	// Input 용 코드
	//if (!m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetKeyboardInput(keyCount, m_Direct3D->GetDeviceContext()))	return false;

	// fps, cpu, timer 용 코드
	//if(!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))	return false;
	//if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))	return false;

	//m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	//m_waterTranslation += 0.001f;
	//if (m_waterTranslation > 1.0f)	m_waterTranslation -= 1.0f;
	//m_Camera->SetPosition(0.0f, 0.0f, 0.0f);

	return true;
}
bool GraphicsClass::Frame(float frameTime)
{
	//if (!m_fadeDone) {
	//	// 누적 시간에 프레임 시간 추가
	//	m_accumulatedTime += frameTime;

	//	// 페이드인 중이라면
	//	if (m_accumulatedTime < m_fadeInTime) {
	//		// 페이드 퍼센트를 진행률로 계속 업에디트
	//		m_fadePercentage = m_accumulatedTime / m_fadeInTime;
	//	}
	//	else {
	//		// 끝났으면 완료 설정
	//		m_fadeDone = true;
	//		m_fadePercentage = 1.0f;
	//	}
	//}

	// 파티클 시스템에 대한 프레임 처리를 실행
	//m_ParticleSystem->Frame(frameTime, m_Direct3D->GetDeviceContext());

	return true;
}
bool GraphicsClass::Frame(XMFLOAT3& position)
{
	//m_Camera->SetPosition(position.x, position.y, position.z);

	return true;
}

bool GraphicsClass::Frame(XMFLOAT3& position, XMFLOAT3& rotation)
{
	//m_Camera->SetPosition(position.x, position.y, position.z);
	//m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);

	//static float lightPositionX = -5.0f;
	//lightPositionX += 0.05f;
	//if (lightPositionX > 5.0f)	lightPositionX = -5.0f;
	//m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);

	return true;
}

bool GraphicsClass::Render()
{
	//if (m_fadeDone) {
	//	// 백 버퍼의 장면 렌더링
	//	if (!RenderScene())	return false;
	//}
	//else {
	//	// 전체 장면을 먼저 텍스처로 렌더링
	//	if (!RenderToTexture()) return false;
	//	if (!RenderFadingScene()) return false;
	//}

	//if (!RenderRefractionToTexture()) return false;
	//if (!RenderReflectionToTexture()) return false;
	//if (!RenderWaterScene()) return false;

	/*
	// 블러 과정
	if (!RenderSceneToTexture()) return false;
	if (!DownSampleTexture()) return false;
	if (!RenderHorizontalBlurToTexture()) return false;
	if (!RenderVerticalBlurToTexture()) return false;
	if (!UpSampleTexture()) return false;
	if (!Render2DTextureScene()) return false;
	*/

	// 전체 장면을 먼저 텍스처로 렌더링
	//if (!RenderToTexture()) return false;
	// 백 버퍼의 장면 렌더링
	if (!RenderScene())	return false;

	return true;
}

/*
bool GraphicsClass::RenderToTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 RTT로 설정
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명 뷰 행렬 생성
	m_Light->GenerateViewMatrix();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_Model3D->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);
	m_Model3D->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix))	return false;

	
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix))	return false;

	
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix))	return false;

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}
*/


bool GraphicsClass::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;

	// 모델 회전용 코드
	static float rotation = 0.0f;

	// 각 프레임의 회전을 업데이트
	rotation += (float)XM_PI * 0.0025f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	worldMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_Model3D->Render(m_Direct3D->GetDeviceContext());
	if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture()))	return false;

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}

/*
bool GraphicsClass::RenderFadingScene()
{
	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 텍스쳐 셰이더를 이용하여 모델 렌더링
	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 0))	return false;
	if (!m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_fadePercentage))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}
*/

/*
bool GraphicsClass::RenderRefractionToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	if(!m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane))	return false;

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
bool GraphicsClass::RenderReflectionToTexture()
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->RenderReflection(m_waterHeight);
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);
	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
bool GraphicsClass::RenderWaterScene()
{
	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);
	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WallModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_ExampleWaterShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))	return false;

	reflectionMatrix = m_Camera->GetReflectionViewMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);
	m_WaterModel->Render(m_Direct3D->GetDeviceContext());
	if (!m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, 
		m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(),m_waterTranslation, 0.1f))	return false;

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}
*/

/*
bool GraphicsClass::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 렌더링 대상을 RTT로 설정
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 모델 회전용 코드
	static float rotation = 0.0f;

	// 각 프레임의 회전을 업데이트
	rotation += (float)XM_PI * 0.0025f;
	if (rotation > 360.0f)	rotation -= 360.0f;

	// 모델이 회전할 수 있도록 회전 값으로 세계 행렬 세팅
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_Model3D->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if(!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture()))	return false;

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_DownSampleTexture->GetOrthoMatrix(orthoMatrix);

	// 렌더링 대상을 RTT로 설정
	m_DownSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_DownSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if(!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView()))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderHorizontalBlurToTexture()
{
	// 수평 블러 셰이더에서 사용될 화면 폭 저장
	float screenSizeX = static_cast<float>(m_HorizontalBlurTexture->GetTextureWidth());

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 렌더링 대상을 RTT로 설정
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_DownSampleTexture->GetShaderResourceView(), screenSizeX))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderVerticalBlurToTexture()
{
	// 수직 블러 셰이더에서 사용될 화면 높이 저장
	float screenSizeY = static_cast<float>(m_VerticalBlurTexture->GetTextureHeight());

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 렌더링 대상을 RTT로 설정
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::UpSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_UpSampleTexture->GetOrthoMatrix(orthoMatrix);

	// 렌더링 대상을 RTT로 설정
	m_UpSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// RTT 초기화
	m_UpSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_VerticalBlurTexture->GetShaderResourceView()))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 다시 원래 백버퍼로 설정
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render2DTextureScene()
{
	// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Scene을 그리기 위해 버퍼 삭제
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// 카메라의 위치에 따라 뷰 행렬 생성
	m_Camera->Render();

	// 2D 렌더링을 위해 Z 버퍼 OFF
	m_Direct3D->TurnZBufferOff();

	// 텍스쳐 셰이더를 이용하여 모델 렌더링
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_UpSampleTexture->GetShaderResourceView()))	return false;

	// 2D 렌더링이 완료되었으면 Z 버퍼 ON
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}
*/

/* ---------------------------------------- 코드 정리--------------------------------------------

// 렌더링할 모델 세팅
//float positionX = 0;
//float positionY = 0;
//float positionZ = 0;
//float radius = 1.0f;
//XMFLOAT4 color;
//float fogColor = 0.5f;
//float fogStart = 0.0f;
//float fogEnd = 5.0f;
//float blendAmount = 0.5f;

// 클리핑 평면 생성
// 이 면은 Y축 값이 0 아래인 것들을 그리지 않게 한다
//XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);

// 텍스처 이동 위치 설정
//static float textureTranslation = 0.0f;
//textureTranslation += 0.005f;
//if (textureTranslation > 1.0f) textureTranslation -= 1.0f;

// Scene을 그리기 위해 버퍼 삭제
m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
//m_Direct3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

// 백 버퍼에 렌더링
//if (!RenderScene())	return false;

// 카메라의 위치에 따라 뷰 행렬 생성
m_Camera->Render();

// 카메라 및 Direct3D 객체에서 월드, 뷰, 투영 행렬을 가져온다
XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
m_Direct3D->GetWorldMatrix(worldMatrix);
m_Camera->GetViewMatrix(viewMatrix);
m_Direct3D->GetProjectionMatrix(projectionMatrix);
m_Direct3D->GetOrthoMatrix(orthoMatrix);

// 카메라가 반사행렬을 계산
m_Camera->RenderReflection(-1.5f);

// 일반 뷰 행렬 대신 반사 뷰 행렬을 가져온다
reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
m_Direct3D->GetWorldMatrix(worldMatrix);
m_Direct3D->GetProjectionMatrix(projectionMatrix);

// 모델 회전용 코드
static float rotation = 0.0f;

// 각 프레임의 회전을 업데이트
rotation += (float)XM_PI * 0.0025f;
if (rotation > 360.0f)	rotation -= 360.0f;

// 모델이 회전할 수 있도록 회전 값으로 세계 행렬 세팅
worldMatrix = XMMatrixRotationY(rotation);


// 매 프레임마다 시야 행렬에 근거하여 절두체를 생성
//m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

// 랜더링 될 모델의 수 세팅 및 초기화
//int modelCount = m_ModelList->GetModelCount();
//int renderCount = 0;

// 모든 모델을 탐색하고, 카메라 뷰에 보여지는 모델만 렌더링
//for (int i = 0; i < modelCount; i++) {
//	// 모델 위치와 색상 세팅
//	m_ModelList->GetData(i, positionX, positionY, positionZ, color);
//	
//	// 모델이 절두체 안에 있는지 확인
//	if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius)) {
//		// 모델을 렌더링 할 위치로 이동
//		worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
//		
//		// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
//		m_Model3D->Render(m_Direct3D->GetDeviceContext());

//		// 빛 셰이더를 사용하여 모델 렌더링
//		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(),
//			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

//		// 세계 행렬 리셋
//		m_Direct3D->GetWorldMatrix(worldMatrix);

//		renderCount++;
//	}
//}

//	static float frameTime = 0.0f;
//	frameTime += 0.01f;
//	if (frameTime > 1000.0f) frameTime = 0.0f;

	// 3개의 노이즈 텍스쳐의 스크롤 속도, 크기, 왜곡 값 설정
//	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);
//	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);
//	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
//	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
//	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

//	float distortionScale = 0.8f;
//	float distortionBias = 0.5f;

// 2D 렌더링을 위해 Z 버퍼 OFF
//m_Direct3D->TurnZBufferOff();

// 알파 블랜딩 on
//m_Direct3D->TurnOnAlphaBlending();

// 모델의 정점과 인덱스 버퍼를 그래픽 파이프라인에 묶어 렌더링을 준비
//m_Model->Render(m_Direct3D->GetDeviceContext());
//m_ModelTexture->Render(m_Direct3D->GetDeviceContext());
//m_Model3D->Render(m_Direct3D->GetDeviceContext());
//if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 400, 300))	return false;
//if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))	return false;
//if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

// 셰이더를 사용하여 모델 렌더링
//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))	return false;

// 텍스쳐 셰이더를 사용하여 모델 렌더링
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTexture->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelTexture->GetTexture()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTextureArray()))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView()))	return false;
//if (!m_FogShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), fogStart, fogEnd))	return false;
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), textureTranslation))	return false;
//if (!m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D2->GetTexture(), m_Model3D2->GetTexture2(), m_RenderTexture->GetShaderResourceView(), refractionScale))	return false;
//if (!m_TessellationShader->Render(m_Direct3D->GetDeviceContext(), m_ModelTessellation->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, tessellationAmount))	return false;

// 텍스쳐 셰이더와 반사 뷰 행렬을 사용하여 모델 렌더링
// 방향 조명
//if (!m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
//	m_GroundModel->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))	return false;

// 포인트 조명
if (!m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	m_Model3D->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))	return false;

// 빛 셰이더를 사용하여 모델 렌더링
//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_model3D->GetTexture(),
//	m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;
//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTexture(), diffuseColor, lightPosition))	return false;

// 텍스트 문자열 렌더링
//if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))	return false;
//if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))	return false;

// 모델 인스턴싱
//m_ModelInstance->Render(m_Direct3D->GetDeviceContext());
//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_ModelInstance->GetVertexCount(), m_ModelInstance->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_ModelInstance->GetTexture()))	return false;

// 투명도 적용한 오브젝트를 추가하기 위함
// x축으로 1만큼, z축으로 -1만큼 이동
// 그 후 알파블렌딩을 on 하고 2번째 모델을 렌더링
//worldMatrix = XMMatrixTranslation(0.5f, 0.5f, -0.1f);
//m_Direct3D->TurnOnAlphaBlending();
//m_Model3D2->Render(m_Direct3D->GetDeviceContext());
//if (!m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D2->GetTexture(), blendAmount))	return false;

// 빌보드의 카메라 위치와 모델 위치 설정
XMFLOAT3 cameraPosition, modelPosition;
cameraPosition = m_Camera->GetPosition();
modelPosition.x = 0.0f;
modelPosition.y = 1.5f;
modelPosition.z = 0.0f;

// 아크탄젠트 함수를 사용하여 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야 하는 각도를 계산
// 이렇게 하여 빌보드 모델이 현재 카메라 위치를 바라보게 한다.
double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (100.0f / (float)XM_PI);
float rotation = (float)angle * 0.0174532925f;

// 세계 행렬을 사용하여 원점에서 빌보드 회전을 설정
worldMatrix = XMMatrixRotationY(rotation);

// 빌보드 모델에서 이동 행렬을 설정
translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

// 회전, 이동 행렬을 곱하여 빌보드 모델의 최종 행렬 설정
worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

m_Billboard->Render(m_Direct3D->GetDeviceContext());
if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Billboard->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Billboard->GetTexture()))	return false;

// 알파 블랜딩 off
//m_Direct3D->TurnOffAlphaBlending();

// 2D 렌더링이 완료되었으면 Z 버퍼 ON
//m_Direct3D->TurnZBufferOn();

// 범프 맵 셰이더를 사용하여 모델 렌더링
//if (!m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model3D->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model3D->GetTextureArray(),
//	m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))	return false;

// 버퍼의 내용을 화면에 출력
//m_Direct3D->EndScene();


*/