#pragma once

class TextureClass;

class FontClass
{
private:
	// �۲� �ε��� ������ ������
	struct FontType
	{
		float left, right;	// �ؽ��� U ��ǥ
		int size;			// �ȼ� �ʺ�
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, const char*, const WCHAR*);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, const char*, float, float);

private:
	bool LoadFontData(const char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_font = nullptr;
	TextureClass* m_texture = nullptr;
};
