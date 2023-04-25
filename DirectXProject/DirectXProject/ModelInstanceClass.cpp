#include "Stdafx.h"
#include "TextureClass.h"
#include "ModelInstanceClass.h"

ModelInstanceClass::ModelInstanceClass() {}
ModelInstanceClass::ModelInstanceClass(const ModelInstanceClass& other) {}
ModelInstanceClass:: ~ModelInstanceClass() {}

bool ModelInstanceClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	if (!InitializeBuffers(device))	return false;
	return LoadTexture(device, textureFilename);
}

void ModelInstanceClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelInstanceClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelInstanceClass::GetVertexCount() { return m_vertexCount; }
int ModelInstanceClass::GetInstanceCount() { return m_instanceCount; }
ID3D11ShaderResourceView* ModelInstanceClass::GetTexture() { return m_texture->GetTexture(); }

bool ModelInstanceClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ���� ��
	m_vertexCount = 3;	// �ﰢ��

	// ���� �迭 ����
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// �ﰢ��
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

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

	// ���� �� ���� �� �ε��� ���� ����
	delete[] vertices;
	vertices = nullptr;

	// �ν��Ͻ� �迭�� ��
	m_instanceCount = 4;	// 4�� ����

	// �ν��Ͻ� �迭 ����
	InstanceType* instances = new InstanceType[m_instanceCount];
	if (!instances)	return false;

	// �ﰢ��
	instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	instances[1].position = XMFLOAT3(-1.5f, 1.5f, 5.0f);
	instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);

	// �ν��Ͻ� ���� ������ ����
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// ���� ���� ����
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))	return false;

	// ���� �� �ν��Ͻ� ���� ����
	delete[] instances;
	instances = nullptr;

	return true;
}

void ModelInstanceClass::ShutdownBuffers()
{
	// �ν��Ͻ� ���� ����
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = nullptr;
	}

	// �������̴� ����
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void ModelInstanceClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ������ ������ ������ ����
	unsigned int stride[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offset[2] = { 0, 0 };

	// �������� �迭�� ���� ���ۿ� �ν��Ͻ� ���۷� ����
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, stride, offset);

	// ���� ���۷� �׸� �⺻�� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelInstanceClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ��� ��ü ����
	m_texture = new TextureClass;
	if (!m_texture)	return false;
	if (!m_texture->Initialize(device, filename))	return false;

	return true;
}

void ModelInstanceClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}