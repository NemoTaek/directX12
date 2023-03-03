#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
//class ModelClass;
class ModelTextureClass;
//class ColorShaderClass;
class TextureShaderClass;

// �� ������Ʈ���� ���Ǵ� ��� �׷��� ��ü�� ���� ȣ���� ����ϴ� Ŭ����
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
	bool Render();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	//ModelClass* m_Model = nullptr;
	ModelTextureClass* m_ModelTexture = nullptr;
	//ColorShaderClass* m_ColorShader = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
};
