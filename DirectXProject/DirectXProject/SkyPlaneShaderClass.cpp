#include "Stdafx.h"
#include "SkyPlaneShaderClass.h"

SkyPlaneShaderClass::SkyPlaneShaderClass() {}
SkyPlaneShaderClass::SkyPlaneShaderClass(const SkyPlaneShaderClass& other) {}
SkyPlaneShaderClass:: ~SkyPlaneShaderClass() {}

bool SkyPlaneShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//bool result;
	//WCHAR vs[] = L"VS.hlsl";
	//WCHAR ps[] = L"PS.hlsl";
	//result = InitializeShader(device, hwnd, vs, ps);
	//return result;
	return InitializeShader(device, hwnd, L"./shader/SkyPlane.vs", L"./shader/SkyPlane.ps");
}

void SkyPlaneShaderClass::Shutdown()
{
	ShutdownShader();
}

bool SkyPlaneShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, float firstTranslationX, float firstTranslationZ, float secondTranslationX, float secondTranslationZ, float brightness)
{
	// 렌더링에 사용할 셰이더 매개변수 설정
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, firstTranslationX, firstTranslationZ, secondTranslationX, secondTranslationZ, brightness))	return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool SkyPlaneShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// 정점셰이더 코드 컴파일
	// D3DCompileFromFile 의 4번째 매개변수(pEntryPoint)가 해당 셰이더 파일의 진입점이 같아야 통과함. 다르면 그냥 무조건 오류띄워버림
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "SkyPlaneVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK); }
		return false;
	}

	// 픽셀셰이더 코드 컴파일
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "SkyPlanePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
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
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))	return false;

	// 더이상 사용되지 않는 정점셰이더 버퍼와 픽셀셰이더 버퍼를 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 텍스쳐 샘플러 상태 서술자 설정
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

	// 정점셰이더에 있는 행렬 상수버퍼의 서술자 작성
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

	// 픽셀 셰이더에있는 광원 동적 상수 버퍼 서술자 작성
	D3D11_BUFFER_DESC skyBufferDesc;
	skyBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	skyBufferDesc.ByteWidth = sizeof(SkyBufferType);
	skyBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	skyBufferDesc.MiscFlags = 0;
	skyBufferDesc.StructureByteStride = 0;

	// 정점셰이더 상수버퍼에 접근할 수 있도록 상수버퍼 생성
	if (FAILED(device->CreateBuffer(&skyBufferDesc, NULL, &m_skyBuffer)))	return false;

	return true;
}

void SkyPlaneShaderClass::ShutdownShader()
{
	// 텍스처 정보 상수버퍼 해제
	if (m_skyBuffer) {
		m_skyBuffer->Release();
		m_skyBuffer = 0;
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

void SkyPlaneShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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
	//fout.open("light-error.txt");

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

bool SkyPlaneShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, float firstTranslationX, float firstTranslationZ, float secondTranslationX, float secondTranslationZ, float brightness)
{
	// 셰이더에서 사용할 수 있도록 전치행렬화
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼 자원에 자료를 올릴 수 있도록 포인터 생성
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	ConstantBufferType* constantPtr = (ConstantBufferType*)mappedResource.pData;

	// 상수버퍼에 행렬 복사
	constantPtr->world = worldMatrix;
	constantPtr->view = viewMatrix;
	constantPtr->projection = projectionMatrix;

	// 상수버퍼 메모리 해제
	deviceContext->Unmap(m_constantBuffer, 0);
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	if (FAILED(deviceContext->Map(m_skyBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;
	SkyBufferType* skyBufferPtr = (SkyBufferType*)mappedResource.pData;

	skyBufferPtr->firstTranslationX = firstTranslationX;
	skyBufferPtr->firstTranslationZ = firstTranslationZ;
	skyBufferPtr->secondTranslationX = secondTranslationX;
	skyBufferPtr->secondTranslationZ = secondTranslationZ;
	skyBufferPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_skyBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_skyBuffer);

	// 픽셀셰이더에서 셰이더 텍스쳐 리소스 설정
	deviceContext->PSSetShaderResources(0, 1, &texture1);
	deviceContext->PSSetShaderResources(1, 1, &texture2);

	return true;
}

void SkyPlaneShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃 설정
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점셰이더와 픽셀셰이더 설정
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그린다
	deviceContext->DrawIndexed(indexCount, 0, 0);
}