/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� ����� �� wavpack-�����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace
{

// ������ �� ���������.
int32_t read_bytes( void * id, void * data, int32_t bcount )
{
    return reinterpret_cast<source *>(id)->read(data, 1, bcount);
}

// ��������� ������� ������.
uint32_t get_pos( void * id )
{
    return reinterpret_cast<source *>(id)->tell();
}

// ��������� ����������� ��������.
int32_t set_pos_abs( void * id, uint32_t pos )
{
    reinterpret_cast<source *>(id)->seek(pos, SEEK_SET);
    return 0;
}

// ��������� �������������� ��������.
int32_t set_pos_rel( void * id, int32_t delta, int32_t mode )
{
    reinterpret_cast<source *>(id)->seek(delta, mode);
    return 0;
}

// ����������� �� ���� ������ ����� (ungetc).
int32_t push_back_byte( void * id, int32_t c )
{
    reinterpret_cast<source *>(id)->seek(-1, SEEK_CUR);
    return c;
}

// ��������� ����� ������.
uint32_t get_length( void * id )
{
    source * input = reinterpret_cast<source *>(id);
    u32 offset = input->tell();
    input->seek(0, SEEK_END);
    u32 size = input->tell();
    input->seek(offset, SEEK_SET);
    return size;
}

// �������� �� ����������� �� �����.
int32_t can_seek( void * )
{
    return 1;
}

// ������ � �����, �� ��������������.
int32_t write_bytes( void *, void *, int32_t )
{
    return 0;
}

// ������� ��� ������ ������ ���������.
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

// ���������� �������� ��� �������� ������ �� ������� ������.
const u32 MAGIC = 0x900d900d;

} // namespace

// �����������.
wavpack_decoder::wavpack_decoder( std::auto_ptr<source> source )
    : mSource(source), mSampleRate(0), mSampleCount(0),
      mChannelCount(0), mBuffer(NULL), mBufferSize(0),
      mBufferFilledSize(0), mBufferOffset(0)
{
    assert(mSource.get());

    // ����� ��� ������ ��� ������� wv-�����, ������ � 80 �������� - �� ������������.
    char errorBuffer[80];
    mWavpackFile = WavpackOpenFileInputEx(&gCallbacks, mSource.get(), NULL, errorBuffer, 0, 0);
    if (NULL == mWavpackFile)
        throw std::runtime_error(errorBuffer);

    // �������� ���������� �������.
    mChannelCount = WavpackGetNumChannels(mWavpackFile);
    if (mChannelCount > 2)
    {
        WavpackCloseFile(mWavpackFile);
        throw std::runtime_error("Expected mono or stereo stream.");
    }
    
    // ��������� ��������.
    if (WavpackGetBitsPerSample(mWavpackFile) != 16)
    {
        WavpackCloseFile(mWavpackFile);
        throw std::runtime_error("Unsupported sample format (expected 16 bits).");
    }
    
    // �������� �������.
    mSampleRate = WavpackGetSampleRate(mWavpackFile);

    // ����� ����� � �������.
    mSampleCount = WavpackGetNumSamples(mWavpackFile);
    
    // ����� �������������.
    const u32 BUFFER_SIZE = mSampleRate * DECODE_BUFFER_SIZE_IN_MSEC / 1000;
    mBufferSize = (mSampleCount < BUFFER_SIZE ? mSampleCount : BUFFER_SIZE) * mChannelCount;
    mBuffer = new s32[mBufferSize + 1];
    mBuffer[mBufferSize] = MAGIC; // �������� ��� �������� ������ �� �������.
}

// ������������� ������ �� ���������� �����. ���������� ���������� �������������� �������.
u32 wavpack_decoder::unpack_to_buffer()
{
    return WavpackUnpackSamples(mWavpackFile, reinterpret_cast<int32_t *>(mBuffer),
                                mBufferSize / mChannelCount) * mChannelCount;
}

// ��������� ������� ������ ������������� �� �������� �����.
void wavpack_decoder::seek( u32 sample )
{
    // ���� ������, ������ ���� ��������� ���� ������!
    if (!WavpackSeekSample(mWavpackFile, sample))
    {
        WavpackCloseFile(mWavpackFile);
        delete [] mBuffer;
        throw std::runtime_error("Seek failed");
    }
    // ���������� ��������� �� ������� �����
    mBufferOffset = 0;
    mBufferFilledSize = 0;
}

// �������� � ����� � ������ ������ count �������������� �������.
u32 wavpack_decoder::unpack( f32 * left, f32 * right, u32 count )
{
    s32 * ptr = mBuffer + mBufferOffset;
    
    u32 sampleCount = 0;
    while (sampleCount < count)
    {
        if (mBufferOffset >= mBufferFilledSize)
        {
            assert(mBufferOffset == mBufferFilledSize);
            
            // �������� ���������� ��� ������ � �����
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
    assert(mBuffer[mBufferSize] == MAGIC); // ��������� ����� �� ������� ������.
    return sampleCount;
}

// ����������.
wavpack_decoder::~wavpack_decoder()
{
    WavpackCloseFile(mWavpackFile);
    delete [] mBuffer;
}
