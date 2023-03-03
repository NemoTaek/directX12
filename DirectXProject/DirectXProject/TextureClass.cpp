#include "Stdafx.h"
#include "TextureClass.h"
#include <stdio.h>

#include "DDSTextureLoader.h"

TextureClass::TextureClass() {}
TextureClass::TextureClass(const TextureClass& other) {}
TextureClass::~TextureClass() {}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* filename)
{
	// �̺κ��� targa ������ �ؽ��ķ� ��� �� �� ���
	/*
	int width = 0;
	int height = 0;

	// targa �̹��� �����͸� �޸𸮿� �ε�
	if (!LoadTarga(filename, width, height))	return false;

	// �ؽ��� ����ü ����
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �� �ؽ��� ����
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_texture)))	return false;

	// targa �̹��� �������� �ʺ� ������ ����
	UINT rowPitch = width * 4 * sizeof(unsigned char);

	// targa �̹��� �����͸� �ؽ��Ŀ� ����
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// SRV ����ü ����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ����� SRV ����
	if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView)))	return false;

	// �� �ؽ��Ŀ� ���� �Ӹ� ����
	deviceContext->GenerateMips(m_textureView);

	// �̹��� �����Ͱ� �ؽ��Ŀ� �ε�Ǿ����Ƿ� targa �̹��� �����͸� ����
	delete[] m_targaData;
	m_targaData = 0;
	*/


	// ���⼭���ʹ� dds ������ �ؽ��ķ� ����� �� ���
	if(FAILED(CreateDDSTextureFromFile(device, filename, &m_texture, &m_textureView)))	return false;

	return true;
}

void TextureClass::Shutdown()
{
	// �ؽ��� �� ���ҽ� ����
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// �ؽ��� ����
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa �̹��� ������ ����
	//if (m_targaData)
	//{
	//	delete[] m_targaData;
	//	m_targaData = 0;
	//}
}

ID3D11ShaderResourceView* TextureClass::GetTexture() { return m_textureView; }

/*
bool TextureClass::LoadTarga(char* filename, int& width, int& height)
{
	// targa ������ ���̳ʸ� ���� ������ ����
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)	return false;

	// ���� ��� �ε�
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)	return false;

	width = (int)targaFileHeader.width;
	height = (int)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;
	if (bpp != 32)	return false;

	// 32��Ʈ �̹��� ������ ũ�� ���
	int imageSize = width * height * 4;

	// targa �̹��� ������ �޸� �Ҵ�
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)	return false;

	// targa �̹��� ������ �ε�
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)	return false;

	// ���� �ݱ�
	if (fclose(filePtr) != 0) return false;

	// targa ��� �����Ϳ� ���� �޸� �Ҵ�
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)	return false;

	// targa ��� ������ �迭�� �ε��� �ʱ�ȭ
	int index = 0;

	// targa �̹��� ������ �ε��� �ʱ�ȭ
	int k = imageSize - (width * 4);

	// targa ������ �Ųٷ� ����Ǿ����Ƿ� �ùٸ� ������ targa �̹��� �����͸� targa ��� �迭�� ����
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			m_targaData[index + 0] = targaImage[k + 2];	// R
			m_targaData[index + 1] = targaImage[k + 1];	// G
			m_targaData[index + 2] = targaImage[k + 0];	// B
			m_targaData[index + 3] = targaImage[k + 3];	// A

			// �ε����� targa �����ͷ� ����
			k += 4;
			index += 4;
		}
		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���ۺκп��� ���� ������ �ٽ� ����
		k -= (width * 8);
	}

	// targa �̹��� ������ ����
	delete[] targaImage;
	targaImage = 0;

	return true;
}
*/