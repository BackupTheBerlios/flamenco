#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

namespace flamenco
{

// Источник гармонического звука.
class Wav : public Pin
{
public:
    // Открытие wav файла
    static reference<Wav> create( const char * path, bool looped = false );
    
    // Деструктор
    ~Wav();
    
private:
    // Скрытый конструктор
    Wav( const char * path, bool looped );

    // Заполняет буферы каналов звуковыми данными.
    void process( s16 * left, s16 * right );

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

    // Зацикленность звука
    bool mIsLooped;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
