/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 Реализация источника звука из wav-файла.
 */
#include <flamenco/flamenco.h>

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
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferRealSize(0), mBufferOffset(0)
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

    // Размер буфера определяется экспериментальным путем
    mBufferSize = mSampleCount > CHANNEL_BUFFER_SIZE_IN_SAMPLES * LATENCY_MSEC ? CHANNEL_BUFFER_SIZE_IN_SAMPLES * LATENCY_MSEC : mSampleCount;

	// Магическое домножение
	mBufferSize *= 2 * mChannelCount;

    mBuffer = new s16[mBufferSize];
}

void ogg_decoder::seek( u32 sample )
{
    if (ov_pcm_seek(mVorbisFile, sample * mChannelCount) != 0)
        throw std::runtime_error("Seeking error.");
    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    // Обнуляем буфер
    mBufferRealSize = 0;
}

// Распаковывает из vorbis потока count семплов во внутренний буфер начиная со смещения offset 
// Возвращает количество прочитанных семплов
u32 ogg_decoder::unpack_vorbis(s16 * dst, u32 offset, u32 count)
{
	int currentSection;
	const u32 ORIGINAL_OFFSET = offset;

    while (offset < count)
    {
        int result = ov_read(mVorbisFile, reinterpret_cast<char *>(dst + offset / sizeof(s16)), (count - offset) , 0, sizeof(s16), 1, &currentSection);
        if (result == 0)
            break;
        else if (result > 0)
			offset += result;
    }
    return (offset - ORIGINAL_OFFSET) / sizeof(s16);
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 ogg_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    u32 samplesCount = 0; 
    s16 * ptr = mBuffer + mBufferOffset;

	for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
	{
		if (mBufferOffset >= mBufferRealSize)
		{
			// Пытаемся подгрузить еще данных в буфер
			mBufferOffset = 0;
			ptr = mBuffer;
			if (0 == (mBufferRealSize = unpack_vorbis(mBuffer, 0, mBufferSize)))
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

ogg_decoder::~ogg_decoder()
{
    ov_clear(mVorbisFile);
    delete [] mBuffer;
}
