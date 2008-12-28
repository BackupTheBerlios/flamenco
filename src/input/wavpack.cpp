/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wv-файла.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace
{

int32_t read_bytes( void * id, void * data, int32_t bcount )
{
    source * input = reinterpret_cast<source *>(id);
    return input->read(data, 1, bcount);
}

uint32_t get_pos( void * id )
{
    source * input = reinterpret_cast<source *>(id);
    return input->tell();
}

static int set_pos_abs( void *id, uint32_t pos )
{
    source * input = reinterpret_cast<source *>(id);
    input->seek(pos, SEEK_SET);
    return 0;
}

static int set_pos_rel( void *id, int32_t delta, int mode )
{
    source * input = reinterpret_cast<source *>(id);
    input->seek(delta, mode);
    return 0;
}

static int push_back_byte( void *id, int c )
{
    // Хитрый хак, подсмотренный в libxine.
    source * input = reinterpret_cast<source *>(id);
    input->seek(-1, SEEK_CUR);
    return c;
}

static uint32_t get_length( void * id )
{
    source * input = reinterpret_cast<source *>(id);
    u32 offset = input->tell();
    input->seek(0, SEEK_END);
    u32 size = input->tell();
    input->seek(offset, SEEK_SET);
    return size;
}

static int can_seek( void * )
{
    return 1;
}

static int32_t write_bytes( void *, void *, int32_t )
{
    return 0;
}

static WavpackStreamReader gCallbacks = {
    read_bytes, get_pos, set_pos_abs, 
    set_pos_rel, push_back_byte, get_length, 
    can_seek, write_bytes
};

}


wavpack_decoder::wavpack_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferRealSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // Буфер для ошибок при разборе wv-файла, количество символов - из документации.
    char error[80];
    mWavpackFile = WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, error, OPEN_NORMALIZE, 0);
    if (NULL == mWavpackFile)
        throw std::runtime_error(error);

    // Получаем количество каналов.
    mChannelCount = WavpackGetNumChannels(mWavpackFile);
    if (mChannelCount > 2)
        throw std::runtime_error("Too many channels in wav-file");

    // На всякий случай проверяем битность.
    assert(WavpackGetBitsPerSample(mWavpackFile) == 16);

    // Получаем частоту.
    mSampleRate = WavpackGetSampleRate(mWavpackFile);

    // Количество семплов
    mSampleCount = WavpackGetNumSamples(mWavpackFile);
    
    // Размер буфера определяется экспериментальным путем
    mBufferSize = mSampleCount > CHANNEL_BUFFER_SIZE_IN_SAMPLES * LATENCY_MSEC ? CHANNEL_BUFFER_SIZE_IN_SAMPLES * LATENCY_MSEC : mSampleCount;

    // Магическое домножение
    mBufferSize *= 2 * mChannelCount;

    mBuffer = new s32[mBufferSize + 1];
}

u32 wavpack_decoder::unpack_wavpack()
{
    return WavpackUnpackSamples(mWavpackFile, reinterpret_cast<int32_t*>(mBuffer), mBufferSize / mChannelCount) * mChannelCount;
}

void wavpack_decoder::seek( u32 sample )
{
    // Если ошибка, значит надо открывать файл заново!
    if (!WavpackSeekSample(mWavpackFile, sample))
    {
        // Открываем файл заново
        WavpackCloseFile(mWavpackFile);
        char error[80];
        mSource->seek(0, SEEK_SET);
        mWavpackFile = WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, error, OPEN_NORMALIZE, 0);
        if (NULL == mWavpackFile)
            throw std::runtime_error(error);
        // Пытаемся сделать seek еще раз
        if (!WavpackSeekSample(mWavpackFile, sample))
            throw std::runtime_error("Seeking error.");
    }
    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    // Обнуляем буфер
    mBufferRealSize = 0;
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 wavpack_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    u32 samplesCount = 0; 
    s32 * ptr = mBuffer + mBufferOffset;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        if (mBufferOffset >= mBufferRealSize)
        {
            // Пытаемся подгрузить еще данных в буфер
            mBufferOffset = 0;
            ptr = mBuffer;
            if (0 == (mBufferRealSize = unpack_wavpack()))
                break;
        }
        f32 sample = *left++ = *ptr++ / static_cast<f32>(1 << 15);
        *right++ = (mChannelCount == 1) ? sample : *ptr++ / static_cast<f32>(1 << 15);
        mBufferOffset += mChannelCount;
        if (++samplesCount == count)
            break;
    }
    return samplesCount;
}


wavpack_decoder::~wavpack_decoder()
{
    WavpackCloseFile(mWavpackFile);
    delete [] mBuffer;
}
