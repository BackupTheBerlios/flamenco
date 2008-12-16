/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ������ ������������� ����������.
 */
#include <flamenco/flamenco.h>
#include <dsound.h>
#include <dxerr9.h>
#include <iostream>

#pragma warning(disable: 4127)
#define check_directx(func)                                                          \
    do { HRESULT hr = (func);                                                        \
         if (FAILED(hr))                                                             \
         {                                                                           \
             std::cerr << #func " failed: " << DXGetErrorDescription9A(hr) << '\n';  \
             exit(1);                                                                \
         }                                                                           \
    } while (false)


int main()
{
    using namespace flamenco;
    
    LPDIRECTSOUND8 directSound;
    
    // ������� ��������� DirectSound.
    check_directx(DirectSoundCreate8(&GUID_NULL, &directSound, NULL));
    check_directx(directSound->SetCooperativeLevel(GetForegroundWindow(), DSSCL_PRIORITY));
    
    // �������� ��������� �����.
    LPDIRECTSOUNDBUFFER primaryBuffer = NULL;
    DSBUFFERDESC descr;
    ZeroMemory(&descr, sizeof(DSBUFFERDESC));
    descr.dwSize = sizeof(DSBUFFERDESC);
    descr.dwFlags = DSBCAPS_PRIMARYBUFFER;
    descr.lpwfxFormat = NULL;
    check_directx(directSound->CreateSoundBuffer(&descr, &primaryBuffer, NULL));
    
    // �������� ������ ���������� ������.
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.cbSize          = sizeof(WAVEFORMATEX);
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
    wfx.nSamplesPerSec  = FREQUENCY;
    wfx.wBitsPerSample  = 16;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = (u32)wfx.nSamplesPerSec * wfx.nBlockAlign;
    check_directx(primaryBuffer->SetFormat(&wfx));
    primaryBuffer->Release();
    
    
    // ������ ������ �������.
    const u32 MIXER_BUFFER_SIZE_IN_BYTES = MIXER_BUFFER_SIZE_IN_SAMPLES * sizeof(s16);
    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize          = sizeof(DSBUFFERDESC);
    desc.dwFlags         = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2;
    desc.dwBufferBytes   = 2 * MIXER_BUFFER_SIZE_IN_BYTES;
    desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    desc.lpwfxFormat     = &wfx;

    // ������� �����.
    LPDIRECTSOUNDBUFFER soundBuffer = NULL;
    check_directx(directSound->CreateSoundBuffer(&desc, &soundBuffer, NULL));
    
    
    
    // ������������� flamenco.
    reference<Pin> sine = Sine::create(500);
    Mixer & mixer = Mixer::singleton();
    mixer.attach(sine);
    
    
    // ���������� ��������� ������.
    s16 * bufferPtr;
    u32 bufferSize;
    check_directx(soundBuffer->Lock(0, 0, reinterpret_cast<void **>(&bufferPtr), &bufferSize,
                                    NULL, NULL, DSBLOCK_ENTIREBUFFER));
    // ��������� ��� ��������� ������.
    mixer.mix(bufferPtr);
    mixer.mix(bufferPtr + MIXER_BUFFER_SIZE_IN_SAMPLES);
    check_directx(soundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0));
    
    soundBuffer->Play(0, 0, DSBPLAY_LOOPING);
    Sleep(1000);
    
    // ������������ ��������.
    soundBuffer->Release();
    directSound->Release();
}
