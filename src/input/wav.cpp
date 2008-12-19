/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника звука из wav-файла.
 */
#include <flamenco/flamenco.h>
#include <fstream>

using namespace flamenco;

// Создание источника звука из wav-файла.
Wav::Wav( const char * path )
    : looping(false), mSamplesCount(0), mSamples(NULL), 
      mChannels(0), mCurrentSample(0)
{
    std::ifstream inputFile;
    inputFile.exceptions(std::ios::failbit);
    inputFile.open(path, std::ios_base::binary);
    
    // Получаем общий размер файла.
    inputFile.seekg(0, std::ios_base::end);
    u32 bufferSize  = inputFile.tellg();
    inputFile.seekg(0);
    
    // Считываем заголовок.
    mBuffer = new u8[bufferSize];
    inputFile.read(reinterpret_cast<char *>(mBuffer), bufferSize);
    
    // Проверка правильности файла.
    u32 * ptr = reinterpret_cast<u32 *>(mBuffer);
    if (*ptr != 0x46464952 /* "RIFF" */)
        throw std::runtime_error("Missing RIFF signature");
    if (*(ptr + 2) != 0x45564157 /* "WAVE" */)
        throw std::runtime_error("Missing WAVE header");
    ptr += 3;
    if (*ptr != 0x20746d66 /* "fmt " */)
        throw std::runtime_error("Missing 'fmt' block");
    if (*(u16 *)(ptr + 2) != 1 /* PCM (uncompressed) */)
        throw std::runtime_error("Compression is not supported");
    if (*((u16 *)(ptr) + 11) != 16 /* 16 bit per sample */)
        throw std::runtime_error("Only 16 bit per sample format is supported");
    
    // Получаем количество каналов.
    mChannels = *((u16 *)(ptr) + 5);
    if (mChannels != 1 && mChannels != 2)
        throw std::runtime_error("Too many channels in wav-file");
    
    // Ищем секцию 'data'.
    u32 sizeRead = 20 + *(ptr + 1);
    u8 * p = (u8 *)mBuffer + sizeRead;
    
    while (*(u32 *)p != 0x61746164 /* "data" */)
    {
        assert(sizeRead < bufferSize);
        sizeRead += *(u32 *)(p + 4) + 8;
        p = (u8 *)mBuffer + sizeRead;
    }
    
    mSamples = (s16 *)(mBuffer + sizeRead + 4);
    mSamplesCount = ((bufferSize - sizeRead - 4) / 2);
}

// Заполняем буфер новыми данными из wav файла.
void Wav::process( f32 * left, f32 * right )
{
    bool looping = this->looping.value();
    if (!looping && mCurrentSample > mSamplesCount)
        return;
    
    s16 * ptr = mSamples + mCurrentSample;
    
    for (u32 t = 0; t < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++t)
    {
        f32 sample = *left++ = static_cast<f32>(*ptr++) / (1 << 16);
        *right++ = (mChannels == 1) ? sample : static_cast<f32>(*ptr++) / (1 << 16);
        
        mCurrentSample += mChannels;
        if (mCurrentSample > mSamplesCount)
        {
            if (looping)
            {
                mCurrentSample = 0;
                ptr = mSamples;
                continue;
            }
            break;
        }
    }
}

// Деструктор
Wav::~Wav()
{
    delete [] mBuffer;
}

// Создание источника звука из wav-файла.
reference<Wav> Wav::create( const char * path )
{
    return reference<Wav>(new Wav(path));
}
