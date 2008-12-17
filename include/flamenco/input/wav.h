#pragma once
#ifndef _FLAMENCO_INPUT_WAV_H_
#define _FLAMENCO_INPUT_WAV_H_

namespace flamenco
{

// �������� �������������� �����.
class Wav : public Pin
{
public:
    // �������� wav �����
    static reference<Wav> create( const char * path, bool looped = false );
    
    // ����������
    ~Wav();
    
private:
    // ������� �����������
    Wav( const char * path, bool looped );

    // ��������� ������ ������� ��������� �������.
    void process( s16 * left, s16 * right );

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

    // ������������� �����
    bool mIsLooped;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_WAV_H_
