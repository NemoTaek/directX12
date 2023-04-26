#pragma once

class TessellationShaderClass : public AlignedAllocationPolicy<16>
{
private:
	struct ConstantBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct TessellationBufferType
	{
		float tessellationAmount;
		XMFLOAT3 padding;
	};

public:
	TessellationShaderClass();
	TessellationShaderClass(const TessellationShaderClass&);
	~TessellationShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11HullShader* m_hullShader = nullptr;
	ID3D11DomainShader* m_domainShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11Buffer* m_constantBuffer = nullptr;
	ID3D11Buffer* m_tessellationBuffer = nullptr;
};