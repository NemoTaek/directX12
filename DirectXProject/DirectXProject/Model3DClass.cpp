#include "Stdafx.h"
#include "TextureArrayClass.h"
#include "Model3DClass.h"

#include <fstream>
using namespace std;

Model3DClass::Model3DClass() {}
Model3DClass::Model3DClass(const Model3DClass& other) {}
Model3DClass:: ~Model3DClass() {}

bool Model3DClass::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* textureFilename1, const WCHAR* textureFilename2, const WCHAR* textureFilename3)
{
	if (!LoadModel(modelFilename))	return false;
	CalculateModelVectors();	// 모델의 법선, 접선, 이항벡터 계산
	if (!InitializeBuffers(device))	return false;
	return LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
}

void Model3DClass::Shutdown()
{
	ReleaseTextures();
	ShutdownBuffers();
	ReleaseModel();
}

void Model3DClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int Model3DClass::GetIndexCount() { return m_indexCount; }

ID3D11ShaderResourceView** Model3DClass::GetTextureArray() { return m_textureArray->GetTextureArray(); }

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
		vertices[i].tangent = XMFLOAT3(m_model3D[i].tx, m_model3D[i].ty, m_model3D[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model3D[i].bx, m_model3D[i].by, m_model3D[i].bz);

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

bool Model3DClass::LoadTextures(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{
	// 텍스쳐 객체 생성
	m_textureArray = new TextureArrayClass;
	if (!m_textureArray)	return false;

	// 텍스쳐 객체 초기화
	return m_textureArray->Initialize(device, filename1, filename2, filename3);
}

void Model3DClass::ReleaseTextures()
{
	// 택스쳐 객체 릴리즈
	if (m_textureArray)
	{
		m_textureArray->Shutdown();
		delete m_textureArray;
		m_textureArray = 0;
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

void Model3DClass::CalculateModelVectors()
{
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	// 모델의 면 수 계산
	int faceCount = m_vertexCount / 3;

	// 모델 데이터에 대한 인덱스 초기화
	int index = 0;

	// 모든 면을 살펴보고 tangent, binoraml, normal 벡터 계산 
	for (int i = 0; i < faceCount; i++) {
		vertex1.x = m_model3D[index].x;
		vertex1.y = m_model3D[index].y;
		vertex1.z = m_model3D[index].z;
		vertex1.tu = m_model3D[index].tu;
		vertex1.tv = m_model3D[index].tv;
		vertex1.nx = m_model3D[index].nx;
		vertex1.ny = m_model3D[index].ny;
		vertex1.nz = m_model3D[index].nz;
		index++;

		vertex2.x = m_model3D[index].x;
		vertex2.y = m_model3D[index].y;
		vertex2.z = m_model3D[index].z;
		vertex2.tu = m_model3D[index].tu;
		vertex2.tv = m_model3D[index].tv;
		vertex2.nx = m_model3D[index].nx;
		vertex2.ny = m_model3D[index].ny;
		vertex2.nz = m_model3D[index].nz;
		index++;

		vertex3.x = m_model3D[index].x;
		vertex3.y = m_model3D[index].y;
		vertex3.z = m_model3D[index].z;
		vertex3.tu = m_model3D[index].tu;
		vertex3.tv = m_model3D[index].tv;
		vertex3.nx = m_model3D[index].nx;
		vertex3.ny = m_model3D[index].ny;
		vertex3.nz = m_model3D[index].nz;
		index++;

		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		CalculateNormal(tangent, binormal, normal);

		m_model3D[index - 1].nx = normal.x;
		m_model3D[index - 1].ny = normal.y;
		m_model3D[index - 1].nz = normal.z;
		m_model3D[index - 1].tx = tangent.x;
		m_model3D[index - 1].ty = tangent.y;
		m_model3D[index - 1].tz = tangent.z;
		m_model3D[index - 1].bx = binormal.x;
		m_model3D[index - 1].by = binormal.y;
		m_model3D[index - 1].bz = binormal.z;

		m_model3D[index - 2].nx = normal.x;
		m_model3D[index - 2].ny = normal.y;
		m_model3D[index - 2].nz = normal.z;
		m_model3D[index - 2].tx = tangent.x;
		m_model3D[index - 2].ty = tangent.y;
		m_model3D[index - 2].tz = tangent.z;
		m_model3D[index - 2].bx = binormal.x;
		m_model3D[index - 2].by = binormal.y;
		m_model3D[index - 2].bz = binormal.z;

		m_model3D[index - 3].nx = normal.x;
		m_model3D[index - 3].ny = normal.y;
		m_model3D[index - 3].nz = normal.z;
		m_model3D[index - 3].tx = tangent.x;
		m_model3D[index - 3].ty = tangent.y;
		m_model3D[index - 3].tz = tangent.z;
		m_model3D[index - 3].bx = binormal.x;
		m_model3D[index - 3].by = binormal.y;
		m_model3D[index - 3].bz = binormal.z;
	}
}

void Model3DClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	// 현재 표면의 두 벡터 계산
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// 텍스처 공간 벡터 계산
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// 외적 계산으로 tangent, binormal 계산
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// 법선의 길이 계산 후 정규화
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

void Model3DClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	// 외적 계산으로 normal 계산
	normal.x = tangent.y * binormal.z - tangent.z * binormal.y;
	normal.y = tangent.z * binormal.x - tangent.x * binormal.z;
	normal.z = tangent.x * binormal.y - tangent.y * binormal.x;

	// 법선의 길이 계산 후 정규화
	float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;
}