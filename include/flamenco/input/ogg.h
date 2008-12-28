/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ������� ogg vorbis ������
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace flamenco
{

// ������� ogg vorbis ������
class ogg_decoder : noncopyable
{
public:
    ogg_decoder( std::auto_ptr<source> source );
    ~ogg_decoder();
    
    // �������� � ����� � ������ ������ count �������������� �������.
    // ���������� ���������� ������������� �������, ��� ����� ���� ������ count,
    // ���� ����� ����������.
    u32 unpack( f32 * left, f32 * right, u32 count );
    
    // ��������� ������� ������ ������������� �� �������� �����.
    void seek( u32 sample );
    
    // ����� ������ � �������. ������� �������������� ��� ������ �������.
    inline u32 length() const
    {
        return mSampleCount;
    }
    
    // ������� ��������� ������ (��� �������������� ������� �������).
    inline u32 frequency() const
    {
        return mSampleRate;
    }
    
private:
	// ������������� �� vorbis ������ mBufferSize ������� �� ���������� ������.
	// ���������� ���������� ����������� �������.
	u32 unpack_vorbis();

    // �������� ������.
    std::auto_ptr<source> mSource;

    // ������� �������������.
    u32 mSampleRate;
    // ����� ����� � �������.
    u32 mSampleCount;
    // ���������� ������� (1 ��� 2).
    u32 mChannelCount;

    // ������ ��� ������ � ������� �������.
    f32 * mBufferL;
    f32 * mBufferR;
    // ������ ������ ������ � �������
    u32 mBufferSize;

	// ������� ���������� ������� � ����� ������
	u32 mBufferRealSize;
	// ������� ����� � ������ (����� ��� ����, ����� ��� ������)
	u32 mBufferOffset;

    // ������� ���������� �����
    OggVorbis_File  *mVorbisFile;
    // ���������� � ������
    vorbis_info    *mVorbisInfo;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
