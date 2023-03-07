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
	// ���� �迭�� ����Ű�� ������
	VertexType* vertexPtr = reinterpret_cast<VertexType*>(vertices);

	// ���忡 �ʿ��� ���� ��
	int numLetters = static_cast<int>(strlen(sentence));

	// ���� �� �ε��� �迭 ����
	int index = 0;
	for (int i = 0; i < numLetters; i++) {
		// �ƽ�Ű�ڵ� ���� ��ȣ
		int letter = static_cast<int>(sentence[i] - ASCII_START);

		// ù ����(�����̽�)�� 3�ȼ� �̵�
		if (letter == 0) { drawX += 3.0f; }
		else {
			// �»�
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			// ����
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// ����
			vertexPtr[index].position = XMFLOAT3(drawX, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			index++;

			// �»�
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			// ���
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			index++;

			// ����
			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY - FONT_HEIGHT, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// ���� ������ x ��ġ�� ���� 1�ȼ� �̵�
			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}
}

bool FontClass::LoadFontData(const char* filename)
{
	ifstream fin;
	char temp;

	// ��Ʈ ���� ��ü ����
	m_font = new FontType[ASCII_NUM];
	if (!m_font) return false;

	fin.open(filename);
	if (fin.fail())	return false;

	// ���� ���� ����
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