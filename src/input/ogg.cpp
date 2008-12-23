/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ��������� ����� �� wav-�����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// ��������� �������� ��� �������� ������ �� ������� �������.
const s16 Ogg::MAGIC = 0x900dU;
// ������������ ������ ������ � �������
const u32 Ogg::BUFFER_SIZE_IN_SAMPLES = 1 << 16;

namespace
{
// ��������������� ������ ��� ������ �����
size_t read_func(void * ptr, size_t size, size_t count, void *datasource)
{
	FILE * f = reinterpret_cast<FILE *>(datasource);
	return fread(static_cast<char *>(ptr), size, count, f);
}

int seek_func(void * datasource, ogg_int64_t offset, int whence)
{
	FILE * f = reinterpret_cast<FILE *>(datasource);
	return fseek(f, static_cast<long>(offset), whence) == 0 ? 0 : -1;
}

long tell_func(void * datasource)
{
	FILE * f = reinterpret_cast<FILE *>(datasource);
	return ftell(f);
}

int close_func(void *datasource)
{
	FILE * f = reinterpret_cast<FILE *>(datasource);
	fclose(f);
	return 0;
}


};

// �������� ��������� ����� �� wav-�����.
Ogg::Ogg( const char * path )
    : mSamples(NULL), mSampleCount(0), mSamplesCurrent(0),
      mChannels(0), looping(false), mIsFinished(false)
{
	ov_callbacks cb;

	cb.close_func = close_func;
	cb.read_func  = read_func;
	cb.seek_func  = seek_func;
	cb.tell_func  = tell_func;

	if (NULL == (mInput = fopen(path, "rb")))
		throw std::runtime_error("Can`t open file");

	mVorbisFile = new OggVorbis_File;

	if (ov_open_callbacks(mInput, mVorbisFile, NULL, -1, cb) < 0)
		throw std::runtime_error("Can't open file");

	mVorbisInfo = ov_info(mVorbisFile, -1);

	mChannels  = mVorbisInfo->channels;
	if (mChannels != 1 && mChannels != 2)
		throw std::runtime_error("Too many channels in wav-file");

	mFrequency = mVorbisInfo->rate;

	// ������� ����� ��� ������ �����
	mSamples = new s16[BUFFER_SIZE_IN_SAMPLES + 1];
	mSamples[BUFFER_SIZE_IN_SAMPLES] = MAGIC;

	int current_section;	

	while (mSampleCount < BUFFER_SIZE_IN_SAMPLES)
	{
		int result = ov_read(mVorbisFile, reinterpret_cast<char *>(mSamples + mSampleCount / 2), (BUFFER_SIZE_IN_SAMPLES - mSampleCount) , 0, 2, 1, &current_section);
		if (result == 0)
			break;
		else if (result > 0)
			mSampleCount += result;
	}
	// �������� ������ �� ������� �������
	assert(MAGIC == mSamples[BUFFER_SIZE_IN_SAMPLES]);
}

// ������ �� ����� � ����� � ����������� �� ����� looping
void Ogg::fill(bool looping)
{
	int current_section;		

	mSampleCount = 0;
    // �������� ��������� ���� ����� �� ���
	while (mSampleCount < BUFFER_SIZE_IN_SAMPLES)
	{
		int result = ov_read(mVorbisFile, reinterpret_cast<char *>(mSamples + mSampleCount / 2), (BUFFER_SIZE_IN_SAMPLES - mSampleCount) , 0, 2, 1, &current_section);
		if (result == 0)
			break;
		else if (result > 0)
			mSampleCount += result;
	}

    if (looping)
    {
        // ���� ��������� ��� ������ ������, ������ ������� ������ ������
        while (mSampleCount < BUFFER_SIZE_IN_SAMPLES)
        {
            // ��������� �� ������ ������ � �����
			ov_pcm_seek(mVorbisFile, 0);
            // ������ ��������� ������
			while (mSampleCount < BUFFER_SIZE_IN_SAMPLES)
			{
				int result = ov_read(mVorbisFile, reinterpret_cast<char *>(mSamples + mSampleCount / 2), (BUFFER_SIZE_IN_SAMPLES - mSampleCount) , 0, 2, 1, &current_section);
				if (result == 0)
					break;
				else if (result > 0)
					mSampleCount += result;
			}
        }
    }
    mIsFinished = (mSampleCount < BUFFER_SIZE_IN_SAMPLES && !looping);

    // �������� ������ �� ������� �������
    assert(MAGIC == mSamples[BUFFER_SIZE_IN_SAMPLES]);
};

// ��������� ����� � ������ ������ �� ����������� ������.
void Ogg::process( f32 * left, f32 * right )
{   
    bool looping = this->looping();

    if (!looping && mIsFinished)
        return;

    const s16 * ptr = mSamples + mSamplesCurrent;

    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        // ���� ����� ������� � �����
        if (mSamplesCurrent >= mSampleCount / 2)
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
Ogg::~Ogg()
{
    delete [] mSamples;
	ov_clear(mVorbisFile);
}

// �������� ��������� ����� �� wav-�����.
reference<Ogg> Ogg::create( const char * path )
{
    return reference<Ogg>(new Ogg(path));
}
