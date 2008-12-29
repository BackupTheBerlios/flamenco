/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wavpack-файла.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace
{

// Чтение из источника.
int32_t read_bytes( void * id, void * data, int32_t bcount )
{
    return reinterpret_cast<source *>(id)->read(data, 1, bcount);
}

// Получение позиции чтения.
uint32_t get_pos( void * id )
{
    return reinterpret_cast<source *>(id)->tell();
}

// Установка абсолютного смещения.
int32_t set_pos_abs( void * id, uint32_t pos )
{
    reinterpret_cast<source *>(id)->seek(pos, SEEK_SET);
    return 0;
}

// Установка относительного смещения.
int32_t set_pos_rel( void * id, int32_t delta, int32_t mode )
{
    reinterpret_cast<source *>(id)->seek(delta, mode);
    return 0;
}

// Перемещение на один символ назад (ungetc).
int32_t push_back_byte( void * id, int32_t c )
{
    reinterpret_cast<source *>(id)->seek(-1, SEEK_CUR);
    return c;
}

// Получение длины потока.
uint32_t get_length( void * id )
{
    source * input = reinterpret_cast<source *>(id);
    u32 offset = input->tell();
    input->seek(0, SEEK_END);
    u32 size = input->tell();
    input->seek(offset, SEEK_SET);
    return size;
}

// Возможно ли перемещение по файлу.
int32_t can_seek( void * )
{
    return 1;
}

// Запись в поток, не поддерживается.
int32_t write_bytes( void *, void *, int32_t )
{
    return 0;
}

// Функции для чтения данных декодером.
WavpackStreamReader gCallbacks = {
    read_bytes,
    get_pos,
    set_pos_abs,
    set_pos_rel,
    push_back_byte,
    get_length, 
    can_seek,
    write_bytes
};

// Магическое значение для проверки выхода за границы буфера.
const u32 MAGIC = 0x900d900d;

} // namespace

// Конструктор.
wavpack_decoder::wavpack_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferFilledSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // Буфер для ошибок при разборе wv-файла, размер в 80 символов - из документации.
    char errorBuffer[80];
    mWavpackFile = WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, errorBuffer, 0, 0);
    if (NULL == mWavpackFile)
        throw std::runtime_error(errorBuffer);

    // Получаем количество каналов.
    mChannelCount = WavpackGetNumChannels(mWavpackFile);
    if (mChannelCount > 2)
    {
        WavpackCloseFile(mWavpackFile);
        throw std::runtime_error("Expected mono or stereo stream.");
    }
    
    // Проверяем битность.
    if (WavpackGetBitsPerSample(mWavpackFile) != 16)
    {
        WavpackCloseFile(mWavpackFile);
        throw std::runtime_error("Unsupported sample format (expected 16 bits).");
    }
    
    // Получаем частоту.
    mSampleRate = WavpackGetSampleRate(mWavpackFile);

    // Длина звука в семплах.
    mSampleCount = WavpackGetNumSamples(mWavpackFile);
    
    // Буфер декодирования.
    const u32 BUFFER_SIZE = mSampleRate * DECODE_BUFFER_SIZE_IN_MSEC / 1000;
    mBufferSize = (mSampleCount < BUFFER_SIZE ? mSampleCount : BUFFER_SIZE) * mChannelCount;
    mBuffer = new s32[mBufferSize + 1];
    mBuffer[mBufferSize] = MAGIC; // Значение для проверки выхода за границы.
}

// Декодирование потока во внутренний буфер. Возвращает количество декодированных семплов.
u32 wavpack_decoder::unpack_to_buffer()
{
    return WavpackUnpackSamples(mWavpackFile, reinterpret_cast<int32_t *>(mBuffer),
                                mBufferSize / mChannelCount) * mChannelCount;
}

// Установка курсора начала декодирования на заданный семпл.
void wavpack_decoder::seek( u32 sample )
{
    // Если ошибка, значит надо открывать файл заново!
    if (!WavpackSeekSample(mWavpackFile, sample))
    {
        WavpackCloseFile(mWavpackFile);
        delete [] mBuffer;
        throw std::runtime_error("Seek failed");
    }
    // Сбрасываем указатель на текущий семпл
    mBufferOffset = 0;
    mBufferFilledSize = 0;
}

// Копирует в левый и правый каналы count декодированных семплов.
u32 wavpack_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    s32 * ptr = mBuffer + mBufferOffset;
    
    u32 sampleCount = 0;
    while (sampleCount < count)
    {
        if (mBufferOffset >= mBufferFilledSize)
        {
            assert(mBufferOffset == mBufferFilledSize);
            
            // Пытаемся подгрузить еще данных в буфер
            mBufferOffset = 0;
            ptr = mBuffer;
            if (0 == (mBufferFilledSize = unpack_to_buffer()))
                break;
        }
        f32 sample = *left++ = *ptr++ / static_cast<f32>(1 << 15);
        *right++ = (mChannelCount == 1) ? sample : *ptr++ / static_cast<f32>(1 << 15);
        mBufferOffset += mChannelCount;
        sampleCount++;
    }
    assert(mBuffer[mBufferSize] == MAGIC); // Проверяем выход за границы буфера.
    return sampleCount;
}

// Деструктор.
wavpack_decoder::~wavpack_decoder()
{
    WavpackCloseFile(mWavpackFile);
    delete [] mBuffer;
}
