#pragma once

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	// 2D 렌더링을 위해 Z버퍼 on/off
	void TurnZBufferOn();
	void TurnZBufferOff();

	// 블렌딩 설정
	// 블렌딩 on 상태면 font만 화면에 나타나고
	// 블렌딩 off 상태면 font 뒤에 모델(사각형)이 나타난다.
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	// 표면 컬링 설정
	void TurnOnCulling();
	void TurnOffCulling();

	// 보조 혼합 상태 설정
	void EnableSecondBlendState();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewport();

private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0 };
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11RasterizerState* m_rasterStateNoCulling = nullptr;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
	ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState* m_alphaDisableBlendingState = nullptr;
	ID3D11BlendState* m_alphaBlendState2 = nullptr;

	D3D11_VIEWPORT m_viewport;
};