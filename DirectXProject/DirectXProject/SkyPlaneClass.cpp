#include "Stdafx.h"
#include "TextureClass.h"
#include "SkyPlaneClass.h"

SkyPlaneClass::SkyPlaneClass() {}
SkyPlaneClass::SkyPlaneClass(const SkyPlaneClass& other) {}
SkyPlaneClass::~SkyPlaneClass() {}

bool SkyPlaneClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename1, const WCHAR* textureFilename2)
{
	// �ϴ� ��� �Ű����� ����
	int skyPlaneResolution = 10;	// �ϴ� ��� �ػ�
	int textureRepeat = 4;
	float skyPlaneWidth = 10.0f;
	float skyPlaneTop = 0.5f;
	float skyPlaneBottom = 0.0f;

	// ������ ��� ����
	m_brightness = 0.65f;

	// ���� ��ȯ �ӵ� ����
	m_translationSpeed[0] = 0.0003f;	// 1��° �ؽ�ó X ��ȯ �ӵ�
	m_translationSpeed[1] = 0.0f;		// 1��° �ؽ�ó Z ��ȯ �ӵ�
	m_translationSpeed[2] = 0.00015f;	// 2��° �ؽ�ó X ��ȯ �ӵ�
	m_translationSpeed[3] = 0.0f;		// 2��° �ؽ�ó Z ��ȯ �ӵ�

	// �ؽ�ó ��ȯ �� �ʱ�ȭ
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;

	// �ϴ� ��� ����
	if (!InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat))	return false;
	if (!InitializeBuffers(device, skyPlaneResolution))	return false;

	// �ϴ� ��� �ؽ�ó �ε�
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
	// �����̴� ������ ǥ���ϱ� ���� ��ȯ �� ����
	m_textureTranslation[0] += m_translationSpeed[0];
	m_textureTranslation[1] += m_translationSpeed[1];
	m_textureTranslation[2] += m_translationSpeed[2];
	m_textureTranslation[3] += m_translationSpeed[3];

	// ���� 0~1 ������ ����
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

	// �ϴ� ��鿡�� �� ������ �ױ� ����
	float quadSize = skyPlaneWidth / static_cast<float>(skyPlaneResolution);

	// �ʺ� �������� �ϴ� ����� ������ ���
	float radius = skyPlaneWidth / 2.0f;

	// ������ ���� ��� ���
	float constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// �ؽ�ó ��ǥ ���� �� ���
	float textureDelta = static_cast<float>(textureRepeat) / static_cast<float>(skyPlaneResolution);

	// �ϴ� ����� �ݺ��ϰ� ���� ���� �������� ��ǥ ����
	for (int i = 0; i <= skyPlaneResolution; i++) {
		for (int j = 0; j <= skyPlaneResolution; j++) {
			// ���� ��ǥ ���
			positionX = (-0.5f * skyPlaneWidth) + (static_cast<float>(j) * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + (static_cast<float>(i) * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ + positionZ)));
			
			// �ؽ�ó ��ǥ ���
			tu = static_cast<float>(j) * textureDelta;
			tv = static_cast<float>(i) * textureDelta;

			int index = i * (skyPlaneResolution + 1) + j;

			// �ϴ� ��� �迭�� ��ǥ �߰�
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

			// �»�
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftTop].x, m_skyPlane[indexLeftTop].y, m_skyPlane[indexLeftTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftTop].tu, m_skyPlane[indexLeftTop].tv);
			indices[index] = index;
			index++;

			// ���
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightTop].x, m_skyPlane[indexRightTop].y, m_skyPlane[indexRightTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightTop].tu, m_skyPlane[indexRightTop].tv);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftBottom].x, m_skyPlane[indexLeftBottom].y, m_skyPlane[indexLeftBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftBottom].tu, m_skyPlane[indexLeftBottom].tv);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_skyPlane[indexLeftBottom].x, m_skyPlane[indexLeftBottom].y, m_skyPlane[indexLeftBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexLeftBottom].tu, m_skyPlane[indexLeftBottom].tv);
			indices[index] = index;
			index++;

			// ���
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightTop].x, m_skyPlane[indexRightTop].y, m_skyPlane[indexRightTop].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightTop].tu, m_skyPlane[indexRightTop].tv);
			indices[index] = index;
			index++;

			// ����
			vertices[index].position = XMFLOAT3(m_skyPlane[indexRightBottom].x, m_skyPlane[indexRightBottom].y, m_skyPlane[indexRightBottom].z);
			vertices[index].texture = XMFLOAT2(m_skyPlane[indexRightBottom].tu, m_skyPlane[indexRightBottom].tv);
			indices[index] = index;
			index++;
		}
	}

	// ���� ���� ������ ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))	return false;

	// �ε��� ���� ������ ����
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))	return false;

	// ���� �� ���� �� �ε��� ���� ����
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
	//  �ε��� ���� ����
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// �������̴� ����
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void SkyPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ������ ������ ������ ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� �� �ε��� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
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