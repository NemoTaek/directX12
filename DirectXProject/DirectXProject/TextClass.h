#pragma once

class FontClass;
class FontShaderClass;

// ���ڿ��� �׸��� Ŭ����
class TextClass
{
private:
	// ���� �������� �ʿ��� ����
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);
	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetKeyboardInput(int, ID3D11DeviceContext*);
	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private:
	FontClass* m_font = nullptr;
	FontShaderClass* m_fontShader = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	XMMATRIX m_baseViewMatrix;
	SentenceType* m_sentence1 = nullptr;
	SentenceType* m_sentence2 = nullptr;
	SentenceType* m_sentence3 = nullptr;
	SentenceType* m_sentence4 = nullptr;
	SentenceType* m_sentence5 = nullptr;
};
