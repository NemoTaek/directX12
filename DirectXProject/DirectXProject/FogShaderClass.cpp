#include "Stdafx.h"
#include "FogShaderClass.h"
#include <iostream>
#include <fstream>
using namespace std;

FogShaderClass::FogShaderClass() {}
FogShaderClass::FogShaderClass(const FogShaderClass& other) {}
FogShaderClass:: ~FogShaderClass() {}

bool FogShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//bool result;
	//WCHAR vs[] = L"VS.hlsl";
	//WCHAR ps[] = L"PS.hlsl";
	//result = InitializeShader(device, hwnd, vs, ps);
	//return result;
	return InitializeShader(device, hwnd, L"./Fog.vs", L"./Fog.ps");
}

void FogShaderClass::Shutdown()
{
	ShutdownShader();
}

// ������ �Ű������� ���� �ؽ��� �϶��� texture, ���� �ؽ��� �϶��� textureArray
bool FogShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fogStart, float fogEnd)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, fogStart, fogEnd))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FogShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// �������̴� �ڵ� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "FogVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// �ȼ����̴� �ڵ� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "FogPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
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

	// �������̴��� �ִ� ��� ��������� ����ü �ۼ�
	D3D11_BUFFER_DESC fogBufferDesc;
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	// �������̴� ������ۿ� ������ �� �ֵ��� ������� ����
	if (FAILED(device->CreateBuffer(&fogBufferDesc, NULL, &m_fogBuffer)))	return false;

	return true;
}

void FogShaderClass::ShutdownShader()
{
	// �Ȱ� ������� ����
	if (m_fogBuffer) {
		m_fogBuffer->Release();
		m_fogBuffer = 0;
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

void FogShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// �����޼��� ���â�� ǥ��
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޼����� ���̸� �����ɴϴ�.
	bufferSize = errorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޼����� ����մϴ�.
	fout.open("texture-error.txt");

	// ���� �޼����� ���ϴ�.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// ������ �ݽ��ϴ�.
	fout.close();

	errorMessage->Release();
	errorMessage = 0;
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool FogShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fogStart, float fogEnd)
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
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	// �ȼ����̴����� ���̴� �ؽ��� ���ҽ� ����
	// 2��° ���ڰ� ���ҽ� ���� ������ �ϸ� ������ �� textureArray ���� ���´�
	//deviceContext->PSSetShaderResources(0, 3, textureArray);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// ��� ���� �ڿ��� �ڷḦ �ø� �� �ֵ��� ������ ����
	if (FAILED(deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	FogBufferType* fogPtr = (FogBufferType*)mappedResource.pData;

	// ������ۿ� ��� ����
	fogPtr->fogStart = fogStart;
	fogPtr->fogEnd = fogEnd;

	// ������� �޸� ����
	deviceContext->Unmap(m_fogBuffer, 0);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);

	return true;
}

void FogShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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