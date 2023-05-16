#pragma once

class TextureClass;

class WaterClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	WaterClass();
	WaterClass(const WaterClass&);
	~WaterClass();

	bool Initialize(ID3D11Device*, const WCHAR*, float, float);
	void Shutdown();
	void Frame();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	float GetWaterHeight();
	XMFLOAT2 GetNormalMapTiling();
	float GetWaterTranslation();
	float GetReflectRefractScale();
	XMFLOAT4 GetRefractionTint();
	float GetSpecularShiness();

private:
	bool InitializeBuffers(ID3D11Device*, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

private:
	float m_waterHeight = 0.0f;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	TextureClass* m_texture = nullptr;
	XMFLOAT2 m_normalMapTiling;
	float m_waterTranslation = 0.0f;
	float m_reflectRefractScale = 0.0f;
	XMFLOAT4 m_refractionTint;
	float m_specularShiness = 0.0f;
};