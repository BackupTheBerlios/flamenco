/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wv-файла.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Волшебное значение для проверки выхода за границы буферов.
const s32 WavPack::smMagic = 0x900d;
// Максимальный размер буфера в семплах
const u32 WavPack::smBufferSizeInSamples = 1 << 16;

// Создание источника звука из wv-файла.
WavPack::WavPack( const char * path )
    : mSamples(NULL), mSamplesCount(0), mSamplesCurrent(0),
      mChannels(0), looping(false), mIsFinished(false)
{
    // Буфер для ошибок при разборе wv-файла, количество символов - из документации.
    char error[80];
    mInput = WavpackOpenFileInput(path, error, OPEN_NORMALIZE, 0);
    if (error)
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
    mSamples = new s32[smBufferSizeInSamples + 1];
    mSamples[smBufferSizeInSamples] = smMagic;
    mSamplesCount = WavpackUnpackSamples(mInput, reinterpret_cast<int32_t *>(mSamples), smBufferSizeInSamples);
}

// Читаем из файла в буфер в зависимости от флага looping
void WavPack::fill(bool looping)
{

    // Пытаемся заполнить весь буфер за раз
    mSamplesCount = WavpackUnpackSamples(mInput, reinterpret_cast<int32_t *>(mSamples), smBufferSizeInSamples);

    if (looping)
    {
        // Цикл необходим для чтения файлов, размер которых меньше буфера
        while (mSamplesCount < smBufferSizeInSamples)
        {
            // Переходим на начало данных в файле
            WavpackSeekSample(mInput, 0);
            // Читаем очередную порцию
            mSamplesCount += WavpackUnpackSamples(mInput, reinterpret_cast<int32_t *>(mSamples + mSamplesCount), smBufferSizeInSamples - mSamplesCount);
        }
    }
    mIsFinished = (mSamplesCount < smBufferSizeInSamples && !looping);

    // Проверка выхода за пределы массива
    assert(smMagic == mSamples[smBufferSizeInSamples]);
};

// Заполняем левый и правый каналы из внутреннего буфера.
void WavPack::process( f32 * left, f32 * right )
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
WavPack::~WavPack()
{
    delete [] mSamples;
    WavpackCloseFile(mInput);
}

// Создание источника звука из wv-файла.
reference<WavPack> WavPack::create( const char * path )
{
    return reference<WavPack>(new WavPack(path));
}
