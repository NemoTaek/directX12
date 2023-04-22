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

// 마지막 매개변수를 단일 텍스쳐 일때는 texture, 다중 텍스쳐 일때는 textureArray
bool FireShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* fireTexture, ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture, float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales,
	XMFLOAT2 distortion1, XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale, float distortionBias)
{
	// 렌더링에 사용할 셰이더 매개변수 설정
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture, alphaTexture, frameTime, scrollSpeeds, scales,
		distortion1, distortion2, distortion3, distortionScale, distortionBias))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FireShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// 정점셰이더 코드 컴파일
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "FireVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// 픽셀셰이더 코드 컴파일
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "FirePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
		else { MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK); }
		return false;
	}

	// 버퍼로부터 정점셰이더 생성
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))	return false;

	// 버퍼로부터 픽셀셰이더 생성
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))	return false;

	// 정점 입력 레이아웃 구조체 설정
	// ModelClass와 셰이더의 VertexType 구조와 일치해야한다.
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

	// 레이아웃 요소 수
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))	return false;

	// 더이상 사용되지 않는 정점셰이더 버퍼와 픽셀셰이더 버퍼를 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점셰이더에 있는 행렬 상수버퍼의 구조체 작성
	// 상수버퍼는 셰이더 프로그램에서 참조하는 자료를 담는 GPU 자원이다.
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// 정점셰이더 상수버퍼에 접근할 수 있도록 상수버퍼 생성
	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer)))	return false;

	// 텍스쳐 샘플러 상태 구조체 설정
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

	// 텍스쳐 샘플러 상태 생성
	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))	return false;

	// 불꽃 효과에 사용할 때는 WRAP 대신 CLAMP를 사용하여 바닥쪽이 망가지지 않게 함
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

	// 텍스쳐 샘플러 상태 생성
	if (FAILED(device->CreateSamplerState(&samplerDesc2, &m_sampleState2)))	return false;

	// 노이즈 버퍼 구조체 설정
	D3D11_BUFFER_DESC noiseBufferDesc;
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	// 노이즈 버퍼 생성
	if (FAILED(device->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer)))	return false;

	// 왜곡 버퍼 구조체 설정
	D3D11_BUFFER_DESC distortionBufferDesc;
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	// 왜곡 버퍼 생성
	if (FAILED(device->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer)))	return false;

	return true;
}

void FireShaderClass::ShutdownShader()
{
	// 왜곡 상태 해제
	if (m_distortionBuffer) {
		m_distortionBuffer->Release();
		m_distortionBuffer = 0;
	}

	// 노이즈 버퍼 해제
	if (m_noiseBuffer) {
		m_noiseBuffer->Release();
		m_noiseBuffer = 0;
	}

	// 샘플러 상태 해제
	if (m_sampleState2) {
		m_sampleState2->Release();
		m_sampleState2 = 0;
	}

	// 샘플러 상태 해제
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 행렬 상수버퍼 해제
	if (m_constantBuffer) {
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	// 레이아웃 해제
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀셰이더 해제
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점셰이더 해제
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void FireShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// 에러메세지 출력창에 표시
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	//char* compileErrors;
	//unsigned long bufferSize, i;
	//std::ofstream fout;


	//// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져옵니다.
	//compileErrors = (char*)(errorMessage->GetBufferPointer());

	//// 메세지의 길이를 가져옵니다.
	//bufferSize = errorMessage->GetBufferSize();

	//// 파일을 열고 안에 메세지를 기록합니다.
	//fout.open("texture-error.txt");

	//// 에러 메세지를 씁니다.
	//for (i = 0; i < bufferSize; i++)
	//{
	//	fout << compileErrors[i];
	//}

	//// 파일을 닫습니다.
	//fout.close();

	errorMessage->Release();
	errorMessage = 0;
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool FireShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* fireTexture, ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture, float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales,
	XMFLOAT2 distortion1, XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale, float distortionBias)
{
	// 셰이더에서 사용할 수 있도록 전치행렬화
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼 자원에 자료를 올릴 수 있도록 포인터 생성
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	ConstantBufferType* matrixPtr = (ConstantBufferType*)mappedResource.pData;

	// 상수버퍼에 행렬 복사
	matrixPtr->world = worldMatrix;
	matrixPtr->view = viewMatrix;
	matrixPtr->projection = projectionMatrix;

	// 상수버퍼 메모리 해제
	deviceContext->Unmap(m_constantBuffer, 0);
	unsigned bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	// 픽셀셰이더에서 셰이더 텍스쳐 리소스 설정
	// 2번째 인자가 리소스 수라서 블렌딩 하면 블렌딩 할 textureArray 수를 적는다
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
	// 정점 입력 레이아웃 설정
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점셰이더와 픽셀셰이더 설정
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀셰이더에서 샘플러 상태 설정
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleState2);

	// 삼각형을 그린다
	deviceContext->DrawIndexed(indexCount, 0, 0);
}