/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ������ ��� ��������� ��������� � ���������������.
 */
#pragma once
#ifndef _FLAMENCO_EFFECT_VOLUMEPAN_H_
#define _FLAMENCO_EFFECT_VOLUMEPAN_H_

namespace flamenco
{

// ������ ��� ��������� ��������� � ���������������.
class VolumePan : public Effect
{
public:
    // �������� ������ �������. ��������� �������� ������ �� 0 �� 1,
    // �������� - �� -1 �� 1.
    static reference<VolumePan> create( reference<Pin> input, f32 volume, f32 pan );
    
    
    // ��������� � ��������� �� 0 �� 1.
    atomic<f32> volume;
    // ������ (��������).
    atomic<f32> pan;
    
    
    // ����������: ���� ���� �������� ��������� ��� ������� ������ ��
    // ������������ ���������, ������ ������� ����� ��������������
    // ����������� � ���� ��������.
    
    
private:
    VolumePan( reference<Pin> input, f32 volume, f32 pan );
    
    // ��������� ������ ������� ��������� �������.
    void process( f32 * left, f32 * right );
    
    // volume � pan ������� ��������� ���������� ��� get/set, �.�. atomic_f32 ���
    // �������� ���� get � set, ��� ��� �� ����� �� �����.
};

} // namespace flamenco

#endif // _FLAMENCO_EFFECT_VOLUMEPAN_H_
