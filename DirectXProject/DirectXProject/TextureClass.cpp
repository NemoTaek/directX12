#include "Stdafx.h"
#include "TextureClass.h"
#include <stdio.h>

#include "DDSTextureLoader.h"

TextureClass::TextureClass() {}
TextureClass::TextureClass(const TextureClass& other) {}
TextureClass::~TextureClass() {}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* filename)
{
	// 이부분은 targa 파일을 텍스쳐로 사용 할 시 사용
	/*
	int width = 0;
	int height = 0;

	// targa 이미지 데이터를 메모리에 로드
	if (!LoadTarga(filename, width, height))	return false;

	// 텍스쳐 구조체 설정
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

	// 빈 텍스쳐 생성
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_texture)))	return false;

	// targa 이미지 데이터의 너비 사이즈 설정
	UINT rowPitch = width * 4 * sizeof(unsigned char);

	// targa 이미지 데이터를 텍스쳐에 복사
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// SRV 구조체 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스쳐의 SRV 생성
	if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView)))	return false;

	// 이 텍스쳐에 대한 밉맵 생성
	deviceContext->GenerateMips(m_textureView);

	// 이미지 데이터가 텍스쳐에 로드되었으므로 targa 이미지 데이터를 해제
	delete[] m_targaData;
	m_targaData = 0;
	*/


	// 여기서부터는 dds 파일을 텍스쳐로 사용할 시 사용
	if(FAILED(CreateDDSTextureFromFile(device, filename, &m_texture, &m_textureView)))	return false;

	return true;
}

void TextureClass::Shutdown()
{
	// 텍스쳐 뷰 리소스 해제
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// 텍스쳐 해제
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa 이미지 데이터 해제
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
	// targa 파일을 바이너리 모드로 파일을 오픈
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)	return false;

	// 파일 헤더 로드
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)	return false;

	width = (int)targaFileHeader.width;
	height = (int)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;
	if (bpp != 32)	return false;

	// 32비트 이미지 데이터 크기 계산
	int imageSize = width * height * 4;

	// targa 이미지 데이터 메모리 할당
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)	return false;

	// targa 이미지 데이터 로드
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)	return false;

	// 파일 닫기
	if (fclose(filePtr) != 0) return false;

	// targa 대상 데이터에 대한 메모리 할당
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)	return false;

	// targa 대상 데이터 배열의 인덱스 초기화
	int index = 0;

	// targa 이미지 데이터 인덱스 초기화
	int k = imageSize - (width * 4);

	// targa 형식이 거꾸로 저장되었으므로 올바른 순서로 targa 이미지 데이터를 targa 대상 배열에 복사
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			m_targaData[index + 0] = targaImage[k + 2];	// R
			m_targaData[index + 1] = targaImage[k + 1];	// G
			m_targaData[index + 2] = targaImage[k + 0];	// B
			m_targaData[index + 3] = targaImage[k + 3];	// A

			// 인덱스를 targa 데이터로 증가
			k += 4;
			index += 4;
		}
		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작부분에서 이전 행으로 다시 설정
		k -= (width * 8);
	}

	// targa 이미지 데이터 해제
	delete[] targaImage;
	targaImage = 0;

	return true;
}
*/