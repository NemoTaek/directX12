#include "Stdafx.h"
#include "RenderTextureClass.h"

RenderTextureClass::RenderTextureClass() {}
RenderTextureClass::RenderTextureClass(const RenderTextureClass& other) {}
RenderTextureClass::~RenderTextureClass() {}

bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	// 렌더 타겟 텍스처 구조체 초기화 및 설정
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture)))	return false;

	// 렌더 타겟 뷰 구조체 설정
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView)))	return false;

	// 셰이더 리소스 뷰 구조체 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView)))	return false;

	return true;
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	// 렌더 텍스처의 크기 저장
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	// 렌더 타겟 텍스처 구조체 초기화 및 설정
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture)))	return false;

	// 렌더 타겟 뷰 구조체 설정
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView)))	return false;

	// 셰이더 리소스 뷰 구조체 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView)))	return false;

	// 깊이버퍼 구조체 초기화 및 설정
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))	return false;

	// 스텐실 뷰 구조체 초기화 및 설정
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))	return false;

	// 렌더링을 위한 뷰포트 설정
	m_viewport.Width = static_cast<float>(textureWidth);
	m_viewport.Height = static_cast<float>(textureHeight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), (static_cast<float>(textureWidth) / static_cast<float>(textureHeight)), screenNear, screenDepth);
	m_orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(textureWidth), static_cast<float>(textureHeight), screenNear, screenDepth);

	return true;
}

void RenderTextureClass::Shutdown()
{
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// RTV와 DSV를 출력 렌더 파이프라인에 바인딩
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}

void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	// 버퍼를 초기화할 색 설정
	float color[4] = { red, green, blue, alpha };

	// 백 버퍼 초기화
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼 초기화
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// RTV와 DSV를 출력 렌더 파이프라인에 바인딩
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 뷰포트 세팅
	deviceContext->RSSetViewports(1, &m_viewport);
}


void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	// 버퍼를 초기화할 색 설정
	float color[4] = { red, green, blue, alpha };

	// 백 버퍼 초기화
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼 초기화
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView() { return m_shaderResourceView; }

void RenderTextureClass::GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; }

void RenderTextureClass::GetOrthoMatrix(XMMATRIX& orthoMatrix) { orthoMatrix = m_orthoMatrix; }

int RenderTextureClass::GetTextureWidth() { return m_textureWidth; }

int RenderTextureClass::GetTextureHeight() { return m_textureHeight; }