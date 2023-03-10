#include "Stdafx.h"
#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass() {}
TextureArrayClass::TextureArrayClass(const TextureArrayClass& other) {}
TextureArrayClass::~TextureArrayClass() {}

bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_textures[0])))	return false;
	if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_textures[1])))	return false;
	if (FAILED(CreateDDSTextureFromFile(device, filename3, nullptr, &m_textures[2])))	return false;
	return true;
}

void TextureArrayClass::Shutdown()
{
	if (m_textures[0]) {
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if (m_textures[1]) {
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	if (m_textures[2]) {
		m_textures[2]->Release();
		m_textures[2] = 0;
	}
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray() { return m_textures; }