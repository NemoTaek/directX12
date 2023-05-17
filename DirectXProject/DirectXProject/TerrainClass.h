#pragma once

const int TEXTURE_REPEAT = 32;
class TextureClass;
class TerrainShaderClass;

class TerrainClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;	// �Ϲ� �ؽ��Ĵ� XMFLOAT2, ������ �ؽ��Ĵ� XMFLOAT4
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		//XMFLOAT4 color;
		XMFLOAT2 texture2;
	};

	struct HeightMapType
	{
		float x, y, z;
		//float tu, tv;
		float nx, ny, nz;
		//float r, g, b;
		//int rIndex, gIndex, bIndex;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		//float r, g, b;
		float tu2, tv2;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
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
	bool Initialize(ID3D11Device*, const char*, float);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool RenderMaterials(ID3D11DeviceContext*, TerrainShaderClass*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	// ������ ��
	ID3D11ShaderResourceView* GetDetailMapTexture();

	// ��� �ؽ�ó
	ID3D11ShaderResourceView* GetFlatTexture();
	ID3D11ShaderResourceView* GetSlopeTexture();
	ID3D11ShaderResourceView* GetScarpTexture();

	// ���� ��
	ID3D11ShaderResourceView* GetColorTexture();
	ID3D11ShaderResourceView* GetNormalTexture();

	void GetTerrainSize(int&, int&);
	int GetVertexCount();
	void CopyVertexArray(void*);

private:
	bool LoadHeightMap(const char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ReduceHeightMap(float);
	void ReleaseHeightMap();

	// ���� �ؽ�ó
	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, const WCHAR*, const WCHAR*);
	void ReleaseTexture();

	// ���� �÷� ����
	bool LoadColorMap(const char*);

	bool BuildModel();
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void ReleaseModel();

	// ���͸��� ����
	bool LoadMaterialFile(const char*, const char*, ID3D11Device*);
	bool LoadMaterialMap(const char*);
	bool LoadMaterialBuffers(ID3D11Device*);
	void ReleaseMaterials();

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
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
	TextureClass* m_detailTexture = nullptr;
	TextureClass* m_flatTexture = nullptr;
	TextureClass* m_slopeTexture = nullptr;
	TextureClass* m_scarpTexture = nullptr;
	VertexType* m_vertices = nullptr;

	ModelType* m_model = nullptr;
	TextureClass* m_colorTexture = nullptr;
	TextureClass* m_normalTexture = nullptr;

	int m_textureCount = 0;
	int m_materialCount = 0;
	MaterialGroupType* m_material = nullptr;
};