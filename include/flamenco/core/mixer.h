/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_MIXER_H_
#define _FLAMENCO_CORE_MIXER_H_

namespace flamenco
{

// ������.
class Mixer : noncopyable
{
public:
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
    void mix( u16 * buffer );
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_MIXER_H_
