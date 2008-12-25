/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Декодер wav-файлов без сжатия.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

#include <stdio.h>

namespace flamenco
{

// Декодер wav-файлов без сжатия.
class wav_decoder : noncopyable
{
public:
    wav_decoder( std::auto_ptr<source> source );
    ~wav_decoder();
    
    // Копирует в левый и правый каналы count декодированных семплов.
    // Возвращает количество скопированных семплов, оно может быть меньше count,
    // если поток закончился.
    u32 unpack( f32 * left, f32 * right, u32 count );
    
    // Установка курсора начала декодирования на заданный семпл.
    void seek( u32 sample );
    
    // Длина потока в семплах. Функция оптимизирована для частых вызовов.
    inline u32 length() const
    {
        return mSampleCount;
    }
    
    // Частота звукового потока (для преобразования частоты потоком).
    inline u32 frequency() const
    {
        return mSampleRate;
    }
    
private:
    // Источник данных.
    std::auto_ptr<source> mSource;
    
    // Смещение от начала файла до начала wav-данных.
    u32 mDataOffset;
    // Частота дискретизации.
    u32 mSampleRate;
    // Длина звука в семплах.
    u32 mSampleCount;
    // Количество каналов (1 или 2).
    u32 mChannelCount;
    // Буфер для преобразования семплов из interleaved s16 в separate f32.
    s16 * mChannelBuffer;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
