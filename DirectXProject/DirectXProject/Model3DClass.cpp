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
	// 정점 배열 생성
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// 인덱스 배열 생성
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// 정점 배열과 인덱스 배열을 데이터로 읽어옴
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model3D[i].x, m_model3D[i].y, m_model3D[i].z);
		vertices[i].texture = XMFLOAT2(m_model3D[i].tu, m_model3D[i].tv);
		vertices[i].normal = XMFLOAT3(m_model3D[i].nx, m_model3D[i].ny, m_model3D[i].nz);

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

void Model3DClass::ShutdownBuffers()
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

void Model3DClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool Model3DClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스쳐 객체 생성
	m_Texture = new TextureClass;
	if (!m_Texture)	return false;

	// 텍스쳐 객체 초기화
	return m_Texture->Initialize(device, filename);
}

void Model3DClass::ReleaseTexture()
{
	// 택스쳐 객체 릴리즈
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

	// 정점 개수만큼 읽어옴
	char input = 0;
	fin.get(input);
	// vertex Count: 개수
	while (input != ':') { fin.get(input); }

	// 정점 개수 읽어옴
	fin >> m_vertexCount;

	// 인덱스 수를 정점 수와 같게 설정
	m_indexCount = m_vertexCount;

	// 읽어온 정점 개수로 모델 생성
	m_model3D = new ModelType[m_vertexCount];
	if (!m_model3D)	return false;

	// 데이터의 시작부분까지 읽어옴
	fin.get(input);
	// vertex Count: 개수
	while (input != ':') { fin.get(input); }
	fin.get(input);
	fin.get(input);

	// 정점 데이터 읽어옴
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