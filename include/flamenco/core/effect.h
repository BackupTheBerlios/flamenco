/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
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
protected:
    // ������� ������ � ������������ � ���� ������� ���.
    Effect( reference<Pin> input );
    
    // ������ �������� mInputPin.process() � ������������
    // ����������� �� ������. �� ����� � ��������� �� ��������������.
    virtual void process( f32 * left, f32 * right ) = 0;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_EFFECT_H_
