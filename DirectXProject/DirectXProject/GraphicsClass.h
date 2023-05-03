#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

class D3DClass;				
class CameraClass;
//class ViewPointClass;	// 텍스처를 투영하는 관점에서 장면을 볼 시점 포인트 클래스
//class TextureClass;
//class TextureShaderClass;	// 텍스쳐 셰이더
//class Model3DClass;		// 3D 모델
//class LightShaderClass;	// 조명 셰이더
//class LightClass;			// 조명 관련 값 설정 및 조회
//class ExampleWaterShaderClass;	// 물의 반사와 굴절을 표현하기 위한 예시 클래스
//class BitmapClass;		// 2D 모델
//class TextClass;			// 텍스트 모델
//class InputClass;			// Direct Input 모델
//class FrustumClass;			// 절단
//class ModelListClass;		// 여러개의 랜덤 모델
//class BumpMapShaderClass;	// 범프 매핑, 반사 매핑
//class RenderTextureClass;	// 백버퍼 대신 텍스처로 렌더링 대상을 설정
//class DebugWindowClass;		// 텍스처를 가지지 않는 2D 모델 (RTT 목적)
//class FogShaderClass;		// 안개
//class TransparentShaderClass;		// 투명도 적용된 클래스
//class ReflectionShaderClass;	// 반사 적용된 클래스
//class FadeShaderClass;		// 페이드 효과 적용된 클래스
//class RefractionShaderClass;	// 굴절 적용된 클래스
//class FireShaderClass;		// 불
//class DepthShaderClass;		// 깊이 버퍼 셰이더 클래스
//class HorizontalBlurShaderClass;	// 수평 블러 클래스
//class VerticalBlurShaderClass;	// 수직 블러 클래스
//class OrthoWindowClass;		// 3D 공간의 사각형을 2D 화면으로 투영하는 클래스(다운샘플링)
//class ModelInstanceClass;	// 인스턴스화 된 모델 클래스
//class TessellationShaderClass;	// 하드웨어에 테셀레이션을 하도록 하는 클래스
//class ModelTessellationClass;	// 테셀레이션을 위한 모델 클래스
//class ParticleSystemClass;		// 파티클 클래스
//class ProjectionShaderClass;	// 투영 클래스
//class ShaderManagerClass;	// 셰이더 관리 클래스

// 이 프로젝트에서 사용되는 모든 그래픽 객체에 대한 호출을 담당하는 클래스
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Frame(float);
	bool Frame(XMFLOAT3&);
	bool Frame(XMFLOAT3&, XMFLOAT3&);
	bool Render();

private:
	bool RenderToTexture();
	bool RenderScene();

	// 페이드 효과 씬
	bool RenderFadingScene();

	// 물 씬
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();
	bool RenderWaterScene();

	// 블러 처리를 위한 함수
	// 장면을 텍스처로 렌더링 -> 텍스처 크기를 절반 이하로 다운샘플링 -> 수평 및 수직 블러 수행 -> 원래 크기로 업샘플링 -> 블러처리 된 텍스처를 화면에 렌더링
	bool RenderSceneToTexture();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	//ViewPointClass* m_ViewPoint = nullptr;
	//ModelClass* m_Model = nullptr;
	//ModelTextureClass* m_ModelTexture = nullptr;
	//TextureClass* m_ProjectionTexture = nullptr;
	//TextureShaderClass* m_TextureShader = nullptr;
	//Model3DClass* m_Model3D = nullptr;
	//ModelLightClass* m_ModelLight = nullptr;
	//LightShaderClass* m_LightShader = nullptr;
	//LightClass* m_Light = nullptr;
	//LightClass* m_Light1 = nullptr;
	//LightClass* m_Light2 = nullptr;
	//LightClass* m_Light3 = nullptr;
	//LightClass* m_Light4 = nullptr;
	//LightClass* m_Light5 = nullptr;
	//BitmapClass* m_Bitmap = nullptr;
	//TextClass* m_Text = nullptr;
	//InputClass* m_Input = nullptr;
	//FrustumClass* m_Frustum = nullptr;
	//ModelListClass* m_ModelList = nullptr;
	//BumpMapShaderClass* m_BumpMapShader = nullptr;
	//RenderTextureClass* m_RenderTexture = nullptr;
	//DebugWindowClass* m_DebugWindow = nullptr;
	//FogShaderClass* m_FogShader = nullptr;
	//ReflectionShaderClass* m_ReflectionShader = nullptr;
	//FadeShaderClass* m_FadeShader = nullptr;
	//RefractionShaderClass* m_RefractionShader = nullptr;
	//FireShaderClass* m_FireShader = nullptr;
	//Model3DClass* m_Billboard = nullptr;
	//DepthShaderClass* m_DepthShader = nullptr;
	//ModelInstanceClass* m_ModelInstance = nullptr;
	//ModelTessellationClass* m_ModelTessellation = nullptr;
	//TessellationShaderClass* m_TessellationShader = nullptr;
	//ParticleSystemClass* m_ParticleSystem = nullptr;
	//Model3DClass* m_GroundModel = nullptr;
	//ProjectionShaderClass* m_ProjectionShader = nullptr;
	//ShaderManagerClass* m_ShaderManager = nullptr;

	/*
	// 페이드 효과에 사용하는 변수
	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	bool m_fadeDone = false;
	*/

	/*
	// 물 효과에 사용하는 변수
	Model3DClass* m_GroundModel = nullptr;
	Model3DClass* m_WallModel = nullptr;
	Model3DClass* m_BathModel = nullptr;
	Model3DClass* m_WaterModel = nullptr;
	ExampleWaterShaderClass* m_ExampleWaterShader = nullptr;
	RenderTextureClass* m_ReflectionTexture = nullptr;
	RenderTextureClass* m_RefractionTexture = nullptr;
	ReflectionShaderClass* m_WaterShader = nullptr;
	RefractionShaderClass* m_RefractionShader = nullptr;
	float m_waterHeight = 0;
	float m_waterTranslation = 0;
	*/

	/*
	// 블러 효과에 사용하는 변수
	HorizontalBlurShaderClass* m_HorizontalBlurShader = nullptr;
	VerticalBlurShaderClass* m_VerticalBlurShader = nullptr;
	RenderTextureClass* m_DownSampleTexture = nullptr;
	RenderTextureClass* m_HorizontalBlurTexture = nullptr;
	RenderTextureClass* m_VerticalBlurTexture = nullptr;
	RenderTextureClass* m_UpSampleTexture = nullptr;
	OrthoWindowClass* m_SmallWindow = nullptr;
	OrthoWindowClass* m_FullScreenWindow = nullptr;
	*/
};
