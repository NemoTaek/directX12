#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;				
class CameraClass;
//class ModelClass;			// 일반 모델
//class ColorShaderClass;	// 컬러 셰이더
//class ModelTextureClass;	// 텍스쳐 입힌 모델
//class TextureShaderClass;	// 텍스쳐 셰이더
class Model3DClass;		// 3D 모델
//class ModelLightClass;	// 조명 입힌 모델
//class LightShaderClass;	// 조명 셰이더
class LightClass;			// 조명 관련 값 설정 및 조회
//class BitmapClass;		// 2D 모델
//class TextClass;			// 텍스트 모델
//class InputClass;			// Direct Input 모델
//class FrustumClass;			// 절단
//class ModelListClass;		// 여러개의 랜덤 모델
class BumpMapShaderClass;	// 범프 매핑

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
	bool Render();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	//ModelClass* m_Model = nullptr;
	//ColorShaderClass* m_ColorShader = nullptr;
	//ModelTextureClass* m_ModelTexture = nullptr;
	//TextureShaderClass* m_TextureShader = nullptr;
	Model3DClass* m_Model3D = nullptr;
	//ModelLightClass* m_ModelLight = nullptr;
	//LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;
	//BitmapClass* m_Bitmap = nullptr;
	//TextClass* m_Text = nullptr;
	//InputClass* m_Input = nullptr;
	//FrustumClass* m_Frustum = nullptr;
	//ModelListClass* m_ModelList = nullptr;
	BumpMapShaderClass* m_BumpMapShader = nullptr;
};
