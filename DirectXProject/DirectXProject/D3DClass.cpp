#include "Stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass() {}
D3DClass::D3DClass(const D3DClass& other) {}
D3DClass:: ~D3DClass() {}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	// 수직동기화 상태 저장
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))	return false;

	// 팩토리 객체를 사용하여 첫번째 그래픽카드 인터페이스 어댑터 생성
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))	return false;

	// 출력(모니터)에 대한 첫번째 어댑터를 지정
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))	return false;

	// 출력(모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수 획득
	// DXGI_FORMAT_R8G8B8A8_UNORM는 텍스쳐 형식중 하나
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))	return false;

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성
	DXGI_MODE_DESC* displayModelList = new DXGI_MODE_DESC[numModes];
	if (!displayModelList) return false;

	// 디스플레이 모드에 대한 리스트 설정
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModelList)))	return false;

	// 모든 디스플레이 모드에 대해 화면 너비, 높이에 맞는 디스플레이 모드를 탐색
	// 적절한 모드를 찾으면 모니터의 새로고침 비율의 분모 분자 값을 저장
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

	// 비디오카드의 구조체 획득
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc))) return false;

	// 비디오카드 메모리 용량 단위를 MB 단위로 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오카드 이름 저장
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)	return false;

	// 디스플레이 모드 리스트 해제
	delete[] displayModelList;
	displayModelList = 0;

	// 출력 어댑터 해제
	adapterOutput->Release();
	adapterOutput = 0;

	// 어댑터 해제
	adapter->Release();
	adapter = 0;

	// 팩토리 객체 해제
	factory->Release();
	factory = 0;

	// SwapChain 구조체 초기화
	// 프론트버퍼와 백버퍼를 교환한다.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백버퍼를 1개만 사용하도록 지정
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 넓이와 높이 지정
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 32bit surface 설정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 새로고침 비율 설정
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 사용 용도 지정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링에 사용될 윈도우 핸들 지정
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링 off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 창모드, 풀스크린 모드 설정
	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서 및 크기를 지정하지 않음으로 설정
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 다음 백버퍼 비우기
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그 미사용
	swapChainDesc.Flags = 0;

	// 피처레벨을 DirectX11로 설정
	// 책 산건 12인데 11로 해도 무방하다고 해서 일단 11로 함(11이 더 자료도 많고 편하다고 그래서..)
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// SwapChain, Device, DeviceContext 생성
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))	return false;

	// 백버퍼 get
	ID3D11Texture2D* backBufferPtr = nullptr;
	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&(backBufferPtr)))))	return false;

	// 백버퍼 포인터로 RTV 생성
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView))) return false;

	// 백버퍼 포인터 해제
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 깊이 버퍼 구조체 초기화
	// 각 픽셀의 깊이 정보를 담는다 (0~1인데, 0는 가장 가까운 물체, 1은 가장 먼 물체)
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼 구조체 작성
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

	// 설정된 깊이 버퍼 구조체 사용하여 깊이 버퍼 텍스쳐 생성
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))	return false;

	// 스텐실 상태 구조체 초기화
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태 구조체 작성
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

	// 깊이 스텐실 상태 생성
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))	return false;

	// 깊이 스텐실 상태 설정
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// DSV 구조체 초기화
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// DSV 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// DSV 생성
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))	return false;

	// RTV와 DSV를 출력 렌더 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 그려지는 폴리곤과 방법을 결정할 래스터 구조체 설정
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;	// 테셀레이션을 위하여 삼각형의 와이어프레임을 그리도록 함
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 래스터 구조체에서 래스터라이저 상태를 생성
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))	return false;

	// 래스터라이저 상태 설정
	m_deviceContext->RSSetState(m_rasterState);

	// 후면 컬링을 해제하는 래스터라이저 구조체 설정 (하늘 돔에서 사용)
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;	// 이부분을 컬링을 해제하는 것으로 수정
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 래스터 구조체에서 잘리지 않는 래스터라이저 상태를 생성
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling)))	return false;

	// 랜더링을 위해 뷰포트 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// 뷰포트 생성
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// 투영행렬 설정
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위한 투영행렬
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 세계행렬을 항등핼렬로 초기화
	m_worldMatrix = XMMatrixIdentity();

	// 2D 렌더링을 위한 직교 투영행렬
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// 2D 렌더링을 위한 Z 버퍼를 비활성화 하는 2번째 깊이 스탠실 상태 구조체 생성
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// z버퍼 비활성화 스텐실 상태 구조체 작성
	// 상단에 작성한 스탠실 상태 구조체와 같지만 여기는 비활성화기 때문에 DepthEnable를 false로 변경
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

	// DSS 생성
	if (FAILED(m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))	return false;

	// 블렌딩 구조체 생성
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// 블렌딩 구조체 작성
	// src: 픽셀셰이더가 출력한 색상
	// dest: 후면 버퍼에 있는 픽셀 색상
	// 블렌딩을 적용하지 않으면 src가 dest를 덮어쓴다
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	// 파티클이 겹치면 서로의 색상을 더하면 additive 블렌딩 사용
	// color = (1 * source) + (1 * destination)
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// 블렌드 상태 생성
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState)))	return false;

	// 알파 비활성화 블렌드 상태를 생성하기 위해 기존 서술자를 수정
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState)))	return false;

	// 2차 알파블렌딩 상태 구조체 생성
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// 2차 알파블렌딩 상태 생성
	if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaBlendState2)))	return false;

	return true;
}

void D3DClass::Shutdown()
{
	// 종료 전 윈도우모드로 설정하지 않으면 SwapChain을 해제할 때 예외가 발생한다.
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
	// 버퍼를 지울 색을 설정
	float color[4] = { red, green, blue, alpha };

	// 백버퍼 삭제
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼 삭제
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// 렌더링이 완료되었으므로 화변에 백버퍼를 표시
	if (m_vsync_enabled) {
		// 화면 새로고침 비율을 고정
		m_swapChain->Present(1, 0);
	}
	else {
		// 가능한 빠르게 출력
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
	// RTV와 DSV 버퍼를 출력 렌더 파이프 라인데 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DClass::ResetViewport() { m_deviceContext->RSSetViewports(1, &m_viewport); }