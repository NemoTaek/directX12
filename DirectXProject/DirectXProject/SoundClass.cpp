#include "Stdafx.h"
#include "SoundClass.h"
#include <stdio.h>

SoundClass::SoundClass() {}
SoundClass::SoundClass(const SoundClass& other) {}
SoundClass::~SoundClass() {}

bool SoundClass::Initialize(HWND hwnd)
{
	// direct sound 1�� ���� �ʱ�ȭ
	if (!InitializeDirectSound(hwnd))	return false;

	// wav ��������� 2�� ���� �ʱ�ȭ
	if (!LoadWaveFile("./data/sound01.wav", &m_secondaryBuffer1)) {
		MessageBox(hwnd, L"Could not initialize the sound object", L"Error", MB_OK);
		return false;
	}

	return PlayWaveFile();
}

void SoundClass::Shutdown()
{
	// 2������ ����
	ShutdownWaveFile(&m_secondaryBuffer1);

	// direct sound 1�� ���� ����
	ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	// �⺻ ���� ��ġ�� ���� ������ ����
	if (FAILED(DirectSoundCreate8(NULL, &m_directSound, NULL)))	return false;

	// ���� ������ ����Ǿ ���� ������ 1�� ������ ���� ���� ����
	if (FAILED(m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))	return false;

	// 1�� ���� ������ ����
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// �⺻ ���� ��ġ�� ���� 1�� ���� ���� ����
	if (FAILED(m_directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL)))	return false;

	// 1�� ���� ������ ���� ����
	// ���⼭�� wav ���Ͽ� 44100 ����, 16��Ʈ ���׷����� ����
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	if (FAILED(m_primaryBuffer->SetFormat(&waveFormat)))	return false;

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	// 1�� ���� ���� ������ ����
	if (m_primaryBuffer) {
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// direct sound ������ ����
	if (m_directSound) {
		m_directSound->Release();
		m_directSound = 0;
	}
}

bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	FILE* filePtr = nullptr;
	int error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)	return false;

	// wav ����
	SoundHeaderType waveFileHeader;
	unsigned int count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)	return false;

	// RIFF ���� üũ
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// WAV ���� üũ
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// fmt ���� üũ
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// ����� ������ pcm �����ϴ��� üũ
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// ���׷��� �������� üũ
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// sample rate�� 44.1 KHz���� üũ
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// 16 bit �������� üũ
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// data ������� üũ
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// 2�� ���� ���� ���� ����
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 2�� ���� ���۸� ����� ���� �ӽ� ����
	IDirectSoundBuffer* tempBuffer = nullptr;
	if (FAILED(m_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL)))
	{
		return false;
	}

	// �ӽ� ���۷� 2�� ���� ���� ����
	if (FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer)))
	{
		return false;
	}

	// �ӽ� ���� ����
	tempBuffer->Release();
	tempBuffer = 0;

	// ���������� ���ۺκ����� �̵�
	fseek(filePtr, sizeof(SoundHeaderType), SEEK_SET);

	// ���������� hold �ϱ� ���� �ӽ� ���� ����
	unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)	return false;

	// �ӽ� ���ۿ��� ���� �б�
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)	return false;

	error = fclose(filePtr);
	if (error != 0) return false;

	// ���������� ���� ���� 2�� ���۸� ���
	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize = 0;
	if (FAILED((*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, reinterpret_cast<void**>(&bufferPtr), reinterpret_cast<DWORD*>(&bufferSize), NULL, 0, 0)))	return false;

	// ���������� ���ۿ� ����
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// �۾� �� ��� ����
	if (FAILED((*secondaryBuffer)->Unlock(reinterpret_cast<void*>(bufferPtr), bufferSize, NULL, 0)))	return false;

	// ������� ��ü ����
	delete[] waveData;
	waveData = 0;

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}
}

bool SoundClass::PlayWaveFile()
{
	// ���� ������ �������� ����
	if (FAILED(m_secondaryBuffer1->SetCurrentPosition(0)))	return false;

	// ������ ���� 100%�� ����
	if (FAILED(m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX)))	return false;

	// 2�� ���� ������ ���� ���
	if (FAILED(m_secondaryBuffer1->Play(0, 0, 0)))	return false;

	return true;
}