#pragma once

class TextureClass
{
//private:
//	struct TargaHeader
//	{
//		unsigned char data1[12];
//		unsigned short width;
//		unsigned short height;
//		unsigned char bpp;
//		unsigned char data2;
//	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	//bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const WCHAR*);
	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	//bool LoadTarga(char*, int&, int&);

private:
	//unsigned char* m_targaData = nullptr;
	/*ID3D11Texture2D* m_texture = nullptr;*/
	ID3D11Resource* m_texture = nullptr;
	ID3D11ShaderResourceView* m_textureView = nullptr;
};