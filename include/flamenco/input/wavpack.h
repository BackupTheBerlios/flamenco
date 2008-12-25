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
class wavpack_decoder : noncopyable
{
public:
	wavpack_decoder( std::auto_ptr<source> source );
    ~wavpack_decoder();

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
	// ������������� �� wavpack ������ count ������� �� ���������� �����.
	// ���������� ���������� ����������� �������
	u32 unpack_wavpack(s32 * dst, u32 size);

	// �������� ������.
	std::auto_ptr<source> mSource;

	// ������� �������������.
	u32 mSampleRate;
	// ����� ����� � �������.
	u32 mSampleCount;
	// ���������� ������� (1 ��� 2).
	u32 mChannelCount;

	// ����� ��� �������������� ������� �� interleaved s16 � separate f32.
	s32 * mBuffer;
	// ������ ������ � �������
	u32 mBufferSize;

	// ������� ���������� ������� � ������
	u32 mBufferRealSize;
	// ������� ����� � ������
	u32 mBufferOffset;

	// ������� ���������� �����
	WavpackContext  *mWavpackFile;
};


} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAVPACK_H_
