#include "Stdafx.h"
#include "TextureClass.h"
#include "FontClass.h"

#include <fstream>
using namespace std;

#define FONT_WIDTH 1024
#define FONT_HEIGHT 16
#define ASCII_START 32
#define ASCII_NUM 95

FontClass::FontClass() {}
FontClass::FontClass(const FontClass& other) {}
FontClass::~FontClass() {}

bool FontClass::Initialize(ID3D11Device* device, const char* fontFilename, const WCHAR* textureFilename)
{
	if (!LoadFontData(fontFilename))	return false;
	
	return LoadTexture(device, textureFilename);
}

void FontClass::Shutdown()
{
	ReleaseTexture();
	ReleaseFontData();
}

ID3D11ShaderResourceView* FontClass::GetTexture() { return m_texture->GetTexture(); }

void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	// 정점 배열을 가리키는 포인터
	VertexType* vertexPtr = reinterpret_cast<VertexType*>(vertices);

	// 문장에 필요한 글자 수
	int numLetters = static_cast<int>(strlen(sentence));

	// 정점 및 인덱스 배열 생성
	int index = 0;
	for (int i = 0; i < numLetters; i++) {
		// 아스키코드 문자 번호
		int letter = static_cast<int>(sentence[i] - ASCII_START);

		// 첫 문자(스페이스)면 3픽셀 이동
		if (letter == 0) { drawX += 3.0f; }
		else {
			// 좌상
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			// 우하
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// 좌하
			vertexPtr[index].position = XMFLOAT3(drawX, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			index++;

			// 좌상
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			// 우상
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			index++;

			// 우하
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// 다음 글자의 x 위치를 위해 1픽셀 이동
			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}
}

bool FontClass::LoadFontData(const char* filename)
{
	ifstream fin;
	char temp;

	// 폰트 버퍼 객체 생성
	m_font = new FontType[ASCII_NUM];
	if (!m_font) return false;

	fin.open(filename);
	if (fin.fail())	return false;

	// 문자 정보 저장
	for (int i = 0; i < ASCII_NUM; i++) {
		fin.get(temp);
		while (temp != ' ')	fin.get(temp);
		fin.get(temp);
		while (temp != ' ')	fin.get(temp);

		fin >> m_font[i].left;
		fin >> m_font[i].right;
		fin >> m_font[i].size;
	}

	fin.close();
	return true;
}

void FontClass::ReleaseFontData()
{
	if (m_font) {
		delete[] m_font;
		m_font = 0;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_texture = new TextureClass;
	if (!m_texture) return false;
	return m_texture->Initialize(device, filename);
}

void FontClass::ReleaseTexture()
{
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}