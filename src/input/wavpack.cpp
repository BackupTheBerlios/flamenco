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

int set_pos_abs( void *id, uint32_t pos )
{
	source * input = reinterpret_cast<source *>(id);
	input->seek(pos, SEEK_SET);
	return 0;
}

int set_pos_rel( void *id, int32_t delta, int mode )
{
	source * input = reinterpret_cast<source *>(id);
	input->seek(delta, mode);
	return 0;
}

int push_back_byte( void *, int )
{
	return EOF;
}

uint32_t get_length( void *id )
{
	source * input = reinterpret_cast<source *>(id);
	u32 offset = input->tell();
	input->seek(0, SEEK_END);
	u32 size = input->tell();
	input->seek(offset, SEEK_SET);
	return size;
}

int can_seek( void * )
{
	return true;
}

int32_t write_bytes( void *, void *, int32_t )
{
	return 0;
}

}

wavpack_decoder::wavpack_decoder( std::auto_ptr<source> source )
	: mSource(source), mSampleRate(0), mSampleCount(0),
	  mChannelCount(0), mBuffer(NULL), mBufferSize(0),
	  mBufferRealSize(0), mBufferOffset(0)
{
	assert(mSource.get());

	WavpackStreamReader cb =
		{ read_bytes, get_pos, set_pos_abs, set_pos_rel, push_back_byte, get_length, can_seek, write_bytes };


	// Буфер для ошибок при разборе wv-файла, количество символов - из документации.
	char error[80];
	mWavpackFile = WavpackOpenFileInputEx(&cb, mSource.get(), NULL, error, OPEN_NORMALIZE, 0);
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

	mSampleCount = WavpackGetNumSamples(mWavpackFile);

	// Создаем буфер для чтения файла.
/*	mSamples = new s32[BUFFER_SIZE_IN_SAMPLES + 1];
	mSamples[BUFFER_SIZE_IN_SAMPLES] = MAGIC;
	mSamplesCount = unpack(mSamples, 0, BUFFER_SIZE_IN_SAMPLES);
	*/

}

wavpack_decoder::~wavpack_decoder()
{
	WavpackCloseFile(mWavpackFile);
	delete [] mBuffer;
}

#if 0

// Волшебное значение для проверки выхода за границы буферов.
const s32 wavpack::MAGIC = 0x900d;
// Максимальный размер буфера в семплах
const u32 wavpack::BUFFER_SIZE_IN_SAMPLES = 1 << 16;

// Создание источника звука из wv-файла.
wavpack::wavpack( const char * path )
    : mSamples(NULL), mSamplesCount(0), mSamplesCurrent(0),
      mChannels(0), looping(false), mIsFinished(false)
{
	assert(mSource.get());

	WavpackStreamReader cb =
	{
		read_bytes, get_pos, set_pos_abs, set_pos_rel, push_back_byte, get_length, can_seek, write_bytes
	};

	/*
    // Буфер для ошибок при разборе wv-файла, количество символов - из документации.
    char error[80];
    mInput = WavpackOpenFileInput(path, error, OPEN_NORMALIZE, 0);
    if (NULL == mInput)
        throw std::runtime_error(error);

    // Получаем количество каналов.
    mChannels = WavpackGetNumChannels(mInput);
    if (mChannels != 1 && mChannels != 2)
        throw std::runtime_error("Too many channels in wav-file");

    // На всякий случай проверяем битность.
    assert(WavpackGetBitsPerSample(mInput) == 16);

    // Получаем частоту.
    mFrequency = WavpackGetSampleRate(mInput);

    // Создаем буфер для чтения файла.
    mSamples = new s32[BUFFER_SIZE_IN_SAMPLES + 1];
    mSamples[BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mSamplesCount = unpack(mSamples, 0, BUFFER_SIZE_IN_SAMPLES);
	*/
}

// Распаковка одной порции данных в буфер
u32 wavpack::unpack(s32 * dst, u32 offset, u32 size)
{
    return WavpackUnpackSamples(mInput, reinterpret_cast<int32_t*>(dst + offset), (size - offset) / mChannels) * mChannels;
}

// Читаем из файла в буфер в зависимости от флага looping
void wavpack::fill(bool looping)
{
    // Пытаемся заполнить весь буфер за раз
    mSamplesCount = unpack(mSamples, 0, BUFFER_SIZE_IN_SAMPLES);
    
    if (looping)
    {
        // Цикл необходим для чтения файлов, размер которых меньше буфера
        while (mSamplesCount < BUFFER_SIZE_IN_SAMPLES)
        {
            // Переходим на начало данных в файле
            WavpackSeekSample(mInput, 0);
            // Читаем очередную порцию
            mSamplesCount += unpack(mSamples, mSamplesCount, BUFFER_SIZE_IN_SAMPLES);
        }
    }
    mIsFinished = (mSamplesCount < BUFFER_SIZE_IN_SAMPLES && !looping);
    
    // Проверка выхода за пределы массива
    assert(MAGIC == mSamples[BUFFER_SIZE_IN_SAMPLES]);
};

// Заполняем левый и правый каналы из внутреннего буфера.
void wavpack::process( f32 * left, f32 * right )
{   
    bool looping = this->looping();

    if (!looping && mIsFinished)
        return;

    const s32 * ptr = mSamples + mSamplesCurrent;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        // Если буфер подошел к концу
        if (mSamplesCurrent >= mSamplesCount)
        {
            // Начинаем сначала
            mSamplesCurrent = 0;
            ptr = mSamples;
            // Заполняем внутренний буфер
            fill(looping);
        }
        f32 sample = *left++ = *ptr++ / static_cast<f32>(1 << 16);
        *right++ = (mChannels == 1) ? sample : *ptr++ / static_cast<f32>(1 << 16);
        
        mSamplesCurrent += mChannels;
    }
}

// Деструктор
wavpack::~wavpack()
{
    delete [] mSamples;
    WavpackCloseFile(mInput);
}

// Создание источника звука из wv-файла.
reference<wavpack> wavpack::create( const char * path )
{
    return reference<wavpack>(new wavpack(path));
}
#endif