#include "Stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"

#define SENTENCE_MAX_LENGTH 32

TextClass::TextClass() {}
TextClass::TextClass(const TextClass& other) {}
TextClass:: ~TextClass() {}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	// 화면 크기를 멤버 변수에 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_baseViewMatrix = baseViewMatrix;

	// 폰트 객체 생성 및 초기화
	m_font = new FontClass;
	if (!m_font) return false;
	if (!(m_font->Initialize(device, "./data/fontdata.txt", L"./Textures/font.dds"))) {
		MessageBox(hwnd, L"Could not initialize font object", L"Error", MB_OK);
		return false;
	}

	// 문장 초기화 및 업데이트
	if (!(InitializeSentence(&m_sentence1, 150, device)))	return false;
	if (!(InitializeSentence(&m_sentence2, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence3, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence4, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence5, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence6, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence7, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence8, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence9, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence10, SENTENCE_MAX_LENGTH, device)))	return false;
	if (!(InitializeSentence(&m_sentence11, SENTENCE_MAX_LENGTH, device)))	return false;

	return true;
}

void TextClass::Shutdown()
{
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);
	ReleaseSentence(&m_sentence9);
	ReleaseSentence(&m_sentence10);
	ReleaseSentence(&m_sentence11);

	if (m_font) {
		m_font->Shutdown();
		delete m_font;
		m_font = 0;
	}
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, FontShaderClass* fontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	if (!RenderSentence(deviceContext, m_sentence1, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence2, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence3, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence4, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence5, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence6, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence7, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence8, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence9, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence10, fontShader, worldMatrix, orthoMatrix))	return false;
	if (!RenderSentence(deviceContext, m_sentence11, fontShader, worldMatrix, orthoMatrix))	return false;

	return true;
}

bool TextClass::SetVideoCardInfo(const char* videoCardName, int videoCardMemory, ID3D11DeviceContext* deviceContext)
{
	char dataString[150] = { 0, };
	char tempString[16] = { 0, };
	char memoryString[32] = { 0, };

	// 비디오 카드 이름 문자열을 설정합니다.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if (!UpdateSentence(m_sentence1, dataString, 0, 10, 1.0f, 1.0f, 1.0f, deviceContext))	return false;

	// 버퍼 오버플로우를 막기 위해 메모리 값을 자릅니다.
	if (videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// 비디오 메모리 정수 값을 문자열 형식으로 변환합니다.
	_itoa_s(videoCardMemory, tempString, 10);

	// 비디오 메모리 문자열을 설정합니다.
	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if (!UpdateSentence(m_sentence2, memoryString, 0, 30, 1.0f, 1.0f, 1.0f, deviceContext))	return false;

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char mouseString[SENTENCE_MAX_LENGTH];

	// 마우스 X위치 정수를 문자열로 변경
	_itoa_s(mouseX, tempString, 10);

	// 마우스 X 문자열 생성
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// 문장을 마우스 문자열로 업데이트
	if (!(UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	// 마우스 Y위치 정수를 문자열로 변경
	_itoa_s(mouseY, tempString, 10);

	// 마우스 Y 문자열 생성
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// 문장을 마우스 문자열로 업데이트
	if (!(UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetKeyboardInput(int keyCount, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char keyboardString[SENTENCE_MAX_LENGTH];

	// 카운트 정수를 문자열로 변경
	_itoa_s(keyCount, tempString, 10);

	// 카운트 문자열 생성
	strcpy_s(keyboardString, "count: ");
	strcat_s(keyboardString, tempString);

	// 문장을 키보드 카운트 값으로 업데이트
	if (!(UpdateSentence(m_sentence3, keyboardString, 20, 60, 1.0f, 1.0f, 1.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	// FPS를 10000 미만으로 제한
	if (fps > 9999)	fps = 9999;

	char tempString[SENTENCE_MAX_LENGTH];
	char fpsString[SENTENCE_MAX_LENGTH];
	float red = 0;
	float green = 0;
	float blue = 0;

	// FPS 정수를 문자열로 변경
	_itoa_s(fps, tempString, 10);

	// FPS 문자열 생성
	strcpy_s(fpsString, "FPS: ");
	strcat_s(fpsString, tempString);

	// fps 값에 따른 폰트 색 설정
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

	// 문장을 FPS 값으로 업데이트
	if (!(UpdateSentence(m_sentence3, fpsString, 0, 70, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char cpuString[SENTENCE_MAX_LENGTH];

	// CPU 정수를 문자열로 변경
	_itoa_s(cpu, tempString, 10);

	// CPU 문자열 생성
	strcpy_s(cpuString, "CPU: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// 문장을 CPU 값으로 업데이트
	if (!(UpdateSentence(m_sentence4, cpuString, 0, 90, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::SetCameraPosition(XMFLOAT3 pos, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char dataString[SENTENCE_MAX_LENGTH];

	// 부동 소수점에서 정수로 위치를 변환합니다.
	int positionX = (int)pos.x;
	int positionY = (int)pos.y;
	int positionZ = (int)pos.z;

	// 9999 또는 -9999를 초과하면 위치를 자릅니다.
	if (positionX > 9999) { positionX = 9999; }
	if (positionY > 9999) { positionY = 9999; }
	if (positionZ > 9999) { positionZ = 9999; }

	if (positionX < -9999) { positionX = -9999; }
	if (positionY < -9999) { positionY = -9999; }
	if (positionZ < -9999) { positionZ = -9999; }

	// X 위치 문자열을 설정합니다.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence5, dataString, 0, 130, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	// Y 위치 문자열을 설정합니다.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence6, dataString, 0, 150, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	// Z 위치 문자열을 설정합니다.
	_itoa_s(positionZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence7, dataString, 0, 170, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	return true;
}


bool TextClass::SetCameraRotation(XMFLOAT3 rot, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char dataString[SENTENCE_MAX_LENGTH];

	// 회전값을 정수로 변환합니다.
	int rotationX = (int)rot.x;
	int rotationY = (int)rot.y;
	int rotationZ = (int)rot.z;

	// X 회전 문자열을 설정합니다.
	_itoa_s(rotationX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence8, dataString, 0, 210, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	// Y 회전 문자열을 설정합니다.
	_itoa_s(rotationY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence9, dataString, 0, 230, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	// Z 회전 문자열을 설정합니다.
	_itoa_s(rotationZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	if (!UpdateSentence(m_sentence10, dataString, 0, 250, 0.0f, 1.0f, 0.0f, deviceContext))	return false;

	return true;
}

bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
{
	char tempString[SENTENCE_MAX_LENGTH];
	char renderString[SENTENCE_MAX_LENGTH];

	if (count > 999999999) { count = 999999999; }

	_itoa_s(count, tempString, 10);
	strcpy_s(renderString, "Render Count: ");
	strcat_s(renderString, tempString);

	// 문장을 FPS 값으로 업데이트
	if (!(UpdateSentence(m_sentence11, renderString, 0, 300, 0.0f, 1.0f, 0.0f, deviceContext)))	return false;

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// SentenceType 객체 생성 및 초기화
	*sentence = new SentenceType;
	if (!*sentence)	return false;
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;
	(*sentence)->maxLength = maxLength;
	(*sentence)->vertexCount = 6 * maxLength;	// 한글자에 삼각형 2개 즉 정점 6개 필요
	(*sentence)->indexCount = (*sentence)->vertexCount;

	int vCount = (*sentence)->vertexCount;
	int iCount = (*sentence)->indexCount;

	// 정점 배열 생성 및 초기화
	VertexType* vertices = new VertexType[vCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * vCount));

	// 인덱스 배열 생성
	unsigned long* indices = new unsigned long[iCount];
	if (!indices)	return false;
	for (int i = 0; i < iCount; i++)	indices[i] = i;

	// 정점 버퍼 서술자 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버퍼 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))	return false;

	// 인덱스 버퍼 서술자 설정
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * iCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 초기화용 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))	return false;

	// 생성 후 정점 및 인덱스 버퍼 해제
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

	// 문장 길이
	int numLetters = static_cast<int>(strlen(text));
	if (numLetters > sentence->maxLength)	return false;

	// 정점 버퍼 자원에 자료를 올릴 수 있도록 포인터 생성
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	VertexType* verticesPtr;

	// 정점 배열 생성 및 초기화
	vertices = new VertexType[vCount];
	if (!vertices)	return false;
	memset(vertices, 0, (sizeof(VertexType) * vCount));

	// 비트맵 좌표 계산
	// 정중앙을 (0, 0)으로 두어 상하좌우 계산
	drawX = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);
	drawY = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);

	// 문장 텍스트와 위치를 사용하여 정점 배열을 빌드하기 위해 폰트 클래스 사용
	m_font->BuildVertexArray(reinterpret_cast<void*>(vertices), text, drawX, drawY);

	// 정점 버퍼 매핑
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))	return false;

	// 정점 버퍼에 데이터 복사
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vCount));

	// 정점 버퍼 메모리 해제
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

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 정점 버퍼의 단위와 오프셋 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 및 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 픽셀 색상 벡터를 문장 색상으로 설정
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// 폰트 쉐이더로 텍스트 렌더링
	if (!(FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_font->GetTexture(), pixelColor)))	return false;

	return true;
}