#include "Stdafx.h"
#include "LightShaderClass.h"
#include <iostream>
#include <fstream>
using namespace std;

LightShaderClass::LightShaderClass() {}
LightShaderClass::LightShaderClass(const LightShaderClass& other) {}
LightShaderClass:: ~LightShaderClass() {}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//bool result;
	//WCHAR vs[] = L"VS.hlsl";
	//WCHAR ps[] = L"PS.hlsl";
	//result = InitializeShader(device, hwnd, vs, ps);
	//return result;
	return InitializeShader(device, hwnd, L"./VertexShader.vs", L"./PixelShader.ps");
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// �������̴� �ڵ� ������
	// D3DCompileFromFile �� 4��° �Ű�����(pEntryPoint)�� �ش� ���̴� ������ �������� ���ƾ� �����. �ٸ��� �׳� ������ �����������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// �ȼ����̴� �ڵ� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// ���̾ƿ� ��� ��
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ�
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))	return false;

	// ���̻� ������ �ʴ� �������̴� ���ۿ� �ȼ����̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// �������̴��� �ִ� ��� ��������� ������ �ۼ�
	// ������۴� ���̴� ���α׷����� �����ϴ� �ڷḦ ��� GPU �ڿ��̴�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// �������̴� ������ۿ� ������ �� �ֵ��� ������� ����
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))	return false;

	// �ؽ��� ���÷� ���� ������ ����
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

	// �������̴��� �ִ� ī�޶� ���� ��� ���� ������ ����
	//D3D11_BUFFER_DESC cameraBufferDesc;
	//cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	//cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cameraBufferDesc.MiscFlags = 0;
	//cameraBufferDesc.StructureByteStride = 0;

	// �������̴� ������ۿ� ������ �� �ֵ��� ������� ����
	//if (FAILED(device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer)))	return false;

	// �ȼ����̴��� �ִ� ���� ���� ��� ���� ������ ����
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// �������̴� ������ۿ� ������ �� �ֵ��� ������� ����
	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer)))	return false;

	return true;
}

void LightShaderClass::ShutdownShader()
{
	// ���� ������� ����
	if (m_lightBuffer) {
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	// ī�޶� ������� ����
	//if (m_cameraBuffer) {
	//	m_cameraBuffer->Release();
	//	m_cameraBuffer = 0;
	//}

	// ���÷� ���� ����
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// ��� ������� ����
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
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

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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
	//fout.open("light-error.txt");

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

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	// �ȼ����̴����� ���̴� �ؽ��� ���ҽ� ����
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// ���̴����� ����� �� �ֵ��� ��ġ���ȭ
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ���� �ڿ��� �ڷḦ �ø� �� �ֵ��� ������ ����
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* matrixDataPtr;
	CameraBufferType* cameraDataPtr;
	LightBufferType* lightDataPtr;

	// ������� ����
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	matrixDataPtr = (MatrixBufferType*)mappedResource.pData;

	// ������ۿ� ��� ����
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	// ������� �޸� ����
	deviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//if (FAILED(deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	//cameraDataPtr = (CameraBufferType*)mappedResource.pData;

	// ������ۿ� ��� ����
	//cameraDataPtr->cameraPosition = cameraPosition;
	//cameraDataPtr->padding = 0.0f;

	//deviceContext->Unmap(m_cameraBuffer, 0);
	//bufferNumber = 1;	// ī�޶� ���۴� �������̴����� 2��° �����̱� ������ 1�� ����
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	if (FAILED(deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	lightDataPtr = (LightBufferType*)mappedResource.pData;

	//lightDataPtr->ambientColor = ambientColor;
	lightDataPtr->diffuseColor = diffuseColor;
	lightDataPtr->lightDirection = lightDirection;
	//lightDataPtr->specularColor = specularColor;
	//lightDataPtr->specularPower = specularPower;

	deviceContext->Unmap(m_lightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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