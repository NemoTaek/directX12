#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class InputClass;
class TimerClass;
class PositionClass;
class FpsClass;
class CpuClass;
class TextClass;
class FontShaderClass;
//class ColorShaderClass;
class TerrainClass;
class TerrainShaderClass;
class LightClass;
class TextureClass;

//class FrustumClass;		// 절두체
//class QuadTreeClass;		// 쿼드 트리(현재 시야에 보이는 부분만 렌더링)
//class TextureShaderClass;

//class MiniMapClass;		// 미니맵

class SkyDomeClass;			// 하늘
class SkyDomeShaderClass;	// 하늘
class SkyPlaneClass;		// 하늘 평면(여기서는 구름)
class SkyPlaneShaderClass;	// 하늘 평면(여기서는 구름)

//class DebugWindowClass;
//class RenderTextureClass;
//class DepthShaderClass;

//class ReflectionShaderClass;
//class WaterClass;
//class WaterShaderClass;



class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);
	bool RenderGraphics();
	bool RenderSceneToTexture();
	void RenderRefractionToTexture();
	void RenderReflectionToTexture();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	InputClass* m_Input = nullptr;
	TimerClass* m_Timer = nullptr;
	PositionClass* m_Position = nullptr;
	FpsClass* m_Fps = nullptr;
	CpuClass* m_Cpu = nullptr;
	TextClass* m_Text = nullptr;
	FontShaderClass* m_FontShader = nullptr;
	//ColorShaderClass* m_ColorShader = nullptr;
	TerrainClass* m_Terrain = nullptr;
	TerrainShaderClass* m_TerrainShader = nullptr;
	LightClass* m_Light = nullptr;

	//FrustumClass* m_Frustum = nullptr;
	//QuadTreeClass* m_QuadTree = nullptr;
	//TextureShaderClass* m_TextureShader = nullptr;
	
	//MiniMapClass* m_MiniMap = nullptr;

	SkyDomeClass* m_SkyDome = nullptr;
	SkyDomeShaderClass* m_SkyDomeShader = nullptr;
	SkyPlaneClass* m_SkyPlane = nullptr;
	SkyPlaneShaderClass* m_SkyPlaneShader = nullptr;

	//DebugWindowClass* m_DebugWindow = nullptr;
	//RenderTextureClass* m_RenderTexture = nullptr;
	//DepthShaderClass* m_DepthShader = nullptr;

	//RenderTextureClass* m_RefractionTexture = nullptr;
	//RenderTextureClass* m_ReflectionTexture = nullptr;
	//ReflectionShaderClass* m_ReflectionShader = nullptr;
	//WaterClass* m_Water = nullptr;
	//WaterShaderClass* m_WaterShader = nullptr;

	TextureClass* m_ColorTexture1 = nullptr;
	TextureClass* m_ColorTexture2 = nullptr;
	TextureClass* m_ColorTexture3 = nullptr;
	TextureClass* m_ColorTexture4 = nullptr;
	TextureClass* m_AlphaTexture = nullptr;
	TextureClass* m_NormalTexture1 = nullptr;
	TextureClass* m_NormalTexture2 = nullptr;
};