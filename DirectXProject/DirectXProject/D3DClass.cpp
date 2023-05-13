#include "Stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass() {}
D3DClass::D3DClass(const D3DClass& other) {}
D3DClass:: ~D3DClass() {}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	// ��������ȭ ���� ����
	m_vsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮 ����
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))	return false;

	// ���丮 ��ü�� ����Ͽ� ù��° �׷���ī�� �������̽� ����� ����
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))	return false;

	// ���(�����)�� ���� ù��° ����͸� ����
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))	return false;

	// ���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� �� ȹ��
	// DXGI_FORMAT_R8G8B8A8_UNORM�� �ؽ��� ������ �ϳ�
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))	return false;

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� ����
	DXGI_MODE_DESC* displayModelList = new DXGI_MODE_DESC[numModes];
	if (!displayModelList) return false;

	// ���÷��� ��忡 ���� ����Ʈ ����
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModelList)))	return false;

	// ��� ���÷��� ��忡 ���� ȭ�� �ʺ�, ���̿� �´� ���÷��� ��带 Ž��
	// ������ ��带 ã���� ������� ���ΰ�ħ ������ �и� ���� ���� ����
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++) {
		if (displayModelList[i].Width == (unsigned int)screenWidth) {
			if (displayModelList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModelList[i].RefreshRate.Numerator;
				denominator = displayModelList[i].RefreshRate.Denominator;
			}
		}
	}

	// ����ī���� ����ü ȹ��
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc))) return false;

	// ����ī�� �޸� �뷮 ������ MB ������ ����
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ����ī�� �̸� ����
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)	return false;

	// ���÷��� ��� ����Ʈ ����
	delete[] displayModelList;
	displayModelList = 0;

	// ��� ����� ����
	adapterOutput->Release();
	adapterOutput = 0;

	// ����� ����
	adapter->Release();
	adapter = 0;

	// ���丮 ��ü ����
	factory->Release();
	factory = 0;

	// SwapChain ����ü �ʱ�ȭ
	// ����Ʈ���ۿ� ����۸� ��ȯ�Ѵ�.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ����۸� 1���� ����ϵ��� ����
	swapChainDesc.BufferCount = 1;

	// ������� ���̿� ���� ����
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 32bit surface ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ���� ����
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������� ��� �뵵 ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� ���� ������ �ڵ� ����
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø� off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// â���, Ǯ��ũ�� ��� ����
	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	// ��ĵ ���� ���� �� ũ�⸦ �������� �������� ����
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ���� ����� ����
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��� �̻��
	swapChainDesc.Flags = 0;

	// ��ó������ DirectX11�� ����
	// å ��� 12�ε� 11�� �ص� �����ϴٰ� �ؼ� �ϴ� 11�� ��(11�� �� �ڷᵵ ���� ���ϴٰ� �׷���..)
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// SwapChain, Device, DeviceContext ����
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))	return false;

	// ����� get
	ID3D11Texture2D* backBufferPtr = nullptr;
	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&(backBufferPtr)))))	return false;

	// ����� �����ͷ� RTV ����
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView))) return false;

	// ����� ������ ����
	backBufferPtr->Release();
	backBufferPtr = 0;

	// ���� ���� ����ü �ʱ�ȭ
	// �� �ȼ��� ���� ������ ��´� (0~1�ε�, 0�� ���� ����� ��ü, 1�� ���� �� ��ü)
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ���� ����ü �ۼ�
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ������ ���� ���� ����ü ����Ͽ� ���� ���� �ؽ��� ����
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))	return false;

	// ���ٽ� ���� ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ���� ����ü �ۼ�
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���� ����
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))	return false;

	// ���� ���ٽ� ���� ����
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// DSV ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// DSV ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// DSV ����
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))	return false;

	// RTV�� DSV�� ��� ���� ���������ο� ���ε�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// �׷����� ������� ����� ������ ������ ����ü ����
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;	// �׼����̼��� ���Ͽ� �ﰢ���� ���̾��������� �׸����� ��
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ������ ����ü���� �����Ͷ����� ���¸� ����
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))	return false;

	// �����Ͷ����� ���� ����
	m_deviceContext->RSSetState(m_rasterState);

	// �ĸ� �ø��� �����ϴ� �����Ͷ����� ����ü ���� (�ϴ� ������ ���)
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;	// �̺κ��� �ø��� �����ϴ� ������ ����
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ������ ����ü���� �߸��� �ʴ� �����Ͷ����� ���¸� ����
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling)))	return false;

	// �������� ���� ����Ʈ ����
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// ����Ʈ ����
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// ������� ����
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D �������� ���� �������
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// ��������� �׵��۷ķ� �ʱ�ȭ
	m_worldMatrix = XMMatrixIdentity();

	// 2D �������� ���� ���� �������
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// 2D �������� ���� Z ���۸� ��Ȱ��ȭ �ϴ� 2��° ���� ���Ľ� ���� ����ü ����
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// z���� ��Ȱ��ȭ ���ٽ� ���� ����ü �ۼ�
	// ��ܿ� �ۼ��� ���Ľ� ���� ����ü�� ������ ����� ��Ȱ��ȭ�� ������ DepthEnable�� false�� ����
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// DSS ����
	if (FAILED(m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))	return false;

	// ���� ����ü ����
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// ���� ����ü �ۼ�
	// src: �ȼ����̴��� ����� ����
	// dest: �ĸ� ���ۿ� �ִ� �ȼ� ����
	// ������ �������� ������ src�� dest�� �����
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	// ��ƼŬ�� ��ġ�� ������ ������ ���ϸ� additive ���� ���
	// color = (1 * source) + (1 * destination)
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// ���� ���� ����
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState)))	return false;

	// ���� ��Ȱ��ȭ ���� ���¸� �����ϱ� ���� ���� �����ڸ� ����
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState)))	return false;

	// 2�� ���ĺ��� ���� ����ü ����
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// 2�� ���ĺ��� ���� ����
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaBlendState2)))	return false;

	return true;
}

void D3DClass::Shutdown()
{
	// ���� �� ��������� �������� ������ SwapChain�� ������ �� ���ܰ� �߻��Ѵ�.
	if (m_swapChain) {
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_alphaBlendState2) {
		m_alphaBlendState2->Release();
		m_alphaBlendState2 = 0;
	}

	if (m_alphaEnableBlendingState) {
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_alphaDisableBlendingState) {
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_depthDisabledStencilState) {
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterState) {
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState) {
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext) {
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device) {
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain) {
		m_swapChain->Release();
		m_swapChain = 0;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	// ���۸� ���� ���� ����
	float color[4] = { red, green, blue, alpha };

	// ����� ����
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���� ����
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// �������� �Ϸ�Ǿ����Ƿ� ȭ���� ����۸� ǥ��
	if (m_vsync_enabled) {
		// ȭ�� ���ΰ�ħ ������ ����
		m_swapChain->Present(1, 0);
	}
	else {
		// ������ ������ ���
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOnCulling()
{
	m_deviceContext->RSSetState(m_rasterState);
}

void D3DClass::TurnOffCulling()
{
	m_deviceContext->RSSetState(m_rasterStateNoCulling);
}

void D3DClass::EnableSecondBlendState()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaBlendState2, blendFactor, 0xffffffff);
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView() { return m_depthStencilView; }

void D3DClass::SetBackBufferRenderTarget()
{
	// RTV�� DSV ���۸� ��� ���� ������ ���ε� ���ε�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DClass::ResetViewport() { m_deviceContext->RSSetViewports(1, &m_viewport); }