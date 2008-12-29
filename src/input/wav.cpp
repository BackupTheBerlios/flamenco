/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wav-файла.
 */
#include <flamenco/flamenco.h>
#pragma warning(disable:4201) // Отключение предупреждения о nameless struct.
#include <mmsystem.h>

using namespace flamenco;

// Конструктор.
wav_decoder::wav_decoder( std::auto_ptr<source> source )
    : mSource(source), mDataOffset(0), mSampleRate(0),
      mSampleCount(0), mChannelCount(0), mChannelBuffer(NULL)
{
    assert(mSource.get());
    
    // Проверяем заголовок RIFF.
    const u32 RIFF = 'FFIR',
              WAVE = 'EVAW';
    u32 chunkId;
    mSource->read(&chunkId, sizeof(u32), 1);
    if (chunkId != RIFF)
        throw std::runtime_error("File is not a valid RIFF container.");
    mSource->seek(4, SEEK_CUR);
    mSource->read(&chunkId, sizeof(u32), 1);
    if (chunkId != WAVE)
        throw std::runtime_error("File is not a valid WAVE container.");
    
    // Читаем настройки.
    const u32 FMT = ' tmf';
    mSource->read(&chunkId, sizeof(u32), 1);
    if (chunkId != FMT)
        throw std::runtime_error("Missing 'fmt ' section after RIFF header.");
    u32 chunkSize;
    mSource->read(&chunkSize, sizeof(u32), 1);
    WAVEFORMATEX format;
    mSource->read(&format, sizeof(WAVEFORMATEX), 1);
    
    // Проверяем правильность формата.
    if (format.wFormatTag != WAVE_FORMAT_PCM)
        throw std::runtime_error("Compressed wave files are not supported.");
    if (format.wBitsPerSample != 16)
        throw std::runtime_error("Unsupported sample format (expected 16 bits).");
    mSampleRate = format.nSamplesPerSec;
    mChannelCount = format.nChannels;
    assert(0 != mChannelCount);
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");
    // Создаем буфер для преобразования семплов.
    mChannelBuffer = new s16[CHANNEL_BUFFER_SIZE_IN_SAMPLES * mChannelCount];
    
    // Перематываем на начало следующего чанка и ищем секцию 'data'.
    const u32 DATA = 'atad';
    mSource->seek(chunkSize - sizeof(WAVEFORMATEX), SEEK_CUR);
    for (;;)
    {
        if (mSource->read(&chunkId, sizeof(u32), 1) != 1)
        {
            delete [] mChannelBuffer;
            throw std::runtime_error("EOF while looking for 'data' section.");
        }
        mSource->read(&chunkSize, sizeof(u32), 1);
        if (chunkId == DATA)
            break;
        mSource->seek(chunkSize, SEEK_CUR);
    }
    
    // Секция найдена, в chunkSize ее размер.
    mSampleCount = chunkSize / (sizeof(s16) * mChannelCount);
    mDataOffset = mSource->tell();
}

// Деструктор.
wav_decoder::~wav_decoder()
{
    delete [] mChannelBuffer;
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 wav_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    // Количество семплов, которые можно считать.
    const u32 PORTION_SIZE = sizeof(s16) * mChannelCount,
              READ_COUNT = std::min(count, mSampleCount - (mSource->tell() - mDataOffset) / PORTION_SIZE);
    u32 samplesRead = mSource->read(mChannelBuffer, PORTION_SIZE, READ_COUNT);
    assert(samplesRead == READ_COUNT);
    
    // Моно.
    if (mChannelCount == 1)
    {
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
            *left++ = *right++ = static_cast<f32>(mChannelBuffer[i]) / (1 << 15);
    }
    else // Стерео.
    {
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
        {
            *left++  = static_cast<f32>(mChannelBuffer[(i << 1)])     / (1 << 15);
            *right++ = static_cast<f32>(mChannelBuffer[(i << 1) + 1]) / (1 << 15);
        }
    }
    return READ_COUNT;
}

// Установка курсора начала декодирования на заданный семпл.
void wav_decoder::seek( u32 sample )
{
    assert(sample < mSampleCount);
    mSource->seek(mDataOffset + sample * mChannelCount * sizeof(s16), SEEK_SET);
}
