#pragma once

class TransparentShaderClass : public AlignedAllocationPolicy<16>
{
private:
	struct ConstantBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct TransparentBufferType
	{
		float blendAmount;
		XMFLOAT3 padding;
	};

	//struct TranslateBufferType
	//{
	//	float translation;
	//	XMFLOAT3 padding;
	//};

	//struct ClipPlaneBufferType
	//{
	//	XMFLOAT4 clipPlane;
	//};

public:
	TransparentShaderClass();
	TransparentShaderClass(const TransparentShaderClass&);
	~TransparentShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11Buffer* m_constantBuffer = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11Buffer* m_transparentBuffer = nullptr;
};