/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Источник звука из ogg-vorbis-файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <stdio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


namespace flamenco
{

// Источник звука из ogg-vorbis-файла.
class Ogg : public Pin
{
public:
    ~Ogg();
    
    // Создание источника звука.
    static reference<Ogg> create( const char * path );
    
    // Флаг зацикленности звука.
    atomic<bool> looping;
    
    
private:
    Ogg( const char * path );
    
    // Помещает данные из буфера в левый и правый каналы.
    void process( f32 * left, f32 * right );
    // Чтение данных из файла во внутренний буфер.
    void fill(bool looping);
    
    // Количество каналов.
    u32 mChannels;
    // Частота.
    u32 mFrequency;
    
    // Буфер для семплов размером BUFFER_SIZE_IN_SAMPLES.
    s16 * mSamples;
    // Текущий семпл от начала буфера.
    u32 mSamplesCurrent;
    // Реальное количество семплов в буфере.
    u32 mSampleCount;

	// Файл для чтения
	FILE * mInput;
    
	// Входной файл vorbis
	OggVorbis_File  *mVorbisFile;
	// Информация о файле
	vorbis_info    *mVorbisInfo;
	// Признак окончания файла.
	bool mIsFinished;

   
    // Магическое число для проверки массива.
    static const s16 MAGIC;
    
    // Максимальный размер буфера в семплах.
    static const u32 BUFFER_SIZE_IN_SAMPLES;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_OGG_H_
