#include "Stdafx.h"
#include "TextureClass.h"
#include "SkyPlaneClass.h"

SkyPlaneClass::SkyPlaneClass() {}
SkyPlaneClass::SkyPlaneClass(const SkyPlaneClass& other) {}
SkyPlaneClass::~SkyPlaneClass() {}

bool SkyPlaneClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename1, const WCHAR* textureFilename2)
{
	// 하늘 평면 매개변수 설정
	int skyPlaneResolution = 10;	// 하늘 평면 해상도
	int textureRepeat = 4;
	float skyPlaneWidth = 10.0f;
	float skyPlaneTop = 0.5f;
	float skyPlaneBottom = 0.0f;

	// 구름의 밝기 설정
	m_brightness = 0.65f;

	// 구름 변환 속도 설정
	m_translationSpeed[0] = 0.0003f;	// 1번째 텍스처 X 변환 속도
	m_translationSpeed[1] = 0.0f;		// 1번째 텍스처 Z 변환 속도
	m_translationSpeed[2] = 0.00015f;	// 2번째 텍스처 X 변환 속도
	m_translationSpeed[3] = 0.0f;		// 2번째 텍스처 Z 변환 속도

	// 텍스처 변환 값 초기화
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;

	// 하늘 평면 생성
	if (!InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat))	return false;
	if (!InitializeBuffers(device, skyPlaneResolution))	return false;

	// 하늘 평면 텍스처 로드
	if (!LoadTextures(device, textureFilename1, textureFilename2))	return false;

	return true;
}

void SkyPlaneClass::Shutdown()
{
	ReleaseTextures();
	ShutdownBuffers();
	ShutdownSkyPlane();
}

void SkyPlaneClass::Frame()
{
	// 움직이는 구름을 표현하기 위해 변환 값 증가
	m_textureTranslation[0] += m_translationSpeed[0];
	m_textureTranslation[1] += m_translationSpeed[1];
	m_textureTranslation[2] += m_translationSpeed[2];
	m_textureTranslation[3] += m_translationSpeed[3];

	// 값을 0~1 범위로 유지
	if (m_textureTranslation[0] > 1.0f)	m_textureTranslation[0] -= 1.0f;
	if (m_textureTranslation[1] > 1.0f)	m_textureTranslation[1] -= 1.0f;
	if (m_textureTranslation[2] > 1.0f)	m_textureTranslation[2] -= 1.0f;
	if (m_textureTranslation[3] > 1.0f)	m_textureTranslation[3] -= 1.0f;
}

void SkyPlaneClass::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

int SkyPlaneClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* SkyPlaneClass::GetCloudTexture1() { return m_cloudTexture1->GetTexture(); }

ID3D11ShaderResourceView* SkyPlaneClass::GetCloudTexture2() { return m_cloudTexture2->GetTexture(); }

float SkyPlaneClass::GetBrightness() { return m_brightness; }

float SkyPlaneClass::GetTranslation(int index) { return m_textureTranslation[index]; }

bool SkyPlaneClass::InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat)
{
	float positionX = 0.0f;
	float positionY = 0.0f;
	float positionZ = 0.0f;
	float tu = 0.0f;
	float tv = 0.0f;

	m_skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	if (!m_skyPlane)	return false;

	// 하늘 평면에서 각 쿼드의 그기 결정
	float quadSize = skyPlaneWidth / static_cast<float>(skyPlaneResolution);

	// 너비를 기준으로 하늘 평면의 반지름 계산
	float radius = skyPlaneWidth / 2.0f;

	// 증가할 높이 상수 계산
	float constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// 텍스처 좌표 증가 값 계산
	float textureDelta = static_cast<float>(textureRepeat) / static_cast<float>(skyPlaneResolution);

	// 하늘 평면을 반복하고 증가 값을 기준으로 좌표 생성
	for (int i = 0; i <= skyPlaneResolution; i++) {
		for (int j = 0; j <= skyPlaneResolution; j++) {
			// 정점 좌표 계산
			positionX = (-0.5f * skyPlaneWidth) + (static_cast<float>(j) * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + (static_cast<float>(i) * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ + positionZ)));
			
			// 텍스처 좌표 계산
			tu = static_cast<float>(j) * textureDelta;
			tv = static_cast<float>(i) * textureDelta;

			int index = i * (skyPlaneResolution + 1) + j;

			// 하늘 평면 배열에 좌표 추가
			m_skyPlane[index].x = positionX;
			m_skyPlane[index].y = positionY;
			m_skyPlane[index].z = positionZ;
			m_skyPlane[index].tu = tu;
			m_skyPlane[index].tv = tv;
		}
	}

	return true;
}

bool SkyPlaneClass::InitializeBuffers(ID3D11Device* device, int skyPlaneResolution)
{
	m_vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;
	m_indexCount = m_vertexCount;

	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	int index = 0;

	for (int i = 0; i < skyPlaneResolution; i++) {
		for (int j = 0; j < skyPlaneResolution; j++) {
			int indexLeftTop = ((skyPlaneResolution + 1) * i) + j;
			int indexRightTop = ((skyPlaneResolution + 1) * i) + (j + 1);
			int indexLeftBottom = ((skyPlaneResolution + 1) * (i + 1)) + j;
			int indexRightBottom = ((skyPlaneResolution + 1) * (i + 1)) + (j + 1);

			// 좌상
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftTop].x, m_skyPlane[indexLeftTop].y, m_skyPlane[indexLeftTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftTop].tu, m_skyPlane[indexLeftTop].tv);
			indices[index] = index;
			index++;

			// 우상
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightTop].x, m_skyPlane[indexRightTop].y, m_skyPlane[indexRightTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightTop].tu, m_skyPlane[indexRightTop].tv);
			indices[index] = index;
			index++;

			// 좌하
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftBottom].x, m_skyPlane[indexLeftBottom].y, m_skyPlane[indexLeftBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftBottom].tu, m_skyPlane[indexLeftBottom].tv);
			indices[index] = index;
			index++;

			// 좌하
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftBottom].x, m_skyPlane[indexLeftBottom].y, m_skyPlane[indexLeftBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftBottom].tu, m_skyPlane[indexLeftBottom].tv);
			indices[index] = index;
			index++;

			// 우상
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightTop].x, m_skyPlane[indexRightTop].y, m_skyPlane[indexRightTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightTop].tu, m_skyPlane[indexRightTop].tv);
			indices[index] = index;
			index++;

			// 우하
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightBottom].x, m_skyPlane[indexRightBottom].y, m_skyPlane[indexRightBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightBottom].tu, m_skyPlane[indexRightBottom].tv);
			indices[index] = index;
			index++;
		}
	}

	// 정점 버퍼 서술자 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// 인덱스 버퍼 서술자 설정
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))	return false;

	// 생성 후 정점 및 인덱스 버퍼 해제
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

void SkyPlaneClass::ShutdownSkyPlane()
{
	if (m_skyPlane) {
		delete[] m_skyPlane;
		m_skyPlane = 0;
	}
}

void SkyPlaneClass::ShutdownBuffers()
{
	//  인덱스 버퍼 해제
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 정점셰이더 해제
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void SkyPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool SkyPlaneClass::LoadTextures(ID3D11Device* device, const WCHAR* textureFilename1, const WCHAR* textureFilename2)
{
	m_cloudTexture1 = new TextureClass;
	if (!m_cloudTexture1)	return false;
	if (!m_cloudTexture1->Initialize(device, textureFilename1))	return false;

	m_cloudTexture2 = new TextureClass;
	if (!m_cloudTexture2)	return false;
	if (!m_cloudTexture2->Initialize(device, textureFilename2))	return false;

	return true;
}

void SkyPlaneClass::ReleaseTextures()
{
	if (m_cloudTexture1) {
		m_cloudTexture1->Shutdown();
		delete m_cloudTexture1;
		m_cloudTexture1 = nullptr;
	}

	if (m_cloudTexture2) {
		m_cloudTexture2->Shutdown();
		delete m_cloudTexture2;
		m_cloudTexture2 = nullptr;
	}
}