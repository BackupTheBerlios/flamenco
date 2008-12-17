#include <flamenco/flamenco.h>
#include <fstream>
#include <iostream>

using namespace flamenco;

// Открываем файл на чтение
Wav::Wav( const char * path, bool looped )
    : mSamplesCount(0), mSamples(NULL), mIsLooped(looped), 
      mChannels(0), mCurrentSample(0)
      
     
{
    std::ifstream inputFile;
    inputFile.exceptions(std::ios::failbit);
    inputFile.open(path, std::ios_base::binary);

    // Получаем общий размер файла
    inputFile.seekg(-1, std::ios_base::end);
    u32 bufferSize  = inputFile.tellg();
    inputFile.seekg(0);
    
    // Считываем заголовок
    mBuffer = new u8[bufferSize];
    inputFile.read(reinterpret_cast<char *>(mBuffer), bufferSize);

    u32 * ptr = (u32 *)mBuffer;

    // Различные проверки формата
    assert(*ptr == 0x46464952 /* "RIFF" */);
    assert(*(ptr + 2) == 0x45564157 /* "WAVE" */);
    ptr += 3;
    assert(*ptr == 0x20746d66 /* "fmt " */);
    assert(*(u16 *)(ptr + 2) == 1 /* PCM (uncompressed) */);

    assert(*((u16 *)(ptr) + 11) == 16 /* 16 bit per sample */);

    // Получаем количество каналов
    mChannels = *((u16 *)(ptr) + 5);
    assert(mChannels == 1 || mChannels == 2);

    // Looking for 'data' section
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

// Заполняем буфер новыми данными из wav файла
void Wav::process( s16 * left, s16 * right )
{
    if (!mIsLooped && mCurrentSample > mSamplesCount)
        return;
        
    s16 * ptr = mSamples + mCurrentSample;
    
    for (u32 t = 0; t < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++t)
    {
        s16 sample = *left++ = *ptr++;
        *right++ = (mChannels == 1) ? sample : *ptr++;
        
        mCurrentSample += mChannels;
        if (mCurrentSample > mSamplesCount)
        {
            if (mIsLooped)
            {
                std::cout << "RESET!\n";
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

// Открытие wav файла
reference<Wav> Wav::create( const char * path, bool looped )
{
    return reference<Wav>(new Wav(path, looped));
}
