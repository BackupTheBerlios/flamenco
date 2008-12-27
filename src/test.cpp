/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Пример использования библиотеки.
 */
#include <flamenco/flamenco.h>
#include <MMSystem.h>
#include <dsound.h>
#include <dxerr9.h>
#include <iostream>
#include <conio.h>

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
    
    // Создаем интерфейс DirectSound.
    check_directx(DirectSoundCreate8(&GUID_NULL, &directSound, NULL));
    check_directx(directSound->SetCooperativeLevel(GetForegroundWindow(), DSSCL_PRIORITY));
    
    // Получаем первичный буфер.
    LPDIRECTSOUNDBUFFER primaryBuffer = NULL;
    DSBUFFERDESC descr;
    ZeroMemory(&descr, sizeof(DSBUFFERDESC));
    descr.dwSize = sizeof(DSBUFFERDESC);
    descr.dwFlags = DSBCAPS_PRIMARYBUFFER;
    descr.lpwfxFormat = NULL;
    check_directx(directSound->CreateSoundBuffer(&descr, &primaryBuffer, NULL));
    
    // Изменяем формат первичного буфера.
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
    
    
    // Формат буфера микшера.
    const u32 MIXER_BUFFER_SIZE_IN_BYTES = MIXER_BUFFER_SIZE_IN_SAMPLES * sizeof(s16);
    // Размер звукового буфера должен быть больше 100 ms, иначе GetCurrentPosition()
    // будет выдавать неправильные данные.
    const u32 SOUND_BUFFER_SIZE_IN_BYTES = 10 * MIXER_BUFFER_SIZE_IN_BYTES;
    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize          = sizeof(DSBUFFERDESC);
    desc.dwFlags         = DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2;
    desc.dwBufferBytes   = SOUND_BUFFER_SIZE_IN_BYTES;
    desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    desc.lpwfxFormat     = &wfx;
    
    // Создаем буфер.
    LPDIRECTSOUNDBUFFER soundBuffer = NULL;
    check_directx(directSound->CreateSoundBuffer(&desc, &soundBuffer, NULL));
    
    reference<stream<ogg_decoder> > sound = stream<ogg_decoder>::create(std::auto_ptr<source>(new file_source("input.ogg")));
    
    // Инициализация flamenco.
    reference<pin> wave = sine::create(400);
    //reference<ogg> wave = ogg::create("input.ogg");
    reference<pin> noise = noise::create();
    
    reference<volume_pan> vp = volume_pan::create(sound, 1.0f, 0.0f);
    mixer & mixer = mixer::singleton();
    //mixer.attach(sine);
    mixer.attach(vp);
    //mixer.attach(noise);
    
    // Заполнение звукового буфера.
    s16 * bufferPtr;
    u32 bufferSize;
    check_directx(soundBuffer->Lock(0, 0, reinterpret_cast<void **>(&bufferPtr), &bufferSize,
                                    NULL, NULL, DSBLOCK_ENTIREBUFFER));
    // Заполняем обе половинки буфера.
    mixer.mix(bufferPtr);
    mixer.mix(bufferPtr + MIXER_BUFFER_SIZE_IN_SAMPLES);
    check_directx(soundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0));
    
    // Проигрываем звук и дописываем данные по ходу.
    soundBuffer->Play(0, 0, DSBPLAY_LOOPING);
    u32 writeOffset = MIXER_BUFFER_SIZE_IN_BYTES * 2;
    while (true)
    {
        u32 cursorPos;
        soundBuffer->GetCurrentPosition(&cursorPos, NULL);
        
        // Определяем, нужно ли дописать очередную порцию данных.
        u32 offset = (SOUND_BUFFER_SIZE_IN_BYTES + writeOffset - cursorPos) % SOUND_BUFFER_SIZE_IN_BYTES;
        if (offset > MIXER_BUFFER_SIZE_IN_BYTES)
        {
            check_directx(soundBuffer->Lock(writeOffset, MIXER_BUFFER_SIZE_IN_BYTES,
                              reinterpret_cast<void **>(&bufferPtr), &bufferSize,
                              NULL, NULL, 0));
            mixer.mix(bufferPtr);
            check_directx(soundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0));
            writeOffset = (writeOffset + MIXER_BUFFER_SIZE_IN_BYTES) % SOUND_BUFFER_SIZE_IN_BYTES;
        }
        // Не нужно опрашивать GetCurrentPosition() слишком часто.
        Sleep(LATENCY_MSEC >> 1);
        
        if (_kbhit())
        {
            switch (_getch())
            {
            case '+':
                vp->pan.set(vp->pan() + 0.1f);
                continue;
            
            case '-':
                vp->pan.set(vp->pan() - 0.1f);
                continue;
            
            case 'l':
                sound->looping.set(!sound->looping());
                continue;
            }
            // Если нажата любая другая клавиша - выходим.
            break;
        }   
    }
    
    // Освобождение ресурсов.
    soundBuffer->Release();
    directSound->Release();
}
