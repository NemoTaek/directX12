#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;				
class CameraClass;
//class ModelClass;			// �Ϲ� ��
//class ColorShaderClass;	// �÷� ���̴�
//class ModelTextureClass;	// �ؽ��� ���� ��
//class TextureShaderClass;	// �ؽ��� ���̴�
class Model3DClass;		// 3D ��
//class ModelLightClass;	// ���� ���� ��
//class LightShaderClass;	// ���� ���̴�
class LightClass;			// ���� ���� �� ���� �� ��ȸ
//class BitmapClass;		// 2D ��
//class TextClass;			// �ؽ�Ʈ ��
//class InputClass;			// Direct Input ��
//class FrustumClass;			// ����
//class ModelListClass;		// �������� ���� ��
class BumpMapShaderClass;	// ���� ����

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
