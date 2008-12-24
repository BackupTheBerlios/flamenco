/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 �������� ����� �� wv-�����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAVPACK_H_
#define _FLAMENCO_INPUT_WAVPACK_H_

#include <wavpack/wavpack.h>

namespace flamenco
{


// �������� ����� �� wv-�����.
class wavpack : public pin
{
public:
    ~wavpack();

    // �������� ��������� �����.
    static reference<wavpack> create( const char * path );

    // ���� ������������� �����.
    atomic<bool> looping;


private:
    wavpack( const char * path );

    // �������� ������ �� ������ � ����� � ������ ������
    void process( f32 * left, f32 * right );
    // ������ ������ �� ����� �� ���������� �����
    void fill(bool looping);
    // ���������� ����� ������ ������ � �����
    u32 unpack(s32 * dst, u32 offset, u32 size);

    // ���������� �������
    u32 mChannels;
    // �������
    u32 mFrequency;

    // ����� ��� ������� �������� BUFFER_SIZE_IN_SAMPLES
    s32 * mSamples;
    // ������� ����� �� ������ ������
    u32 mSamplesCurrent;
    // �������� ���������� ������� � ������
    u32 mSamplesCount;

    // ���� ��� ������
    WavpackContext * mInput;
    // ������� ��������� �����
    bool mIsFinished;

    // ���������� ����� ��� �������� �������
    static const s32 MAGIC;

    // ������������ ������ ������ � �������
    static const u32 BUFFER_SIZE_IN_SAMPLES;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAVPACK_H_
