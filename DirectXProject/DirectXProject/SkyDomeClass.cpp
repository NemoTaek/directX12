#include "Stdafx.h"
#include "SkyDomeClass.h"
#include <fstream>
using namespace std;

SkyDomeClass::SkyDomeClass() {}
SkyDomeClass::SkyDomeClass(const SkyDomeClass& other) {}
SkyDomeClass::~SkyDomeClass() {}

bool SkyDomeClass::Initialize(ID3D11Device* device)
{
	// �ϴ� �� �ε�
	if (!LoadSkyDomeModel("./data/skydome.txt"))	return false;

	if (!InitializeBuffers(device))	return false;

	// �ϴ� ����� ���� ����
	m_apexColor = XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f);

	// �ϴ� �߽� ���� ����
	m_centerColor = XMFLOAT4(0.81f, 0.38f, 0.66f, 1.0f);

	return true;
}

void SkyDomeClass::Shutdown()
{
	ShutdownBuffers();
	ReleaseSkyDomeModel();
}


void SkyDomeClass::Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

int SkyDomeClass::GetIndexCount() { return m_indexCount; }

XMFLOAT4 SkyDomeClass::GetApexColor() { return m_apexColor; }

XMFLOAT4 SkyDomeClass::GetCenterColor() { return m_centerColor; }

bool SkyDomeClass::LoadSkyDomeModel(const char* skyDomeFilename)
{
	ifstream fin;
	// �ϴ� �ؽ�Ʈ ���� ����
	fin.open(skyDomeFilename);
	if (fin.fail())	return false;

	// ���� ī��Ʈ ������ ����
	char input = 0;
	fin.get(input);
	while (input != ':')	fin.get(input);

	// ���� �� �ε��� ��
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	// �о���� ���� ������ �� ����
	m_model = new ModelType[m_vertexCount];
	if (!m_model)	return false;

	fin.get(input);
	while (input != ':')	fin.get(input);
	fin.get(input);
	fin.get(input);

	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

bool SkyDomeClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	int index = 0;

	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
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

void SkyDomeClass::ReleaseSkyDomeModel()
{
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
}

void SkyDomeClass::ShutdownBuffers()
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

void SkyDomeClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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