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
//class LightClass;			// ���� ���� �� ���� �� ��ȸ
//class ExampleWaterShaderClass;	// ���� �ݻ�� ������ ǥ���ϱ� ���� ���� Ŭ����
//class BitmapClass;		// 2D ��
//class TextClass;			// �ؽ�Ʈ ��
//class InputClass;			// Direct Input ��
//class FrustumClass;			// ����
//class ModelListClass;		// �������� ���� ��
//class BumpMapShaderClass;	// ���� ����, �ݻ� ����
//class RenderTextureClass;	// ����� ��� �ؽ�ó�� ������ ����� ����
//class DebugWindowClass;		// �ؽ�ó�� ������ �ʴ� 2D �� (RTT ����)
//class FogShaderClass;		// �Ȱ�
//class TransparentShaderClass;		// ���� ����� Ŭ����
//class ReflectionShaderClass;	// �ݻ� ����� Ŭ����
//class FadeShaderClass;		// ���̵� ȿ�� ����� Ŭ����
//class RefractionShaderClass;	// ���� ����� Ŭ����
class FireShaderClass;		// ��

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
	bool Frame(float);
	bool Render();

private:
	bool RenderToTexture();
	bool RenderScene();

	// ���̵� ȿ�� ��
	bool RenderFadingScene();

	// �� ��
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();
	bool RenderWaterScene();

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
	FireShaderClass* m_FireShader = nullptr;

	

	/*
	// ���̵� ȿ���� ����ϴ� ����
	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	bool m_fadeDone = false;
	*/

	/*
	// �� ȿ���� ����ϴ� ����
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
};
