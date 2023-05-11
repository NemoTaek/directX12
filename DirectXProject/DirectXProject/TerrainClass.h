#pragma once

const int TEXTURE_REPEAT = 8;
class TextureClass;
class TerrainShaderClass;

class TerrainClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT4 color;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float r, g, b;
		//int rIndex, gIndex, bIndex;
	};

	struct VectorType
	{
		float x, y, z;
	};

	// MaterialGroup: material�� vertex / index ���� ����
	struct MaterialGroupType
	{
		int textureIndex1, textureIndex2, alphaIndex;
		int red, green, blue;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount, indexCount;
		VertexType* vertices;
		unsigned long* indices;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	// ���� �ؽ�ó ��� ���͸��� ���
	bool Initialize(ID3D11Device*, const char*, const WCHAR*, const char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool RenderMaterials(ID3D11DeviceContext*, TerrainShaderClass*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	void GetTerrainSize(int&, int&);
	int GetVertexCount();
	void CopyVertexArray(void*);

private:
	bool LoadHeightMap(const char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	// ���� �ؽ�ó
	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	// ���� �÷� ����
	bool LoadColorMap(const char*);

	// ���͸��� ����
	bool LoadMaterialFile(const char*, const char*, ID3D11Device*);
	bool LoadMaterialMap(const char*);
	bool LoadMaterialBuffers(ID3D11Device*);
	void ReleaseMaterials();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	int m_terrainWidth = 0;
	int m_terrainHeight = 0;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	HeightMapType* m_heightMap = nullptr;
	TextureClass* m_texture = nullptr;
	VertexType* m_vertices = nullptr;

	int m_textureCount = 0;
	int m_materialCount = 0;
	MaterialGroupType* m_material = nullptr;
};