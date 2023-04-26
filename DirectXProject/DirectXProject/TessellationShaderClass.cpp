#include "Stdafx.h"
#include "TessellationShaderClass.h"
#include <iostream>
#include <fstream>
using namespace std;

TessellationShaderClass::TessellationShaderClass() {}
TessellationShaderClass::TessellationShaderClass(const TessellationShaderClass& other) {}
TessellationShaderClass:: ~TessellationShaderClass() {}

bool TessellationShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"./VertexShader.vs", L"./HullShader.hs", L"./DomainShader.ds", L"./PixelShader.ps");
}

void TessellationShaderClass::Shutdown()
{
	ShutdownShader();
}

// ������ �Ű������� ���� �ؽ��� �϶��� texture, ���� �ؽ��� �϶��� textureArray
bool TessellationShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool TessellationShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* hsFilename, const WCHAR* dsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// �������̴� �ڵ� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "TessellationVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// ����̴� �ڵ� ������
	ID3D10Blob* hullShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(hsFilename, NULL, NULL, "TessellationHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, hsFilename); }
		else { MessageBox(hwnd, hsFilename, L"Missing Hull Shader File", MB_OK); }
		return false;
	}

	// �����μ��̴� �ڵ� ������
	ID3D10Blob* domainShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(dsFilename, NULL, NULL, "TessellationDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, dsFilename); }
		else { MessageBox(hwnd, dsFilename, L"Missing Domain Shader File", MB_OK); }
		return false;
	}

	// �ȼ����̴� �ڵ� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "TessellationPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
		else { MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK); }
		return false;
	}

	// ���۷κ��� �������̴� ����
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))	return false;

	// ���۷κ��� ����̴� ����
	if (FAILED(device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader)))	return false;

	// ���۷κ��� �����μ��̴� ����
	if (FAILED(device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader)))	return false;

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ� ��� ��
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ�
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))	return false;

	// ���̻� ������ �ʴ� ���̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	hullShaderBuffer->Release();
	hullShaderBuffer = 0;

	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

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

	// �׼����̼� ���� ����ü ����
	D3D11_BUFFER_DESC tessellationBufferDesc;
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	// �׼����̼� ���� ����
	if (FAILED(device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer)))	return false;

	return true;
}

void TessellationShaderClass::ShutdownShader()
{
	// �׼����̼� ���� ����
	if (m_tessellationBuffer) {
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
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

	// �����μ��̴� ����
	if (m_domainShader) {
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// ����̴� ����
	if (m_hullShader) {
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// �������̴� ����
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void TessellationShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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

bool TessellationShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
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
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	if (FAILED(deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	TessellationBufferType* tessellationPtr = (TessellationBufferType*)mappedResource.pData;

	tessellationPtr->tessellationAmount = tessellationAmount;
	tessellationPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_tessellationBuffer, 0);
	bufferNumber = 0;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

void TessellationShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� �������̴��� �ȼ����̴� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �׸���
	deviceContext->DrawIndexed(indexCount, 0, 0);
}