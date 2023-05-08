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

	// ���� ���� �迭�� ���� ���� �����ɴϴ�.
	int vertexCount = terrain->GetVertexCount();

	// ��������Ʈ�� �� �ﰢ�� ���� �����մϴ�.
	m_triangleCount = vertexCount / 3;

	// ��� ���� ������ �����ϴ� ���� �迭�� ����ϴ�.
	m_vertexList = new VertexType[vertexCount];
	if (!m_vertexList)
	{
		return false;
	}

	// ���� ������ ���� ��Ͽ� �����մϴ�.
	terrain->CopyVertexArray((void*)m_vertexList);

	// �߽� x, z�� �޽��� �ʺ� ����մϴ�.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// ���� Ʈ���� �θ� ��带 ����ϴ�.
	m_parentNode = new NodeType;
	if (!m_parentNode)
	{
		return false;
	}

	// ���� ��� �����Ϳ� �޽� ������ ������� ���� Ʈ���� ��� ������ �����մϴ�.
	CreateTreeNode(m_parentNode, centerX, centerZ, width, device);

	// ���� Ʈ���� �� ��忡 ������ ���� ������ ���� ����� �����ϴ�.
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}

	return true;
}


void QuadTreeClass::Shutdown()
{
	// ���� Ʈ�� �����͸� ��� ������ �����մϴ�.
	if (m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}
}


void QuadTreeClass::Render(FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
	// �� �����ӿ� ���� �׷����� �ﰢ���� ���� �ʱ�ȭ�մϴ�.
	m_drawCount = 0;

	// �θ� ��忡�� �����Ͽ� Ʈ�� �Ʒ��� �̵��Ͽ� ���̴� �� ��带 �������մϴ�.
	RenderNode(m_parentNode, frustum, deviceContext, shader);
}


int QuadTreeClass::GetDrawCount()
{
	return m_drawCount;
}


void QuadTreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	// �޽��� �߽� ��ġ�� 0���� �ʱ�ȭ�մϴ�.
	centerX = 0.0f;
	centerZ = 0.0f;

	// �޽��� ��� ������ ��ģ��.
	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	// �׸��� �޽��� �߰� ���� ã�� ���� ������ ���� �����ϴ�.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// �޽��� �ִ� �� �ּ� ũ�⸦ �ʱ�ȭ�մϴ�.
	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_vertexList[0].position.x - centerX);
	float minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	// ��� ������ ���캸�� �޽��� �ִ� �ʺ�� �ּ� �ʺ�� ���̸� ã���ϴ�.
	for (int i = 0; i < vertexCount; i++)
	{
		float width = fabsf(m_vertexList[i].position.x - centerX);
		float depth = fabsf(m_vertexList[i].position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// �ּҿ� �ִ� ���̿� �ʺ� ������ ���� �ִ� ���� ã���ϴ�.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// �޽��� �ִ� ������ ����մϴ�.
	meshWidth = max(maxX, maxZ) * 2.0f;
}


void QuadTreeClass::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	// ����� ��ġ�� ũ�⸦ �����Ѵ�.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// ����� �ﰢ�� ���� 0���� �ʱ�ȭ�մϴ�.
	node->triangleCount = 0;

	//���� �� �ε��� ���۸� null�� �ʱ�ȭ�մϴ�.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	// �� ����� �ڽ� ��带 null�� �ʱ�ȭ�մϴ�.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// �� ��� �ȿ� �ִ� �ﰢ�� ���� ����.
	int numTriangles = CountTriangles(positionX, positionZ, width);

	// ��� 1: �� ��忡 �ﰢ���� ������ ����ִ� ���·� ���ư��� ó���� �ʿ䰡 �����ϴ�.
	if (numTriangles == 0)
	{
		return;
	}

	// ��� 2: �� ��忡 �ʹ� ���� �ﰢ���� �ִ� ��� 4 ���� ������ ũ���� �� ���� Ʈ�� ���� �����մϴ�.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
			// ���ο� �ڽ� ��忡 ���� ��ġ �������� ����մϴ�.
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// �� ��忡 �ﰢ���� �ִ��� Ȯ���մϴ�.
			int count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// �� �� ��尡�ִ� �ﰢ�����ִ� ��� �ڽ� ��带 ����ϴ�.
				node->nodes[i] = new NodeType;

				// ������ �� �ڽ� ��忡�� �����ϴ� Ʈ���� Ȯ���մϴ�.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
			}
		}
		return;
	}

	// ��� 3: �� ��尡 ��� �����ʰ� �� ����� �ﰢ�� ���� �ִ� ������ ������ 
	// �� ���� Ʈ���� �� �Ʒ��� �����Ƿ� ������ �ﰢ�� ����� ����ϴ�.
	node->triangleCount = numTriangles;

	// ������ ���� ����մϴ�.
	int vertexCount = numTriangles * 3;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[vertexCount];

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[vertexCount];

	// �� ���ο� ���� �� �ε��� �迭�� �ε����� �ʱ�ȭ�մϴ�.
	int index = 0;
	int vertexIndex = 0;

	// ���� ����� ��� �ﰢ���� ���� ���ϴ�.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// �ﰢ������ ��� �ȿ� ������ ������ �迭�� �߰��մϴ�.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			// ���� ���ؽ� ��Ͽ� �ε����� ����մϴ�.
			vertexIndex = i * 3;

			// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����ɴϴ�.
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			vertices[index].normal = m_vertexList[vertexIndex].normal;
			indices[index] = index;
			index++;
		}
	}

	// ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ��ħ�� ���� ���۸� ����ϴ�.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

	// �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	// ���� ����� ���ۿ� �����Ͱ� ����ǹǷ� �������� �ε��� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}


int QuadTreeClass::CountTriangles(float positionX, float positionZ, float width)
{
	// ī��Ʈ�� 0���� �ʱ�ȭ�Ѵ�.
	int count = 0;

	// ��ü �޽��� ��� �ﰢ���� ���캸�� � ��尡 �� ��� �ȿ� �־�� �ϴ��� Ȯ���մϴ�.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// �ﰢ���� ��� �ȿ� ������ 1�� ������ŵ�ϴ�.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			count++;
		}
	}

	return count;
}


bool QuadTreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// �� ����� �ݰ��� ����մϴ�.
	float radius = width / 2.0f;

	// �ε����� ���� ������� �����ɴϴ�.
	int vertexIndex = index * 3;

	// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����ɴϴ�.
	float x1 = m_vertexList[vertexIndex].position.x;
	float z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	float x2 = m_vertexList[vertexIndex].position.x;
	float z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	float x3 = m_vertexList[vertexIndex].position.x;
	float z3 = m_vertexList[vertexIndex].position.z;

	// �ﰢ���� x ��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
	{
		return false;
	}

	// �ﰢ���� x ��ǥ�� �ִ� ���� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
	{
		return false;
	}

	// �ﰢ���� z ��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))
	{
		return false;
	}

	// �ﰢ���� z ��ǥ�� �ִ� ���� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}


void QuadTreeClass::ReleaseNode(NodeType* node)
{
	// ��� ������ Ʈ�� �Ʒ��� ������ �� �Ʒ� ��带 ���� �����ϴ�.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// �� ����� ���ؽ� ���۸� �����Ѵ�.
	if (node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	// �� ����� �ε��� ���۸� �����մϴ�.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// �� ���� �ڽ� ��带 �����մϴ�.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}

void QuadTreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader)
{
	// ��尡 ���� ���̴� ���� �ִ��� Ȯ��
	if (!frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f)))	return;

	// ���δٸ� 4���� �ڽ� ��� Ȯ��
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (node->nodes[i] != 0) {
			count++;
			RenderNode(node->nodes[i], frustum, deviceContext, shader);
		}
	}
	if (count != 0)	return;

	// ���� ������ ������ ������ ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� �� �ε��� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���� ��忡�� �ε��� �� ����
	int indexCount = node->triangleCount * 3;

	// ���� ���̴��� ȣ���Ͽ� ���� ����� �ٰ��� ������
	shader->RenderShader(deviceContext, indexCount);

	// ���� �����ӵ��� ������ �� �������� �� ����
	m_drawCount += node->triangleCount;
}