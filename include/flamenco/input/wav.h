/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ������� wav-������ ��� ������.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

#include <stdio.h>

namespace flamenco
{

// ������� wav-������ ��� ������.
class wav_decoder : noncopyable
{
public:
    wav_decoder( std::auto_ptr<source> source );
    
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
    // �������� ������.
    std::auto_ptr<source> mSource;
    
    // �������� �� ������ ����� �� ������ wav-������.
    u32 mDataOffset;
    // ������� �������������.
    u32 mSampleRate;
    // ����� ����� � �������.
    u32 mSampleCount;
    // ���������� ������� (1 ��� 2).
    u32 mChannelCount;
    // ����� ��� �������������� ������� �� interleaved s16 � separate f32.
    auto_array<s16> mChannelBuffer;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
