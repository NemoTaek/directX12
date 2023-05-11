#include "Stdafx.h"
#include "TextureClass.h"
#include "BitmapClass.h"

#include <fstream>
using namespace std;

BitmapClass::BitmapClass() {}
BitmapClass::BitmapClass(const BitmapClass& other) {}
BitmapClass:: ~BitmapClass() {}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	// ȭ�� ũ�⸦ ��� ������ ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ������ �� ��Ʈ�� �ȼ��� ũ�⸦ ����
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// ���� ������ ��ġ�� �ʱ�ȭ
	m_previousPosX = -1;
	m_previousPosY = -1;

	if (!InitializeBuffers(device))	return false;

	if (!LoadTexture(device, textureFilename))	return false;

	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// ȭ���� �ٸ� ��ġ�� �������ϱ� ���� ���� ���� ���۸� ����
	if (!UpdateBuffers(deviceContext, positionX, positionY))	return false;

	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* BitmapClass::GetTexture() { return m_texture->GetTexture(); }

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture)	return false;

	return m_texture->Initialize(device, filename);
}


void BitmapClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭 �� �ε��� �迭 ũ�� ����
	m_vertexCount = 6;
	m_indexCount = 6;

	// ���� �迭 ���� �� �ʱ�ȭ
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭 ���� �� �ʱ�ȭ
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;
	for (int i = 0; i < m_indexCount; i++)	indices[i] = i;

	// ���� ���� ������ ����
	// ���� ���� ���۸� ���� Usage�� DEFAULT���� DYNAMIC���� �����ϰ�, CPUAccessFlags�� WRITE�� ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
	// �ε��� ���۴� ������ ��ġ�� �ٲ� ������ ���� ������ ����Ű�� ������ ���� ���۷� ���� �ʿ�� ����
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

void BitmapClass::ShutdownBuffers()
{
	//  �ε��� ���� ����
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// ���� ���� ����
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	// ���� ���� �ڿ��� �ڷḦ �ø� �� �ֵ��� ������ ����
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	VertexType* verticesPtr;

	// ��Ʈ���� ������ �� ��ġ�� ����� ���� ������Ʈ ����
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))	return true;

	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// ��Ʈ�� ��ǥ ���
	// ���߾��� (0, 0)���� �ξ� �����¿� ���
	left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);
	right = left + static_cast<float>(m_bitmapWidth);
	top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);
	bottom = top - static_cast<float>(m_bitmapHeight);

	// ���� �迭 ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	vertices[0].position = XMFLOAT3(left, top, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[3].position = XMFLOAT3(left, top, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[4].position = XMFLOAT3(right, top, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���� ���� ����
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;

	// ���� ���ۿ� ������ ����
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ���� �޸� ����
	deviceContext->Unmap(m_vertexBuffer, 0);
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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