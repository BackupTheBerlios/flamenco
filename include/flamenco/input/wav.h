/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Источник звука из wav-файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

#include <stdio.h>

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

	// Помещает данные из буфера в левый и правый каналы
    void process( f32 * left, f32 * right );
	// Чтение данных из файла во внутренний буфер
	void fill(bool looping);

    // Количество каналов
    u32 mChannels;
    // Частота
    u32 mFrequency;

    // Буфер для семплов размером CHANNEL_BUFFER_SIZE_IN_SAMPLES * mChannels
    s16 * mSamples;

	// MAGIC
	static const s16 MAGIC;

    // Файл для чтения
    FILE * mInput;
    // Начало блока данных в файле
    u32 mInputOffset;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
