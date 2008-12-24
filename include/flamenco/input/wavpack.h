/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Источник звука из wv-файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAVPACK_H_
#define _FLAMENCO_INPUT_WAVPACK_H_

#include <wavpack/wavpack.h>

namespace flamenco
{


// Источник звука из wv-файла.
class wavpack : public pin
{
public:
    ~wavpack();

    // Создание источника звука.
    static reference<wavpack> create( const char * path );

    // Флаг зацикленности звука.
    atomic<bool> looping;


private:
    wavpack( const char * path );

    // Помещает данные из буфера в левый и правый каналы
    void process( f32 * left, f32 * right );
    // Чтение данных из файла во внутренний буфер
    void fill(bool looping);
    // Распаковка одной порции данных в буфер
    u32 unpack(s32 * dst, u32 offset, u32 size);

    // Количество каналов
    u32 mChannels;
    // Частота
    u32 mFrequency;

    // Буфер для семплов размером BUFFER_SIZE_IN_SAMPLES
    s32 * mSamples;
    // Текущий семпл от начала буфера
    u32 mSamplesCurrent;
    // Реальное количество семплов в буфере
    u32 mSamplesCount;

    // Файл для чтения
    WavpackContext * mInput;
    // Признак окончания файла
    bool mIsFinished;

    // Магическое число для проверки массива
    static const s32 MAGIC;

    // Максимальный размер буфера в семплах
    static const u32 BUFFER_SIZE_IN_SAMPLES;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAVPACK_H_
