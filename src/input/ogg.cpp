/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ���������� ��������� ����� �� wav-�����.
 */
#include <flamenco/flamenco.h>

#include <iostream>

using namespace flamenco;

namespace 
{

// ��������������� ������ ��� ������ �����.
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


// �����������
ogg_decoder::ogg_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferRealSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // ������� � ��������� vorbis �����
    mVorbisFile = new OggVorbis_File;
    if (ov_open_callbacks(mSource.get(), mVorbisFile, NULL, -1, gCallbacks) < 0)
        throw std::runtime_error("File is not a valid ogg container.");

    // ���������� � ������ vorbis.
    mVorbisInfo = ov_info(mVorbisFile, -1);

    // ���������� �������.
    mChannelCount = mVorbisInfo->channels;
    assert(mChannelCount != 0);
    if (mChannelCount > 2)
        throw std::runtime_error("Expected mono or stereo stream.");

    // �������.
    mSampleRate = mVorbisInfo->rate;

    // ����� ���������� �������
    mSampleCount = static_cast<u32>(ov_pcm_total(mVorbisFile, -1)) / mChannelCount;

    // ������ ������ �� ���� �������
    mBufferSize = 2 * mSampleRate * mChannelCount;

    mBuffer = new f32[mBufferSize];
}

void ogg_decoder::seek( u32 sample )
{
    if (ov_pcm_seek(mVorbisFile, sample * mChannelCount) != 0)
        throw std::runtime_error("Seeking error.");
    // ���������� ��������� �� ������� �����
    mBufferOffset = 0;
    // �������� �����
    mBufferRealSize = 0;
}

// ������������� �� vorbis ������ count ������� �� ���������� �����.
// ���������� ���������� ����������� �������.
u32 ogg_decoder::unpack_vorbis()
{
    std::cout << "unpack_vorbis\n";
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
            f32 * ptr = mBuffer + readSamples / 2;
            readSamples += result * 2 * mChannelCount;
            
            if (mChannelCount == 1)
                // Mono
                memcpy(ptr, *pcm, result * sizeof(f32));
            else
            {
                // Stereo
                f32 *left = pcm[0], *right = pcm[1];
                for (int i = 0; i < result; ++i)
                {
                    *ptr++ = *left++;
                    *ptr++ = *right++;
                }
            }

        }
    }
    return readSamples / 2;
}

// �������� � ����� � ������ ������ count �������������� �������.
u32 ogg_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    u32 samplesCount = 0; 
    f32 * ptr = mBuffer + mBufferOffset;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        if (mBufferOffset >= mBufferRealSize)
        {
            // �������� ���������� ��� ������ � �����
            mBufferOffset = 0;
            ptr = mBuffer;
            if (0 == (mBufferRealSize = unpack_vorbis()))
                break;
        }
        f32 sample = *left++ = *ptr++;
        *right++ = (mChannelCount == 1) ? sample : *ptr++;
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
