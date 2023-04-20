#include "Stdafx.h"
#include "SoundClass.h"
#include <stdio.h>

SoundClass::SoundClass() {}
SoundClass::SoundClass(const SoundClass& other) {}
SoundClass::~SoundClass() {}

bool SoundClass::Initialize(HWND hwnd)
{
	// direct sound 1차 버퍼 초기화
	if (!InitializeDirectSound(hwnd))	return false;

	// wav 오디오파일 2차 버퍼 초기화
	if (!LoadWaveFile("./data/sound02.wav", &m_secondaryBuffer1, &m_secondary3DBuffer1)) {
		MessageBox(hwnd, L"Could not initialize the sound object", L"Error", MB_OK);
		return false;
	}

	return PlayWaveFile();
}

void SoundClass::Shutdown()
{
	// 2차버퍼 해제
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);

	// direct sound 1차 버퍼 해제
	ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	// 기본 음향 장치에 대한 포인터 생성
	if (FAILED(DirectSoundCreate8(NULL, &m_directSound, NULL)))	return false;

	// 사운드 포맷이 변경되어도 문제 없도록 1차 버퍼의 협력 수준 설정
	if (FAILED(m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))	return false;

	// 1차 버퍼 서술자 설정
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;	// DSBCAPS_CTRL3D 마스크를 사용하여 이 음향 효과가 3D 기능을 가짐을 알려준다
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 기본 음향 장치에 대한 1차 사운드 버퍼 생성
	if (FAILED(m_directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL)))	return false;

	// 1차 사운드 버퍼의 포맷 설정
	// 여기서는 wav 파일에 44100 샘플, 16비트 스테레오로 세팅
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	if (FAILED(m_primaryBuffer->SetFormat(&waveFormat)))	return false;

	// 리스너 인터페이스 설정
	if (FAILED(m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener)))	return false;

	// 리스너의 초기 위치를 장면의 중간에 설정
	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	// 리스너 인터페이스 해제
	if (m_listener) {
		m_listener->Release();
		m_listener = 0;
	}

	// 1차 사운드 버퍼 포인터 해제
	if (m_primaryBuffer) {
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// direct sound 포인터 해제
	if (m_directSound) {
		m_directSound->Release();
		m_directSound = 0;
	}
}

bool SoundClass::LoadWaveFile(const char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer** secondary3DBuffer)
{
	FILE* filePtr = nullptr;
	int error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)	return false;

	// wav 파일
	SoundHeaderType waveFileHeader;
	unsigned int count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)	return false;

	// RIFF 포맷 체크
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// WAV 포맷 체크
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// fmt 포맷 체크
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// 오디오 포맷이 pcm 지원하는지 체크
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// 스테레오 포맷인지 체크
	// 3D 음향 파일은 반드시 단일 채널이어야함
	if (waveFileHeader.numChannels != 1)
	{
		return false;
	}

	// sample rate가 44.1 KHz인지 체크
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// 16 bit 포맷인지 체크
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// data 헤더인지 체크
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// 2차 사운드 버퍼 포맷 세팅
	// 3D 음향 파일은 반드시 단일 채널이어야함
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;	// DSBCAPS_CTRL3D 마스크를 사용하여 이 음향 효과가 3D 기능을 가짐을 알려준다
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 2차 사운드 버퍼를 만들기 위한 임시 버퍼
	IDirectSoundBuffer* tempBuffer = nullptr;
	if (FAILED(m_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL)))
	{
		return false;
	}

	// 임시 버퍼로 2차 사운드 버퍼 생성
	if (FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer)))
	{
		return false;
	}

	// 임시 버퍼 해제
	tempBuffer->Release();
	tempBuffer = 0;

	// 사운드파일의 시작부분으로 이동
	fseek(filePtr, sizeof(SoundHeaderType), SEEK_SET);

	// 사운드파일을 hold 하기 위해 임시 버퍼 생성
	unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)	return false;

	// 임시 버퍼에서 파일 읽기
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)	return false;

	error = fclose(filePtr);
	if (error != 0) return false;

	// 사운드파일을 쓰기 위해 2차 버퍼를 잠금
	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize = 0;
	if (FAILED((*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, reinterpret_cast<void**>(&bufferPtr), reinterpret_cast<DWORD*>(&bufferSize), NULL, 0, 0)))	return false;

	// 사운드파일을 버퍼에 복사
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// 작업 후 잠금 해제
	if (FAILED((*secondaryBuffer)->Unlock(reinterpret_cast<void*>(bufferPtr), bufferSize, NULL, 0)))	return false;

	// 사운드버퍼 객체 해제
	delete[] waveData;
	waveData = 0;

	// 2차 사운드 버퍼에 3D 인터페이스 로드
	if (FAILED((*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer)))	return false;

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	if (*secondary3DBuffer) {
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = nullptr;
	}

	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}
}

bool SoundClass::PlayWaveFile()
{
	// 사운드를 배치 할 3D 위치 설정
	float positionX = 2.0f;
	float positionY = 0.0f;
	float positionZ = 2.0f;

	// 사운드 버퍼의 시작지점 세팅
	if (FAILED(m_secondaryBuffer1->SetCurrentPosition(0)))	return false;

	// 버퍼의 볼륨 100%로 세팅
	if (FAILED(m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX)))	return false;

	// 사운드의 3D 위치 설정
	m_secondary3DBuffer1->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	// 2차 사운드 버퍼의 내용 재생
	if (FAILED(m_secondaryBuffer1->Play(0, 0, 0)))	return false;

	return true;
}