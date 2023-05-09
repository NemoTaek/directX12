#include "Stdafx.h"
#include "TerrainClass.h"
#include "TerrainShaderClass.h"
#include "FrustumClass.h"
#include "QuadTreeClass.h"

QuadTreeClass::QuadTreeClass() {}
QuadTreeClass::QuadTreeClass(const QuadTreeClass& other) {}
QuadTreeClass::~QuadTreeClass() {}

bool QuadTreeClass::Initialize(TerrainClass* terrain, ID3D11Device* device)
{
	float centerX = 0.0f;
	float centerZ = 0.0f;
	float width = 0.0f;

	// 지형 정점 배열의 정점 수
	int vertexCount = terrain->GetVertexCount();

	// 정점들의 총 삼각형 수
	m_triangleCount = vertexCount / 3;

	// 모든 지형 정점을 포함하는 정점 배열 생성
	m_vertexList = new VertexType[vertexCount];
	if (!m_vertexList)	return false;

	// 지형 정점을 정점 목록에 복사
	terrain->CopyVertexArray(static_cast<void*>(m_vertexList));

	// x와 z의 중심, 메쉬 너비 계산
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// 쿼드 트리의 부모 노드 생성
	m_parentNode = new NodeType;
	if (!m_parentNode)	return false;

	// 쿼드 트리 생성
	CreateTreeNode(m_parentNode, centerX, centerZ, width, device);

	if (m_vertexList) {
		delete[] m_vertexList;
		m_vertexList = 0;
	}

	return true;
}

void QuadTreeClass::Shutdown()
{
	if (m_parentNode) {
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}
}

void QuadTreeClass::Render(FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
	m_drawCount = 0;
	RenderNode(m_parentNode, frustum, deviceContext, shader);
}

int QuadTreeClass::GetDrawCount() { return m_drawCount; }

void QuadTreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	centerX = 0.0f;
	centerZ = 0.0f;

	for (int i = 0; i < vertexCount; i++) {
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	// 메쉬의 중심 계산
	centerX = centerX / static_cast<float>(vertexCount);
	centerZ = centerZ / static_cast<float>(vertexCount);

	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	// fabsf(): float 형 실수의 절댓값 반환
	// fabs(): double형 실수 절댓값 반환
	float minWidth = fabsf(m_vertexList[0].position.x - centerX);
	float minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	// 메쉬의 최대 너비, 최소 너비, 깊이 계산
	for (int i = 0; i < vertexCount; i++) {
		float width = fabsf(m_vertexList[i].position.x - centerX);
		float depth = fabsf(m_vertexList[i].position.z - centerZ);

		if (width > maxWidth)	maxWidth = width;
		if (depth > maxDepth)	maxDepth = depth;
		if (width < minWidth)	minWidth = width;
		if (depth < minDepth)	minDepth = depth;
	}

	float maxX = static_cast<float>(max(fabs(minWidth), fabs(maxWidth)));
	float maxZ = static_cast<float>(max(fabs(minDepth), fabs(maxDepth)));

	// 메쉬의 최대 직경을 계산
	meshWidth = max(maxX, maxZ) * 2.0f;
}


void QuadTreeClass::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	node->triangleCount = 0;

	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	node->vertexArray = 0;

	// 현재 노드에 있는 삼각형 수 카운팅
	int numTriangles = CountTriangles(positionX, positionZ, width);

	// 현재 노드에 삼각형이 없으면 패스
	if (numTriangles == 0)	return;

	// 현재 노드에 제한보다 많은 삼각형이 있는 경우, 4개의 동일한 크기의 더 작은 트리 노드로 분할
	else if (numTriangles > MAX_TRIANGLES) {
		for (int i = 0; i < 4; i++) {
			// 새로운 자식 노드에 대한 위치 계산
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// 새 노드에 삼각형이 있는지 확인
			int count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0) {
				// 삼각형이 있다면 자식 노드 생성 후 트리 확장
				node->nodes[i] = new NodeType;
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
			}
		}
		return;
	}

	// 현재 노드가 비어있지 않고, 노드의 삼각형 수가 최대 값보다 작으면 삼각형 목록 생성
	else {
		node->triangleCount = numTriangles;

		int vertexCount = numTriangles * 3;
		VertexType* vertices = new VertexType[vertexCount];
		unsigned long* indices = new unsigned long[vertexCount];
		node->vertexArray = new VectorType[vertexCount];

		int index = 0;
		int vertexIndex = 0;

		for (int i = 0; i < m_triangleCount; i++) {
			// 삼각형이 현재 노드 안에 있으면 꼭지점 배열에 추가
			if (IsTriangleContained(i, positionX, positionZ, width)) {
				vertexIndex = i * 3;

				vertices[index].position = m_vertexList[vertexIndex].position;
				vertices[index].texture = m_vertexList[vertexIndex].texture;
				vertices[index].normal = m_vertexList[vertexIndex].normal;
				indices[index] = index;
				node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
				node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
				node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;
				index++;

				vertexIndex++;
				vertices[index].position = m_vertexList[vertexIndex].position;
				vertices[index].texture = m_vertexList[vertexIndex].texture;
				vertices[index].normal = m_vertexList[vertexIndex].normal;
				indices[index] = index;
				node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
				node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
				node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;
				index++;

				vertexIndex++;
				vertices[index].position = m_vertexList[vertexIndex].position;
				vertices[index].texture = m_vertexList[vertexIndex].texture;
				vertices[index].normal = m_vertexList[vertexIndex].normal;
				indices[index] = index;
				node->vertexArray[index].x = m_vertexList[vertexIndex].position.x;
				node->vertexArray[index].y = m_vertexList[vertexIndex].position.y;
				node->vertexArray[index].z = m_vertexList[vertexIndex].position.z;
				index++;
			}
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

		delete[] vertices;
		vertices = 0;

		delete[] indices;
		indices = 0;
	}
}

int QuadTreeClass::CountTriangles(float positionX, float positionZ, float width)
{
	int count = 0;

	for (int i = 0; i < m_triangleCount; i++) {
		if (IsTriangleContained(i, positionX, positionZ, width))	count++;
	}

	return count;
}

bool QuadTreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// 현재 노드의 반경을 계산
	float radius = width / 2.0f;

	// 인덱스를 정점 목록으로 가져옴
	int vertexIndex = index * 3;

	// 정점 목록에서 삼각형의 세 꼭지점 세팅
	float x1 = m_vertexList[vertexIndex].position.x;
	float z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	float x2 = m_vertexList[vertexIndex].position.x;
	float z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	float x3 = m_vertexList[vertexIndex].position.x;
	float z3 = m_vertexList[vertexIndex].position.z;

	// 삼각형 x좌표의 최소값이 노드 안에 있는지 확인
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))	return false;

	// 삼각형 x좌표의 최대값이 노드 안에 있는지 확인
	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))	return false;

	// 삼각형 z좌표의 최소값이 노드 안에 있는지 확인
	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))	return false;

	// 삼각형 z좌표의 최대값이 노드 안에 있는지 확인
	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))	return false;

	return true;
}


void QuadTreeClass::ReleaseNode(NodeType* node)
{
	for (int i = 0; i < 4; i++) {
		if (node->nodes[i] != 0)	ReleaseNode(node->nodes[i]);
	}

	if (node->vertexBuffer) {
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	if (node->indexBuffer) {
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	if (node->vertexArray) {
		delete[] node->vertexArray;
		node->vertexArray = 0;
	}

	for (int i = 0; i < 4; i++) {
		if (node->nodes[i]) {
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}

void QuadTreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
	// 노드가 현재 보이는 곳에 있는지 확인
	if (!frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f)))	return;

	// 보인다면 4개의 자식 모두 확인
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (node->nodes[i] != 0) {
			count++;
			RenderNode(node->nodes[i], frustum, deviceContext, shader);
		}
	}
	if (count != 0)	return;

	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 현재 노드에서 인덱스 수 결정
	int indexCount = node->triangleCount * 3;

	// 지형 셰이더를 호출하여 현재 노드의 다각형 렌더링
	shader->RenderShader(deviceContext, indexCount);

	// 현재 프레임동안 렌더링 된 폴리곤의 수 갱신
	m_drawCount += node->triangleCount;
}

bool QuadTreeClass::GetHeightAtPosition(float positionX, float positionZ, float& height)
{
	float meshMinX = m_parentNode->positionX - (m_parentNode->width / 2.0f);
	float meshMaxX = m_parentNode->positionX + (m_parentNode->width / 2.0f);
	float meshMinZ = m_parentNode->positionZ - (m_parentNode->width / 2.0f);
	float meshMaxZ = m_parentNode->positionZ + (m_parentNode->width / 2.0f);

	// 좌표가 삼각형 위에 있는지 확인
	if (positionX < meshMinX || positionX > meshMaxX || positionZ < meshMinZ || positionZ > meshMaxZ)	return false;

	// 현재 위치에 대한 삼각형을 포함하는 노드 찾기
	FindNode(m_parentNode, positionX, positionZ, height);

	return true;
}

void QuadTreeClass::FindNode(NodeType* node, float x, float z, float& height)
{
	// 현재 노드 크기 계산
	float xMin = node->positionX - (node->width / 2.0f);
	float xMax = node->positionX + (node->width / 2.0f);
	float zMin = node->positionZ - (node->width / 2.0f);
	float zMax = node->positionZ + (node->width / 2.0f);

	// 좌표가 노드 위에 있는지 확인
	if (x < xMin || x > xMax || z < zMin || z > zMax)	return;

	// 자식노드가 있는지 확인
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (node->nodes[i] != 0) {
			count++;
			FindNode(node->nodes[i], x, z, height);
		}
	}

	if (count > 0)	return;

	float vertex1[3] = { 0.0f, 0.0f, 0.0f };
	float vertex2[3] = { 0.0f, 0.0f, 0.0f };
	float vertex3[3] = { 0.0f, 0.0f, 0.0f };

	// 자식이 없으면 삼각형이 노드 안에 있어야 함
	for (int i = 0; i < node->triangleCount; i++) {
		int index = i * 3;

		vertex1[0] = node->vertexArray[index].x;
		vertex1[1] = node->vertexArray[index].y;
		vertex1[2] = node->vertexArray[index].z;

		index++;
		vertex2[0] = node->vertexArray[index].x;
		vertex2[1] = node->vertexArray[index].y;
		vertex2[2] = node->vertexArray[index].z;

		index++;
		vertex3[0] = node->vertexArray[index].x;
		vertex3[1] = node->vertexArray[index].y;
		vertex3[2] = node->vertexArray[index].z;

		// 찾고자 하는 삼각형인지 확인하고, 삼각형이라면 반환
		if (CheckHeightOfTriangle(x, z, height, vertex1, vertex2, vertex3))	return;
	}
}

bool QuadTreeClass::CheckHeightOfTriangle(float x, float z, float& height, float v1[3], float v2[3], float v3[3])
{
	// 벡터 시작 위치
	float startVector[3] = { x, 0.0f, z };

	// 벡터 방향
	float directionVector[3] = { 0.0f, -1.0f, 0.0f };

	// 모서리 계산
	float edge1[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
	float edge2[3] = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2] };

	// 두 모서리에서 삼각형의 법선 계산
	float normal[3] = { (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]), (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]) , (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]) };

	float magnitude = static_cast<float>(sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2])));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// 원점에서 평면까지의 거리
	float distance = (-normal[0] * v1[0]) + (-normal[1] * v1[1]) + (-normal[2] * v1[2]);

	// 방정식의 분모 계산
	float denominator = (normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]);
	if (fabs(denominator) < 0.0001f)	return false;

	// 방정식의 분자 계산
	float numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + distance);

	// 삼각형과 교차하는 위치 계산
	float t = numerator / denominator;

	// 교차 벡터 계산
	float Q[3] = { startVector[0] + (directionVector[0] * t), startVector[1] + (directionVector[1] * t) , startVector[2] + (directionVector[2] * t) };

	// 삼각형의 변 계산
	float e1[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
	float e2[3] = { v3[0] - v2[0], v3[1] - v2[1], v3[2] - v2[2] };
	float e3[3] = { v1[0] - v3[0], v1[1] - v3[1], v1[2] - v3[2] };

	float edgeNormal[3] = { 0.0f, 0.0f, 0.0f };
	float temp[3] = { 0.0f, 0.0f, 0.0f };
	float determinant = 0.0f;

	// 1번째 변에 대한 법선 계산
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// 행렬이 내부, 외부, 변 위에 있는지를 판단하기 위한 판별식 계산
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];
	determinant = (edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]);

	if (determinant > 0.001f)	return false;

	// 2번째 변에 대한 법선 계산
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// 행렬이 내부, 외부, 변 위에 있는지를 판단하기 위한 판별식 계산
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];
	determinant = (edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]);

	if (determinant > 0.001f)	return false;

	// 3번째 변에 대한 법선 계산
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// 행렬이 내부, 외부, 변 위에 있는지를 판단하기 위한 판별식 계산
	temp[0] = Q[0] - v3[0];
	temp[1] = Q[1] - v3[1];
	temp[2] = Q[2] - v3[2];
	determinant = (edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]);

	if (determinant > 0.001f)	return false;

	// 높이 세팅
	height = Q[1];

	return true;
}