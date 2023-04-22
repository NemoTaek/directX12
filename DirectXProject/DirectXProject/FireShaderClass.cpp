#include "Stdafx.h"
#include "FireShaderClass.h"
#include <iostream>
#include <fstream>
using namespace std;

FireShaderClass::FireShaderClass() {}
FireShaderClass::FireShaderClass(const FireShaderClass& other) {}
FireShaderClass:: ~FireShaderClass() {}

bool FireShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//bool result;
	//WCHAR vs[] = L"VS.hlsl";
	//WCHAR ps[] = L"PS.hlsl";
	//result = InitializeShader(device, hwnd, vs, ps);
	//return result;
	return InitializeShader(device, hwnd, L"./Fire.vs", L"./Fire.ps");
}

void FireShaderClass::Shutdown()
{
	ShutdownShader();
}

// ������ �Ű������� ���� �ؽ��� �϶��� texture, ���� �ؽ��� �϶��� textureArray
bool FireShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* fireTexture, ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture, float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales,
	XMFLOAT2 distortion1, XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale, float distortionBias)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture, alphaTexture, frameTime, scrollSpeeds, scales,
		distortion1, distortion2, distortion3, distortionScale, distortionBias))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FireShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// �������̴� �ڵ� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "FireVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// �ȼ����̴� �ڵ� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "FirePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
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

	// �Ҳ� ȿ���� ����� ���� WRAP ��� CLAMP�� ����Ͽ� �ٴ����� �������� �ʰ� ��
	D3D11_SAMPLER_DESC samplerDesc2;
	samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.MipLODBias = 0.0f;
	samplerDesc2.MaxAnisotropy = 1;
	samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc2.BorderColor[0] = 0;
	samplerDesc2.BorderColor[1] = 0;
	samplerDesc2.BorderColor[2] = 0;
	samplerDesc2.BorderColor[3] = 0;
	samplerDesc2.MinLOD = 0;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ��� ���÷� ���� ����
	if (FAILED(device->CreateSamplerState(&samplerDesc2, &m_sampleState2)))	return false;

	// ������ ���� ����ü ����
	D3D11_BUFFER_DESC noiseBufferDesc;
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	// ������ ���� ����
	if (FAILED(device->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer)))	return false;

	// �ְ� ���� ����ü ����
	D3D11_BUFFER_DESC distortionBufferDesc;
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	// �ְ� ���� ����
	if (FAILED(device->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer)))	return false;

	return true;
}

void FireShaderClass::ShutdownShader()
{
	// �ְ� ���� ����
	if (m_distortionBuffer) {
		m_distortionBuffer->Release();
		m_distortionBuffer = 0;
	}

	// ������ ���� ����
	if (m_noiseBuffer) {
		m_noiseBuffer->Release();
		m_noiseBuffer = 0;
	}

	// ���÷� ���� ����
	if (m_sampleState2) {
		m_sampleState2->Release();
		m_sampleState2 = 0;
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

void FireShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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

bool FireShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* fireTexture, ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture, float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales,
	XMFLOAT2 distortion1, XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale, float distortionBias)
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
	// 2��° ���ڰ� ���ҽ� ���� ���� �ϸ� ���� �� textureArray ���� ���´�
	//deviceContext->PSSetShaderResources(0, 3, textureArray);
	deviceContext->PSSetShaderResources(0, 1, &fireTexture);
	deviceContext->PSSetShaderResources(1, 1, &noiseTexture);
	deviceContext->PSSetShaderResources(2, 1, &alphaTexture);

	if (FAILED(deviceContext->Map(m_noiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	NoiseBufferType* noisePtr = (NoiseBufferType*)mappedResource.pData;

	noisePtr->frameTime = frameTime;
	noisePtr->scrollSpeeds = scrollSpeeds;
	noisePtr->scales = scales;
	noisePtr->padding = 0.0f;

	deviceContext->Unmap(m_noiseBuffer, 0);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_noiseBuffer);

	if (FAILED(deviceContext->Map(m_distortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	DistortionBufferType* distortionPtr = (DistortionBufferType*)mappedResource.pData;

	distortionPtr->distortion1 = distortion1;
	distortionPtr->distortion2 = distortion2;
	distortionPtr->distortion3 = distortion3;
	distortionPtr->distortionScale = distortionScale;
	distortionPtr->distortionBias = distortionBias;

	deviceContext->Unmap(m_distortionBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_distortionBuffer);

	return true;
}

void FireShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� �������̴��� �ȼ����̴� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ����̴����� ���÷� ���� ����
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleState2);

	// �ﰢ���� �׸���
	deviceContext->DrawIndexed(indexCount, 0, 0);
}