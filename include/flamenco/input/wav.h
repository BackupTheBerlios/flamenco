/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Источник звука из wav-файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

namespace flamenco
{

// Источник звука из wav-файла.
class Wav : public Pin
{
public:
    ~Wav();
    
    // Создание источника звука.
    static reference<Wav> create( const char * path );
    
    // Флаг зацикленности звука.
    atomic_bool looping;
    
    
private:
    Wav( const char * path );
    void process( f32 * left, f32 * right );
    
    // Количество каналов
    u32 mChannels;
    
    // Весь файл целиком
    u8 * mBuffer;
    
    // Указатель на начало семплов
    s16 * mSamples;
    
    // Общее количество семплов
    u32 mSamplesCount;
    
    // Текущий семпл
    u32 mCurrentSample;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
