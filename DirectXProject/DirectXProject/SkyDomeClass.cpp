#include "Stdafx.h"
#include "SkyDomeClass.h"
#include <fstream>
using namespace std;

SkyDomeClass::SkyDomeClass() {}
SkyDomeClass::SkyDomeClass(const SkyDomeClass& other) {}
SkyDomeClass::~SkyDomeClass() {}

bool SkyDomeClass::Initialize(ID3D11Device* device)
{
	// 하늘 모델 로드
	if (!LoadSkyDomeModel("./data/skydome.txt"))	return false;

	if (!InitializeBuffers(device))	return false;

	// 하늘 꼭대기 색상 설정
	m_apexColor = XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f);

	// 하늘 중심 색상 설정
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
	// 하늘 텍스트 파일 오픈
	fin.open(skyDomeFilename);
	if (fin.fail())	return false;

	// 정점 카운트 값까지 읽음
	char input = 0;
	fin.get(input);
	while (input != ':')	fin.get(input);

	// 정점 및 인덱스 수
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	// 읽어들인 정점 개수의 모델 생성
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

void SkyDomeClass::ReleaseSkyDomeModel()
{
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
}

void SkyDomeClass::ShutdownBuffers()
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

void SkyDomeClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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