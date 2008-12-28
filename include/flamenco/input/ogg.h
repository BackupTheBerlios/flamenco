/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Декодер ogg vorbis файлов
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace flamenco
{

// Декодер ogg vorbis файлов
class ogg_decoder : noncopyable
{
public:
    ogg_decoder( std::auto_ptr<source> source );
    ~ogg_decoder();
    
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
	// Распаковывает из vorbis потока mBufferSize семплов во внутренние буферы.
	// Возвращает количество прочитанных семплов.
	u32 unpack_vorbis();

    // Источник данных.
    std::auto_ptr<source> mSource;

    // Частота дискретизации.
    u32 mSampleRate;
    // Длина звука в семплах.
    u32 mSampleCount;
    // Количество каналов (1 или 2).
    u32 mChannelCount;

    // Буферы для левого и правого каналов.
    f32 * mBufferL;
    f32 * mBufferR;
    // Размер одного буфера в семплах
    u32 mBufferSize;

	// Текущее количество семплов в одном буфере
	u32 mBufferRealSize;
	// Текущий семпл в буфере (левом для моно, обоих для стерео)
	u32 mBufferOffset;

    // Входной логический поток
    OggVorbis_File  *mVorbisFile;
    // Информация о потоке
    vorbis_info    *mVorbisInfo;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
