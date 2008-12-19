/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ����� �� wav-�����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

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
    void process( f32 * left, f32 * right );
    
    // ���������� �������
    u32 mChannels;
    
    // ���� ���� �������
    u8 * mBuffer;
    
    // ��������� �� ������ �������
    s16 * mSamples;
    
    // ����� ���������� �������
    u32 mSamplesCount;
    
    // ������� �����
    u32 mCurrentSample;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
