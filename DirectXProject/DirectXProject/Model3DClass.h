#pragma once

class TextureArrayClass;

class Model3DClass : public AlignedAllocationPolicy<16>
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		//XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;		// À§Ä¡º¤ÅÍ
		float tu, tv;		// ÅØ½ºÃÄ ÁÂÇ¥
		float nx, ny, nz;	// ¹ý¼±º¤ÅÍ
	};

public:
	Model3DClass();
	Model3DClass(const Model3DClass&);
	~Model3DClass();

	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	int GetIndexCount();
	/*ID3D11ShaderResourceView* GetTexture();*/
	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	void ReleaseTextures();

	bool LoadModel(const WCHAR*);
	void ReleaseModel();

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	//TextureClass* m_Texture = nullptr;
	TextureArrayClass* m_textureArray = nullptr;
	ModelType* m_model3D = nullptr;
};