/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Реализация источника звука из wav-файла.
 */
#include <flamenco/flamenco.h>

#include <iostream>

using namespace flamenco;

namespace 
{

// Вспомогательные методы для чтения файла.
size_t read_func(void * ptr, size_t size, size_t count, void *datasource)
{
    source * input = reinterpret_cast<source *>(datasource);
    return static_cast<size_t>(input->read(static_cast<void *>(ptr), static_cast<u32>(size), static_cast<u32>(count)));
}

int seek_func(void * datasource, ogg_int64_t offset, int whence)
{
    source * input = reinterpret_cast<source *>(datasource);
    input->seek(static_cast<s32>(offset), whence);
    return 0;
}

long tell_func(void * datasource)
{
    source * input = reinterpret_cast<source *>(datasource);
    return input->tell();
}

int close_func(void *)
{
    return 0;
}

static ov_callbacks gCallbacks = {
    read_func,
    seek_func, 
    close_func,
    tell_func
};

}


// Конструктор
ogg_decoder::ogg_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBufferL(NULL), mBufferR(NULL), 
      mBufferSize(0), mBufferRealSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // Создаем и открываем vorbis поток
    mVorbisFile = new OggVorbis_File;
    if (ov_open_callbacks(mSource.get(), mVorbisFile, NULL, -1, gCallbacks) < 0)
        throw std::runtime_error("File is not a valid ogg container.");

    // Информация о потоке vorbis.
    mVorbisInfo = ov_info(mVorbisFile, -1);

    // Количество каналов.
    mChannelCount = mVorbisInfo->channels;
    assert(mChannelCount != 0);
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");

    // Частота.
    mSampleRate = mVorbisInfo->rate;

    // Общее количество семплов
    mSampleCount = static_cast<u32>(ov_pcm_total(mVorbisFile, -1)) / mChannelCount;

    // Размер одного буфера на одну секунду
    mBufferSize = mSampleRate;

    mBufferL = new f32[mBufferSize];
    mBufferR = (mChannelCount == 2) ? new f32[mBufferSize] : mBufferL;
}

void ogg_decoder::seek( u32 sample )
{
    if (ov_pcm_seek(mVorbisFile, sample * mChannelCount) != 0)
        throw std::runtime_error("Seeking error.");

    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    // Обнуляем буферы
    mBufferRealSize = 0;
}

// Распаковывает из vorbis потока count семплов во внутренний буфер.
// Возвращает количество прочитанных семплов.
u32 ogg_decoder::unpack_vorbis()
{
    int currentSection;
    u32 readSamples = 0;
    
    while (readSamples < mBufferSize)
    {
        float ** pcm;
        int result = ov_read_float(mVorbisFile, &pcm, mBufferSize - readSamples, &currentSection);
        if (result == 0)
            break;
        else if (result > 0)
        {
            memcpy(mBufferL + readSamples, pcm[0], result * sizeof(f32));
            if (mChannelCount == 2)
                memcpy(mBufferR + readSamples, pcm[1], result * sizeof(f32));
            readSamples += result;
        }
    }
    return readSamples;
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 ogg_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    u32 samplesCount = 0;
    
    while (samplesCount < count)
    {
        if (mBufferOffset >= mBufferRealSize)
        {
            // Пытаемся подгрузить еще данных в буфер
            mBufferOffset = 0;
            if (0 == (mBufferRealSize = unpack_vorbis()))
                break;
        }
        const u32 SAMPLES_COUNT = std::min(mBufferRealSize - mBufferOffset, count - samplesCount);
        memcpy(left + samplesCount, mBufferL + mBufferOffset, SAMPLES_COUNT * sizeof(f32));
        memcpy(right + samplesCount, mBufferR + mBufferOffset, SAMPLES_COUNT * sizeof(f32));
        samplesCount += SAMPLES_COUNT;
        mBufferOffset += SAMPLES_COUNT;
    }
//    std::cout << "Result: " << samplesCount << '\n';
    return samplesCount;
}

ogg_decoder::~ogg_decoder()
{
    ov_clear(mVorbisFile);
    delete [] mBufferL;
    if (mChannelCount == 2)
        delete [] mBufferR;
}
