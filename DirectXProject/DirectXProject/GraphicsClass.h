#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
//class ModelClass;
//class ColorShaderClass;
//class ModelTextureClass;
//class TextureShaderClass;
class Model3DClass;
class ModelLightClass;
class LightShaderClass;
class LightClass;

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

private:
	bool Render(float);

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	//ModelClass* m_Model = nullptr;
	//ColorShaderClass* m_ColorShader = nullptr;
	//ModelTextureClass* m_ModelTexture = nullptr;
	//TextureShaderClass* m_TextureShader = nullptr;
	Model3DClass* m_model3D = nullptr;
	ModelLightClass* m_ModelLight = nullptr;
	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;
};
