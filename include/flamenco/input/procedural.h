/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ����������� ��������� �����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_PROCEDURAL_H_
#define _FLAMENCO_INPUT_PROCEDURAL_H_

namespace flamenco
{

// �������� �������������� �����.
class Sine : public Pin
{
public:
    // �������� ������ ��������� �������������� ����� � �������� frequency ��.
    static reference<Sine> create( u32 frequency );
    
    
    // ������� ���������.
    atomic_u32 frequency;
    
private:
    Sine( u32 frequency );
    
    // ��������� ������ ������� ��������� �������.
    void process( s16 * left, s16 * right );
    
    // ��������� ����� ���������.
    u32 mTimeShift;
    
    // frequency ������� ��������� ���������� ��� get/set, �.�. atomic_u32 ���
    // �������� ���� get � set, ��� ��� �� ����� �� �����.
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_PROCEDURAL_H_
