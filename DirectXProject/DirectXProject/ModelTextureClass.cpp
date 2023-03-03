#include "Stdafx.h"
#include "TextureClass.h"
#include "ModelTextureClass.h"

ModelTextureClass::ModelTextureClass() {}
ModelTextureClass::ModelTextureClass(const ModelTextureClass& other) {}
ModelTextureClass:: ~ModelTextureClass() {}

bool ModelTextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* textureFilename)
{
	if (!InitializeBuffers(device))	return false;
	return LoadTexture(device, deviceContext, textureFilename);
}

void ModelTextureClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelTextureClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelTextureClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* ModelTextureClass::GetTexture() { return m_Texture->GetTexture(); }

bool ModelTextureClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ���� ��
	m_vertexCount = 3;	// �ﰢ��
	m_vertexCount = 4;	// �簢��

	// �ε��� �迭�� �ε��� ��
	m_indexCount = 3;	// �ﰢ��
	m_indexCount = 6;	// �簢��(���� ���۷� �׸� �⺻���� �ﰢ���̱� ������ �ﰢ�� 2���� ������ �簢���� �ȴ�)

	// ���� �迭 ����
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// �ε��� �迭 ����
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// �ﰢ��
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// �簢��
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

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

void ModelTextureClass::ShutdownBuffers()
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

void ModelTextureClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ModelTextureClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* filename)
{
	// �ؽ��� ��ü ����
	m_Texture = new TextureClass;
	if (!m_Texture)	return false;

	// �ؽ��� ��ü �ʱ�ȭ
	return m_Texture->Initialize(device, deviceContext, filename);
}

void ModelTextureClass::ReleaseTexture()
{
	// �ý��� ��ü ������
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}