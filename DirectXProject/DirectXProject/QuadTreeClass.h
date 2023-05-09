#pragma once

const int MAX_TRIANGLES = 10000;

class TerrainClass;
class TerrainShaderClass;
class FrustumClass;

// 쿼드 트리 파티션 알고리즘을 사용하여 현재 보여지는 지형만 렌더링하도록 설정
// 전체 지형을 4등분 하고 이를 여기서 정한 최대 삼각형 수가 될 때까지 반복하여 분할
// 여기서 어떤 삼각형이 현재 시야에 있는지를 파악하고, 이에 대한 정점 버퍼를 만들고, 삼각형을 채움
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