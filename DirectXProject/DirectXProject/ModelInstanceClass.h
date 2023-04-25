#pragma once

class TextureClass;

class ModelInstanceClass : public AlignedAllocationPolicy<16>
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	struct InstanceType
	{
		XMFLOAT3 position;
	};

public:
	ModelInstanceClass();
	ModelInstanceClass(const ModelInstanceClass&);
	~ModelInstanceClass();

	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	// 정점과 인스턴스 개수를 가져와 같은 모델을 여러개 만들 수 있음
	// InstanceType의 position으로 위치를 지정함
	int GetVertexCount();
	int GetInstanceCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_instanceBuffer = nullptr;
	int m_vertexCount = 0;
	int m_instanceCount = 0;
	TextureClass* m_texture = nullptr;
};