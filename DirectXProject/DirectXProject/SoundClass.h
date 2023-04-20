#pragma once

class SoundClass
{
private:
	struct SoundHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();
	bool LoadWaveFile(const char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	bool PlayWaveFile();

private:
	IDirectSound8* m_directSound = nullptr;
	IDirectSoundBuffer* m_primaryBuffer = nullptr;
	IDirectSoundBuffer8* m_secondaryBuffer1 = nullptr;
	IDirectSound3DListener8* m_listener = nullptr;
	IDirectSound3DBuffer8* m_secondary3DBuffer1 = nullptr;
};