/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_MIXER_H_
#define _FLAMENCO_CORE_MIXER_H_

#include <vector>

namespace flamenco
{

// ������.
class Mixer : noncopyable
{
public:
    // ��������� ������ �� ������������ ����� �������.
    static Mixer & singleton();
    
    
    // ������������� ����. assert(!pin.connected()).
    void attach( reference<Pin> pin );
    // ������������ ����. ���� �� �� �����������, warning � ���.
    void detach( reference<Pin> pin );
    
    // ��������� ����� ��������������� �������. ������ ������ - PCM 44100 16bit interleaved stereo.
    // ������ ������ ���������� - ����� MIXER_BUFFER_SIZE_IN_SAMPLES �������.
    //
    // ������� � ����� ��������� �� ������������ ����� � �������� �� ������� process() �� �����
    // ���������� �������. ����� ��������� ������������ ������ � �������� �����.
    //
    // ���� ���� � ����������: ������ ������� ������ ���� 2 ������ - �� ������ �� ����� ��� ��������
    // � Pin::process. � ����� ������ ����� ��������� ������ ���� ��� ������ ���� ��������� � ���������
    // ������. (��������, ��� ����� ��������, ����� ����� ����� ��� 2 ������ ��� ���������� �������� �������,
    // � ����� ��� �� ���������� � ������������ � ���������� �����).
    // ��� �������� ������ �� ������� ������� ������ ������ ���������� ������ ���� ������, ��� �����
    // � �������� �� ���������� ����������.
    //
    // ����������: ���, �������������� ��������������� � �������, ��� ������ process ������� ������
    // �������, ����������� �������.
    void mix( s16 * buffer );
    
private:
    // ����������� ������.
    static critical_section mCriticalSection;
    // ������ ������������ �����.
    typedef std::vector<reference<Pin> > PinList;
    PinList mPins;
    // ������ ��� ������ � ������� �������.
    // �� 1 ������ ��� �������� ������ �� �������.
    f32 mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1],
        mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1];
    // ��������� �������� ��� �������� ������ �� ������� �������.
    static const f32 MAGIC;
    
    Mixer();
    ~Mixer();
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_MIXER_H_
