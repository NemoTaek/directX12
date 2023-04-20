#include "Stdafx.h"
#include "FadeShaderClass.h"
#include <iostream>
#include <fstream>
using namespace std;

FadeShaderClass::FadeShaderClass() {}
FadeShaderClass::FadeShaderClass(const FadeShaderClass& other) {}
FadeShaderClass:: ~FadeShaderClass() {}

bool FadeShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//bool result;
	//WCHAR vs[] = L"VS.hlsl";
	//WCHAR ps[] = L"PS.hlsl";
	//result = InitializeShader(device, hwnd, vs, ps);
	//return result;
	return InitializeShader(device, hwnd, L"./fade.vs", L"./fade.ps");
}

void FadeShaderClass::Shutdown()
{
	ShutdownShader();
}

// ������ �Ű������� ���� �ؽ��� �϶��� texture, ���� �ؽ��� �϶��� textureArray
bool FadeShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fadeAmount)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, fadeAmount))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FadeShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// �������̴� �ڵ� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "FadeVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// �ȼ����̴� �ڵ� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "FadePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
		else { MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK); }
		return false;
	}

	// ���۷κ��� �������̴� ����
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))	return false;

	// ���۷κ��� �ȼ����̴� ����
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))	return false;

	// ���� �Է� ���̾ƿ� ����ü ����
	// ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ��Ѵ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ� ��� ��
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ�
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))	return false;

	// ���̻� ������ �ʴ� �������̴� ���ۿ� �ȼ����̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// �������̴��� �ִ� ��� ��������� ����ü �ۼ�
	// ������۴� ���̴� ���α׷����� �����ϴ� �ڷḦ ��� GPU �ڿ��̴�.
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// �������̴� ������ۿ� ������ �� �ֵ��� ������� ����
	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer)))	return false;

	// �ؽ��� ���÷� ���� ����ü ����
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ��� ���÷� ���� ����
	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))	return false;

	// �ؽ�ó �̵� ���� ����ü ����
	D3D11_BUFFER_DESC fadeBufferDesc;
	fadeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fadeBufferDesc.ByteWidth = sizeof(FadeBufferType);
	fadeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fadeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fadeBufferDesc.MiscFlags = 0;
	fadeBufferDesc.StructureByteStride = 0;

	// �ؽ�ó �̵� ���� ����
	if (FAILED(device->CreateBuffer(&fadeBufferDesc, NULL, &m_fadeBuffer)))	return false;

	return true;
}

void FadeShaderClass::ShutdownShader()
{
	// �ؽ�ó �̵� ���� ����
	if (m_fadeBuffer) {
		m_fadeBuffer->Release();
		m_fadeBuffer = 0;
	}

	// ���÷� ���� ����
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// ��� ������� ����
	if (m_constantBuffer) {
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	// ���̾ƿ� ����
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ����̴� ����
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// �������̴� ����
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void FadeShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// �����޼��� ���â�� ǥ��
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	//char* compileErrors;
	//unsigned long bufferSize, i;
	//std::ofstream fout;


	//// ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�.
	//compileErrors = (char*)(errorMessage->GetBufferPointer());

	//// �޼����� ���̸� �����ɴϴ�.
	//bufferSize = errorMessage->GetBufferSize();

	//// ������ ���� �ȿ� �޼����� ����մϴ�.
	//fout.open("texture-error.txt");

	//// ���� �޼����� ���ϴ�.
	//for (i = 0; i < bufferSize; i++)
	//{
	//	fout << compileErrors[i];
	//}

	//// ������ �ݽ��ϴ�.
	//fout.close();

	errorMessage->Release();
	errorMessage = 0;
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool FadeShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fadeAmount)
{
	// ���̴����� ����� �� �ֵ��� ��ġ���ȭ
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ���� �ڿ��� �ڷḦ �ø� �� �ֵ��� ������ ����
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	ConstantBufferType* matrixPtr = (ConstantBufferType*)mappedResource.pData;

	// ������ۿ� ��� ����
	matrixPtr->world = worldMatrix;
	matrixPtr->view = viewMatrix;
	matrixPtr->projection = projectionMatrix;

	// ������� �޸� ����
	deviceContext->Unmap(m_constantBuffer, 0);
	unsigned bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	// �ȼ����̴����� ���̴� �ؽ��� ���ҽ� ����
	// 2��° ���ڰ� ���ҽ� ���� ������ �ϸ� ������ �� textureArray ���� ���´�
	//deviceContext->PSSetShaderResources(0, 3, textureArray);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	if (FAILED(deviceContext->Map(m_fadeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	FadeBufferType* fadePtr = (FadeBufferType*)mappedResource.pData;

	// ������ۿ� Ŭ���� ��� ����
	fadePtr->fadeAmount = fadeAmount;
	fadePtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// ������� �޸� ����
	deviceContext->Unmap(m_fadeBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_fadeBuffer);

	return true;
}

void FadeShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� �������̴��� �ȼ����̴� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ����̴����� ���÷� ���� ����
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸���
	deviceContext->DrawIndexed(indexCount, 0, 0);
}