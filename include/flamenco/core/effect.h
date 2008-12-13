/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ������ (���, ������� ���� � �����).
 */
#pragma once
#ifndef _FLAMENCO_CORE_EFFECT_H_
#define _FLAMENCO_CORE_EFFECT_H_

namespace flamenco
{

// ������ - ���, �������������� ������ ������� ����.
class Effect : public Pin
{
public:
    // ������� ������ � ������������ � ���� ������� ���.
    Effect( reference<Pin> input );
    
private:
    // ������ �������� mInputPin.process() � ������������
    // ����������� �� ������. �� ����� � ��������� �� ��������������.
    virtual void process( u16 * left, u16 * right ) = 0;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_EFFECT_H_
