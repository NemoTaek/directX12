#include "Stdafx.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
//#include "BumpMapShaderClass.h"
#include "ShaderManagerClass.h"

ShaderManagerClass::ShaderManagerClass() {}
ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other) {}
ShaderManagerClass:: ~ShaderManagerClass() {}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	m_textureShader = new TextureShaderClass;
	if (!m_textureShader) { return false; }
	// 텍스쳐 셰이더 객체 초기화
	if (!m_textureShader->Initialize(device, hwnd)) {
		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
		return false;
	}

	m_lightShader = new LightShaderClass;
	if (!m_lightShader) { return false; }
	if (!m_lightShader->Initialize(device, hwnd)) {
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}

	//m_bumpMapShader = new BumpMapShaderClass;
	//if (!m_bumpMapShader) { return false; }
	//if (!m_bumpMapShader->Initialize(device, hwnd)) {
	//	MessageBox(hwnd, L"Could not initialize the bump map shader object", L"Error", MB_OK);
	//	return false;
	//}
}

void ShaderManagerClass::Shutdown()
{
	//if (m_bumpMapShader) {
	//	m_bumpMapShader->Shutdown();
	//	delete m_bumpMapShader;
	//	m_bumpMapShader = 0;
	//}

	if (m_lightShader) {
		m_lightShader->Shutdown();
		delete m_lightShader;
		m_lightShader = 0;
	}

	if (m_textureShader) {
		m_textureShader->Shutdown();
		delete m_textureShader;
		m_textureShader = 0;
	}
}

bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return m_textureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor)
{
	return m_lightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture, lightPosition, ambientColor, diffuseColor);
}

//bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray,
//	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
//{
//	return m_bumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, textureArray, lightDirection, diffuseColor, cameraPosition, specularColor, specularPower);
//}