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
class WavPack : public Pin
{
public:
    ~WavPack();

    // �������� ��������� �����.
    static reference<WavPack> create( const char * path );

    // ���� ������������� �����.
    atomic<bool> looping;


private:
    WavPack( const char * path );

    // �������� ������ �� ������ � ����� � ������ ������
    void process( f32 * left, f32 * right );
    // ������ ������ �� ����� �� ���������� �����
    void fill(bool looping);

    // ���������� �������
    u32 mChannels;
    // �������
    u32 mFrequency;

    // ����� ��� ������� �������� smBufferSizeInSamples
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
    static const s32 smMagic;

    // ������������ ������ ������ � �������
    static const u32 smBufferSizeInSamples;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAVPACK_H_
