/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ���������� ����������� ���������� �����.
 */
#include <flamenco/flamenco.h>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace flamenco;

namespace
{

// ������������� ������� ������� (sin(x) * 2^14).
const u32 SINE_TABLE_SIZE = 4096;
s16 gSineTable[SINE_TABLE_SIZE];

// ������������� �������.
bool initSineTable()
{
    for (u32 i = 0; i < SINE_TABLE_SIZE; ++i)
        gSineTable[i] = static_cast<s16>(sin(i * 2 * M_PI / SINE_TABLE_SIZE) * (1 << 14));
    return true;
}
bool gSineTableInitialized = initSineTable();

} // namespace


// �����������.
Sine::Sine( u32 frequency )
    : frequency(frequency), mTimeShift(0)
{
}

// ���������� ������ ������������� ������.
void Sine::process( s16 * left, s16 * right )
{
    u32 frequency = this->frequency.value();
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        u32 index = static_cast<u32>(double(frequency) * (mTimeShift + i) / FREQUENCY * SINE_TABLE_SIZE);
        *left++ = *right++ = gSineTable[index % SINE_TABLE_SIZE];
    }
    mTimeShift += CHANNEL_BUFFER_SIZE_IN_SAMPLES;
}

// �������� ������ ��������� �������������� ����� � �������� frequency ��.
reference<Sine> Sine::create( u32 frequency )
{
    return reference<Sine>(new Sine(frequency));
}

// �����������
Noise::Noise()
{
}

// ���������� ������ �����
void Noise::process( s16 * left, s16 * right )
{
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        *left++  = static_cast<s16>(rand());
        *right++ = static_cast<s16>(rand());
    }
}

// �������� ��������� ����
reference<Noise> Noise::create()
{
    return reference<Noise>(new Noise());
}

// 