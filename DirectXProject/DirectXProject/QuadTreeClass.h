#pragma once

const int MAX_TRIANGLES = 10000;

class TerrainClass;
class TerrainShaderClass;
class FrustumClass;

// ���� Ʈ�� ��Ƽ�� �˰����� ����Ͽ� ���� �������� ������ �������ϵ��� ����
// ��ü ������ 4��� �ϰ� �̸� ���⼭ ���� �ִ� �ﰢ�� ���� �� ������ �ݺ��Ͽ� ����
// ���⼭ � �ﰢ���� ���� �þ߿� �ִ����� �ľ��ϰ�, �̿� ���� ���� ���۸� �����, �ﰢ���� ä��
class QuadTreeClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		NodeType* nodes[4];
		VectorType* vertexArray;
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(TerrainClass*, ID3D11Device*);
	void Shutdown();
	void Render(FrustumClass*, ID3D11DeviceContext*, TerrainShaderClass*);
	int GetDrawCount();
	bool GetHeightAtPosition(float, float, float&);

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, FrustumClass*, ID3D11DeviceContext*, TerrainShaderClass*);

	void FindNode(NodeType*, float, float, float&);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	int m_triangleCount;
	int m_drawCount;
	VertexType* m_vertexList = nullptr;
	NodeType* m_parentNode = nullptr;
};