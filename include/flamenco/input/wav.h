/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 �������� ����� �� wav-�����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

#include <stdio.h>

namespace flamenco
{


// �������� ����� �� wav-�����.
class Wav : public Pin
{
public:
    ~Wav();

    // �������� ��������� �����.
    static reference<Wav> create( const char * path );

    // ���� ������������� �����.
    atomic_bool looping;


private:
    Wav( const char * path );

	// �������� ������ �� ������ � ����� � ������ ������
    void process( f32 * left, f32 * right );
	// ������ ������ �� ����� �� ���������� �����
	void fill(bool looping);

    // ���������� �������
    u32 mChannels;
    // �������
    u32 mFrequency;

    // ����� ��� ������� �������� CHANNEL_BUFFER_SIZE_IN_SAMPLES * mChannels
    s16 * mSamples;

	// MAGIC
	static const s16 MAGIC;

    // ���� ��� ������
    FILE * mInput;
    // ������ ����� ������ � �����
    u32 mInputOffset;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
