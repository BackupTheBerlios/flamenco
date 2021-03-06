/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ������� ��� ��������� ��������� � ���������������.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

namespace
{

// ���������� f32 � ��������� �� 0 �� 1.
inline f32 clamp_0_1( f32 value )
{
    return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

}

// �������� ������ �������.
reference<volume_pan> volume_pan::create( reference<pin> input, f32 volume, f32 pan )
{
    return reference<volume_pan>(new volume_pan(input, volume, pan));
}

// �����������.
volume_pan::volume_pan( reference<pin> input, f32 volume, f32 pan )
    : effect(input), volume(volume), pan(pan)
{
    assert(0.0f <= volume && volume <= 1.0f);
    assert(-1.0f <= pan && pan <= 1.0f);
}

// ��������� ������ ������� ��������� �������.
void volume_pan::process( f32 * left, f32 * right )
{
    f32 volume = clamp_0_1(this->volume()), pan = this->pan();
    f32 volumeLeft  = volume * clamp_0_1(1.0f - pan),
        volumeRight = volume * clamp_0_1(1.0f + pan);
    
    process_input_pin(left, right);
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        *left++  *= volumeLeft;
        *right++ *= volumeRight;
    }
}
