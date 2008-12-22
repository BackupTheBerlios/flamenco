/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� ����� �� wav-�����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// ��������� �������� ��� �������� ������ �� ������� �������.
const s16 Wav::MAGIC = 0x900dU;
// ������������ ������ ������ � �������
const u32 Wav::BUFFER_SIZE_IN_SAMPLES = 1 << 16;

// �������� ��������� ����� �� wav-�����.
Wav::Wav( const char * path )
    : mSamples(NULL), mSampleCount(0), mSamplesCurrent(0),
      mChannels(0), looping(false), mInputOffset(0), mIsFinished(false)
{
    if (NULL == (mInput = fopen(path, "rb")))
        throw std::runtime_error("Can`t open file");
    
    // �������� ����� ������ �����.
    fseek(mInput, 0, SEEK_END);
    u32 inputSize = ftell(mInput);
    fseek(mInput, 0, SEEK_SET);

    // ����� ��� ���������
    u8 headerBuffer[256];
    memset(headerBuffer, 0, sizeof(headerBuffer));

    // ��������� ���������.
    mInputOffset = fread(headerBuffer, 1, sizeof(headerBuffer), mInput);
    assert(mInputOffset == sizeof(headerBuffer));
    
    // �������� ������������ �����.
    u32 * ptr = reinterpret_cast<u32 *>(headerBuffer);
    if (*ptr != 'FFIR' /* "RIFF" */)
        throw std::runtime_error("Missing RIFF signature");
    if (*(ptr + 2) != 'EVAW' /* "WAVE" */)
        throw std::runtime_error("Missing WAVE header");
    ptr += 3;
    if (*ptr != ' tmf' /* "fmt " */)
        throw std::runtime_error("Missing 'fmt' block");
    if (*(u16 *)(ptr + 2) != 1 /* PCM (uncompressed) */)
        throw std::runtime_error("Compression is not supported");
    if (*((u16 *)(ptr) + 11) != 16 /* 16 bit per sample */)
        throw std::runtime_error("Only 16 bit per sample format is supported");
    
    // �������� ���������� �������.
    mChannels = *((u16 *)(ptr) + 5);
    if (mChannels != 1 && mChannels != 2)
        throw std::runtime_error("Too many channels in wav-file");
    mFrequency = *((u16 *)(ptr) + 6);
    
    // ���� ������ 'data'.
    mInputOffset = 20 + *(ptr + 1);
    assert(mInputOffset < inputSize);

    fseek(mInput, mInputOffset, SEEK_SET);

    for(;;)
    {
        assert(mInputOffset < inputSize);
        u32 p = '    ';
        mInputOffset += fread(&p, 1, sizeof(p), mInput);
        if (p == 'atad' /* "data" */) 
            break;
    }

    mInputOffset += 4;
    fseek(mInput, mInputOffset, SEEK_SET);

    // ������� ����� ��� ������ �����
    mSamples = new s16[BUFFER_SIZE_IN_SAMPLES + 1];
    mSamples[BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mSampleCount = fread(mSamples, sizeof(s16), BUFFER_SIZE_IN_SAMPLES, mInput);
}

// ������ �� ����� � ����� � ����������� �� ����� looping
void Wav::fill(bool looping)
{
    // �������� ��������� ���� ����� �� ���
    mSampleCount = fread(mSamples, sizeof(s16), BUFFER_SIZE_IN_SAMPLES, mInput);

    if (looping)
    {
        // ���� ��������� ��� ������ ������, ������ ������� ������ ������
        while (mSampleCount < BUFFER_SIZE_IN_SAMPLES)
        {
            // ��������� �� ������ ������ � �����
            fseek(mInput, mInputOffset, SEEK_SET);
            // ������ ��������� ������
            mSampleCount += fread(mSamples + mSampleCount, sizeof(s16), BUFFER_SIZE_IN_SAMPLES - mSampleCount, mInput);
        }
    }
    mIsFinished = (mSampleCount < BUFFER_SIZE_IN_SAMPLES && !looping);

    // �������� ������ �� ������� �������
    assert(MAGIC == mSamples[BUFFER_SIZE_IN_SAMPLES]);
};

// ��������� ����� � ������ ������ �� ����������� ������.
void Wav::process( f32 * left, f32 * right )
{   
    bool looping = this->looping();

    if (!looping && mIsFinished)
        return;

    const s16 * ptr = mSamples + mSamplesCurrent;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        // ���� ����� ������� � �����
        if (mSamplesCurrent >= mSampleCount)
        {
            // �������� �������
            mSamplesCurrent = 0;
            ptr = mSamples;
            // ��������� ���������� �����
            fill(looping);
        }
        f32 sample = *left++ = *ptr++ / static_cast<f32>(1 << 16);
        *right++ = (mChannels == 1) ? sample : *ptr++ / static_cast<f32>(1 << 16);
        mSamplesCurrent += mChannels;
    }
}

// ����������
Wav::~Wav()
{
    delete [] mSamples;
    fclose(mInput);
}

// �������� ��������� ����� �� wav-�����.
reference<Wav> Wav::create( const char * path )
{
    return reference<Wav>(new Wav(path));
}
