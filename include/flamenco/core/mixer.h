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
class mixer : noncopyable
{
public:
    // ��������� ������ �� ������������ ����� �������.
    static mixer & singleton();
    
    
    // ������������� ����. assert(!pin.connected()).
    void attach( reference<pin> pin );
    // ������������ ����. ���� �� �� �����������, warning � ���.
    void detach( reference<pin> pin );
    
    // ��������� ����� ��������������� �������. ������ ������ - PCM 44100 16bit interleaved stereo.
    // ������ ������ ���������� - ����� MIXER_BUFFER_SIZE_IN_SAMPLES �������.
    //
    // ������� � ����� ��������� �� ������������ ����� � �������� �� ������� process() �� �����
    // ���������� �������. ����� ��������� ������������ ������ � �������� �����.
    //
    // ���� ���� � ����������: ������ ������� ������ ���� 2 ������ - �� ������ �� ����� ��� ��������
    // � pin::process. � ����� ������ ����� ��������� ������ ���� ��� ������ ���� ��������� � ���������
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
    typedef std::vector<reference<pin> > PinList;
    PinList mPins;
    // ������ ��� ������ � ������� �������.
    // �� 1 ������ ��� �������� ������ �� �������.
    f32 mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1],
        mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES + 1];
    // ��������� �������� ��� �������� ������ �� ������� �������.
    static const f32 MAGIC;
    
    mixer();
    ~mixer();
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_MIXER_H_
