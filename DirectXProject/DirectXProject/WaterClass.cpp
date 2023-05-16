#include "Stdafx.h"
#include "TextureClass.h"
#include "WaterClass.h"

WaterClass::WaterClass() {}
WaterClass::WaterClass(const WaterClass& other) {}
WaterClass::~WaterClass() {}

bool WaterClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename, float waterHeight, float waterRadius)
{
	// ���� ����
	m_waterHeight = waterHeight;

	if (!InitializeBuffers(device, waterRadius))	return false;
	if (!LoadTexture(device, textureFilename))	return false;

	// �� ���� ���� Ÿ�ϸ� ����
	m_normalMapTiling.x = 0.01f;	// ���忡 10�� Ÿ���� ���δ�
	m_normalMapTiling.y = 0.02f;	// ���忡 5�� Ÿ���� ���δ�

	// ���� �帧 �ʱ�ȭ
	m_waterTranslation = 0.0f;

	// �� ���� ���� �����ϸ� �� ����
	m_reflectRefractScale = 0.03f;

	// ���� �� ����
	m_refractionTint = XMFLOAT4(0.0f, 0.8f, 1.0f, 1.0f);

	// ���� ���ߴ� �� ���� ����
	m_specularShiness = 200.0f;

	return true;
}

void WaterClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void WaterClass::Frame()
{
	m_waterTranslation += 0.003f;
	if (m_waterTranslation > 1.0f)	m_waterTranslation -= 1.0f;
}

void WaterClass::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

int WaterClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* WaterClass::GetTexture() { return m_texture->GetTexture(); }

float WaterClass::GetWaterHeight() { return m_waterHeight; }

XMFLOAT2 WaterClass::GetNormalMapTiling() { return m_normalMapTiling; }

float WaterClass::GetWaterTranslation() { return m_waterTranslation; }

float WaterClass::GetReflectRefractScale() { return m_reflectRefractScale; }

XMFLOAT4 WaterClass::GetRefractionTint() { return m_refractionTint; }

float WaterClass::GetSpecularShiness() { return m_specularShiness; }

bool WaterClass::InitializeBuffers(ID3D11Device* device, float waterRadius)
{
	m_vertexCount = 6;
	m_indexCount = 6;

	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	vertices[0].position = XMFLOAT3(-waterRadius, 0.0f, waterRadius);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	indices[0] = 0;

	vertices[1].position = XMFLOAT3(waterRadius, 0.0f, waterRadius);
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);
	indices[1] = 1;

	vertices[2].position = XMFLOAT3(-waterRadius, 0.0f, -waterRadius);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	indices[2] = 2;

	vertices[3].position = XMFLOAT3(-waterRadius, 0.0f, -waterRadius);
	vertices[3].texture = XMFLOAT2(0.0f, 1.0f);
	indices[3] = 3;

	vertices[4].position = XMFLOAT3(waterRadius, 0.0f, waterRadius);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	indices[4] = 4;

	vertices[5].position = XMFLOAT3(waterRadius, 0.0f, -waterRadius);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);
	indices[5] = 5;

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

void WaterClass::ShutdownBuffers()
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

void WaterClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool WaterClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	if (!m_texture->Initialize(device, filename))	return false;

	return true;
}

void WaterClass::ReleaseTexture()
{
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}