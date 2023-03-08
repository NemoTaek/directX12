#include "Stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"

#define SENTENCE_MAX_LENGTH 16

TextClass::TextClass() {}
TextClass::TextClass(const TextClass& other) {}
TextClass:: ~TextClass() {}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	// ȭ�� ũ�⸦ ��� ������ ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_baseViewMatrix = baseViewMatrix;

	// ��Ʈ ��ü ���� �� �ʱ�ȭ
	m_font = new FontClass;
	if (!m_font) return false;
	if (!(m_font->Initialize(device, "./data/fontdata.txt", L"./Textures/font.dds"))) {
		MessageBox(hwnd, L"Could not initialize font object", L"Error", MB_OK);
		return false;
	}

	// ��Ʈ ��ü ���� �� �ʱ�ȭ
	m_fontShader = new FontShaderClass;
	if (!m_fontShader) return false;
	if (!(m_fontShader->Initialize(device, hwnd))) {
		MessageBox(hwnd, L"Could not initialize font shader object", L"Error", MB_OK);
		return false;
	}

	// ù��° ���� �ʱ�ȭ �� ������Ʈ
	if (!(InitializeSentence(&m_sentence1, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(UpdateSentence(m_sentence1, "fasdes", 200, 100, 1.0f, 0.0f, 1.0f, deviceContext)))	return false;

	// �ι�° ���� �ʱ�ȭ �� ������Ʈ
	if (!(InitializeSentence(&m_sentence2, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(UpdateSentence(m_sentence2, "6544658465", 300, 200, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	// ����° ���� �ʱ�ȭ �� ������Ʈ
	if (!(InitializeSentence(&m_sentence3, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(UpdateSentence(m_sentence3, "6544658465", 400, 200, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	// ����° ���� �ʱ�ȭ �� ������Ʈ
	if (!(InitializeSentence(&m_sentence4, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(UpdateSentence(m_sentence4, "4532342", 400, 200, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	// ����° ���� �ʱ�ȭ �� ������Ʈ
	if (!(InitializeSentence(&m_sentence5, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(UpdateSentence(m_sentence5, "12342", 400, 200, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	return true;
}

void TextClass::Shutdown()
{
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);

	if (m_fontShader) {
		m_fontShader->Shutdown();
		delete m_fontShader;
		m_fontShader = 0;
	}

	if (m_font) {
		m_font->Shutdown();
		delete m_font;
		m_font = 0;
	}
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	if (!RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence4, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence5, worldMatrix, orthoMatrix))	return false;

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char mouseString[SENTENCE_MAX_LENGTH];

	// ���콺 X��ġ ������ ���ڿ��� ����
	_itoa_s(mouseX, tempString, 10);

	// ���콺 X ���ڿ� ����
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// ������ ���콺 ���ڿ��� ������Ʈ
	if (!(UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	// ���콺 Y��ġ ������ ���ڿ��� ����
	_itoa_s(mouseY, tempString, 10);

	// ���콺 Y ���ڿ� ����
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// ������ ���콺 ���ڿ��� ������Ʈ
	if (!(UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetKeyboardInput(int keyCount, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char keyboardString[SENTENCE_MAX_LENGTH];

	// ī��Ʈ ������ ���ڿ��� ����
	_itoa_s(keyCount, tempString, 10);

	// ī��Ʈ ���ڿ� ����
	strcpy_s(keyboardString, "count: ");
	strcat_s(keyboardString, tempString);

	// ������ Ű���� ī��Ʈ ������ ������Ʈ
	if (!(UpdateSentence(m_sentence3, keyboardString, 20, 60, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	// FPS�� 10000 �̸����� ����
	if (fps > 9999)	fps = 9999;

	char tempString[SENTENCE_MAX_LENGTH];
	char fpsString[SENTENCE_MAX_LENGTH];
	float red = 0;
	float green = 0;
	float blue = 0;

	// FPS ������ ���ڿ��� ����
	_itoa_s(fps, tempString, 10);

	// FPS ���ڿ� ����
	strcpy_s(fpsString, "FPS: ");
	strcat_s(fpsString, tempString);

	// fps ���� ���� ��Ʈ �� ����
	if (fps < 30) {
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}
	else if (fps < 60) {
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	else {
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// ������ FPS ������ ������Ʈ
	if (!(UpdateSentence(m_sentence4, fpsString, 20, 80, red, green, blue, deviceContext)))	return false;

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char cpuString[SENTENCE_MAX_LENGTH];

	// CPU ������ ���ڿ��� ����
	_itoa_s(cpu, tempString, 10);

	// CPU ���ڿ� ����
	strcpy_s(cpuString, "CPU: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// ������ CPU ������ ������Ʈ
	if (!(UpdateSentence(m_sentence5, cpuString, 20, 100, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// SentenceType ��ü ���� �� �ʱ�ȭ
	*sentence = new SentenceType;
	if (!*sentence)	return false;
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;
	(*sentence)->maxLength = maxLength;
	(*sentence)->vertexCount = 6 * maxLength;	// �ѱ��ڿ� �ﰢ�� 2�� �� ���� 6�� �ʿ�
	(*sentence)->indexCount = (*sentence)->vertexCount;

	int vCount = (*sentence)->vertexCount;
	int iCount = (*sentence)->indexCount;

	// ���� �迭 ���� �� �ʱ�ȭ
	VertexType* vertices = new VertexType[vCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * vCount));

	// �ε��� �迭 ����
	unsigned long* indices = new unsigned long[iCount];
	if (!indices)	return false;
	for (int i = 0; i < iCount; i++)	indices[i] = i;

	// ���� ���� ������ ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))	return false;

	// �ε��� ���� ������ ����
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * iCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))	return false;

	// ���� �� ���� �� �ε��� ���� ����
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;
	int vCount = sentence->vertexCount;
	int iCount = sentence->indexCount;
	float drawX, drawY;

	// ���� ����
	int numLetters = static_cast<int>(strlen(text));
	if (numLetters > sentence->maxLength)	return true;

	// ���� ���� �ڿ��� �ڷḦ �ø� �� �ֵ��� ������ ����
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	VertexType* verticesPtr;

	// ���� �迭 ���� �� �ʱ�ȭ
	vertices = new VertexType[vCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * vCount));

	// ��Ʈ�� ��ǥ ���
	// ���߾��� (0, 0)���� �ξ� �����¿� ���
	drawX = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);
	drawY = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);

	// ���� �ؽ�Ʈ�� ��ġ�� ����Ͽ� ���� �迭�� �����ϱ� ���� ��Ʈ Ŭ���� ���
	m_font->BuildVertexArray(reinterpret_cast<void*>(vertices), text, drawX, drawY);

	// ���� ���� ����
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;

	// ���� ���ۿ� ������ ����
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vCount));

	// ���� ���� �޸� ����
	deviceContext->Unmap(sentence->vertexBuffer, 0);
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence) {
		if ((*sentence)->vertexBuffer) {
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}
		if ((*sentence)->indexBuffer) {
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}
		delete* sentence;
		*sentence = nullptr;
	}
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// ���� ������ ������ ������ ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� �� �ε��� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �ȼ� ���� ���͸� ���� �������� ����
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// ��Ʈ ���̴��� �ؽ�Ʈ ������
	if (!(m_fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_font->GetTexture(), pixelColor)))	return false;

	return true;
}