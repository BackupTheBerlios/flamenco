/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� ����� �� wv-�����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// ��������� �������� ��� �������� ������ �� ������� �������.
const s32 WavPack::MAGIC = 0x900d;
// ������������ ������ ������ � �������
const u32 WavPack::BUFFER_SIZE_IN_SAMPLES = 1 << 16;

// �������� ��������� ����� �� wv-�����.
WavPack::WavPack( const char * path )
    : mSamples(NULL), mSamplesCount(0), mSamplesCurrent(0),
      mChannels(0), looping(false), mIsFinished(false)
{
    // ����� ��� ������ ��� ������� wv-�����, ���������� �������� - �� ������������.
    char error[80];
    mInput = WavpackOpenFileInput(path, error, OPEN_NORMALIZE, 0);
    if (NULL == mInput)
        throw std::runtime_error(error);

    // �������� ���������� �������.
    mChannels = WavpackGetNumChannels(mInput);
    if (mChannels != 1 && mChannels != 2)
        throw std::runtime_error("Too many channels in wav-file");

    // �� ������ ������ ��������� ��������.
    assert(WavpackGetBitsPerSample(mInput) == 16);

    // �������� �������.
    mFrequency = WavpackGetSampleRate(mInput);

    // ������� ����� ��� ������ �����.
    mSamples = new s32[BUFFER_SIZE_IN_SAMPLES + 1];
    mSamples[BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mSamplesCount = unpack(mSamples, 0, BUFFER_SIZE_IN_SAMPLES);
}

// ���������� ����� ������ ������ � �����
u32 WavPack::unpack(s32 * dst, u32 offset, u32 size)
{
    return WavpackUnpackSamples(mInput, reinterpret_cast<int32_t*>(dst + offset), (size - offset) / mChannels) * mChannels;
}

// ������ �� ����� � ����� � ����������� �� ����� looping
void WavPack::fill(bool looping)
{
    // �������� ��������� ���� ����� �� ���
    mSamplesCount = unpack(mSamples, 0, BUFFER_SIZE_IN_SAMPLES);
    
    if (looping)
    {
        // ���� ��������� ��� ������ ������, ������ ������� ������ ������
        while (mSamplesCount < BUFFER_SIZE_IN_SAMPLES)
        {
            // ��������� �� ������ ������ � �����
            WavpackSeekSample(mInput, 0);
            // ������ ��������� ������
            mSamplesCount += unpack(mSamples, mSamplesCount, BUFFER_SIZE_IN_SAMPLES);
        }
    }
    mIsFinished = (mSamplesCount < BUFFER_SIZE_IN_SAMPLES && !looping);
    
    // �������� ������ �� ������� �������
    assert(MAGIC == mSamples[BUFFER_SIZE_IN_SAMPLES]);
};

// ��������� ����� � ������ ������ �� ����������� ������.
void WavPack::process( f32 * left, f32 * right )
{   
    bool looping = this->looping();

    if (!looping && mIsFinished)
        return;

    const s32 * ptr = mSamples + mSamplesCurrent;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        // ���� ����� ������� � �����
        if (mSamplesCurrent >= mSamplesCount)
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
WavPack::~WavPack()
{
    delete [] mSamples;
    WavpackCloseFile(mInput);
}

// �������� ��������� ����� �� wv-�����.
reference<WavPack> WavPack::create( const char * path )
{
    return reference<WavPack>(new WavPack(path));
}
