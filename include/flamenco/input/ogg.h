/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 �������� ����� �� ogg-vorbis-�����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_OGG_H_
#define _FLAMENCO_INPUT_OGG_H_

#include <stdio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


namespace flamenco
{

// �������� ����� �� ogg-vorbis-�����.
class Ogg : public Pin
{
public:
    ~Ogg();
    
    // �������� ��������� �����.
    static reference<Ogg> create( const char * path );
    
    // ���� ������������� �����.
    atomic<bool> looping;
    
    
private:
    Ogg( const char * path );
    
    // �������� ������ �� ������ � ����� � ������ ������.
    void process( f32 * left, f32 * right );
    // ������ ������ �� ����� �� ���������� �����.
    void fill(bool looping);
    
    // ���������� �������.
    u32 mChannels;
    // �������.
    u32 mFrequency;
    
    // ����� ��� ������� �������� BUFFER_SIZE_IN_SAMPLES.
    s16 * mSamples;
    // ������� ����� �� ������ ������.
    u32 mSamplesCurrent;
    // �������� ���������� ������� � ������.
    u32 mSampleCount;

	// ���� ��� ������
	FILE * mInput;
    
	// ������� ���� vorbis
	OggVorbis_File  *mVorbisFile;
	// ���������� � �����
	vorbis_info    *mVorbisInfo;
	// ������� ��������� �����.
	bool mIsFinished;

   
    // ���������� ����� ��� �������� �������.
    static const s16 MAGIC;
    
    // ������������ ������ ������ � �������.
    static const u32 BUFFER_SIZE_IN_SAMPLES;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_OGG_H_
