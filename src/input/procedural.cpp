/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ����������� ���������� �����.
 */
#include <flamenco/flamenco.h>
#define _USE_MATH_DEFINES // ��������� ��������-����������� (M_PI)
#include <cmath>

using namespace flamenco;

// �����������.
Sine::Sine( u32 frequency )
    : frequency(frequency), mTimeShift(0)
{
}

// ���������� ������ ������������� ������.
void Sine::process( f32 * left, f32 * right )
{
    u32 frequency = this->frequency();
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        f32 value = sinf(2 * static_cast<f32>(M_PI) * frequency * (mTimeShift + i) / FREQUENCY);
        *left++ = *right++ = value;
    }
    mTimeShift = (mTimeShift + CHANNEL_BUFFER_SIZE_IN_SAMPLES) % FREQUENCY;
}

// �������� ������ ��������� �������������� ����� � �������� frequency ��.
reference<Sine> Sine::create( u32 frequency )
{
    return reference<Sine>(new Sine(frequency));
}


// ���������� ������� ���������� �������.
void Noise::process( f32 * left, f32 * right )
{
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        *left++  = static_cast<f32>(rand() - (RAND_MAX >> 1)) / (RAND_MAX >> 1);
        *right++ = static_cast<f32>(rand() - (RAND_MAX >> 1)) / (RAND_MAX >> 1);
    }
}

// �������� ��������� ����
reference<Noise> Noise::create()
{
    return reference<Noise>(new Noise());
}
