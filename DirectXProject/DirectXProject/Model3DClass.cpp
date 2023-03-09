#include "Stdafx.h"
#include "TextureClass.h"
#include "Model3DClass.h"

#include <fstream>
using namespace std;

Model3DClass::Model3DClass() {}
Model3DClass::Model3DClass(const Model3DClass& other) {}
Model3DClass:: ~Model3DClass() {}

bool Model3DClass::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* textureFilename)
{
	if (!LoadModel(modelFilename))	return false;
	if (!InitializeBuffers(device))	return false;
	return LoadTexture(device, textureFilename);
}

void Model3DClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

void Model3DClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int Model3DClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView* Model3DClass::GetTexture() { return m_Texture->GetTexture(); }

bool Model3DClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭 ����
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// �ε��� �迭 ����
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// ���� �迭�� �ε��� �迭�� �����ͷ� �о��
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model3D[i].x, m_model3D[i].y, m_model3D[i].z);
		vertices[i].texture = XMFLOAT2(m_model3D[i].tu, m_model3D[i].tv);
		vertices[i].normal = XMFLOAT3(m_model3D[i].nx, m_model3D[i].ny, m_model3D[i].nz);

		indices[i] = i;
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

void Model3DClass::ShutdownBuffers()
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

void Model3DClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool Model3DClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ��� ��ü ����
	m_Texture = new TextureClass;
	if (!m_Texture)	return false;

	// �ؽ��� ��ü �ʱ�ȭ
	return m_Texture->Initialize(device, filename);
}

void Model3DClass::ReleaseTexture()
{
	// �ý��� ��ü ������
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

bool Model3DClass::LoadModel(const WCHAR* filename)
{
	ifstream fin;
	fin.open(filename);

	if (fin.fail())	return false;

	// ���� ������ŭ �о��
	char input = 0;
	fin.get(input);
	// vertex Count: ����
	while (input != ':') { fin.get(input); }

	// ���� ���� �о��
	fin >> m_vertexCount;

	// �ε��� ���� ���� ���� ���� ����
	m_indexCount = m_vertexCount;

	// �о�� ���� ������ �� ����
	m_model3D = new ModelType[m_vertexCount];
	if (!m_model3D)	return false;

	// �������� ���ۺκб��� �о��
	fin.get(input);
	// vertex Count: ����
	while (input != ':') { fin.get(input); }
	fin.get(input);
	fin.get(input);

	// ���� ������ �о��
	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model3D[i].x >> m_model3D[i].y >> m_model3D[i].z;
		fin >> m_model3D[i].tu >> m_model3D[i].tv;
		fin >> m_model3D[i].nx >> m_model3D[i].ny >> m_model3D[i].nz;
	}

	fin.close();

	return true;
}

void Model3DClass::ReleaseModel()
{
	if (m_model3D)
	{
		delete[] m_model3D;
		m_model3D = 0;
	}
}