/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Реализация процедурных источников звука.
 */
#include <flamenco/flamenco.h>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace flamenco;

namespace
{

// Целочисленная таблица синусов (sin(x) * 2^14).
const u32 SINE_TABLE_SIZE = 4096;
s16 gSineTable[SINE_TABLE_SIZE];

// Инициализация таблицы.
bool initSineTable()
{
    for (u32 i = 0; i < SINE_TABLE_SIZE; ++i)
        gSineTable[i] = static_cast<s16>(sin(i * 2 * M_PI / SINE_TABLE_SIZE) * (1 << 14));
    return true;
}
bool gSineTableInitialized = initSineTable();

} // namespace


// Конструктор.
Sine::Sine( u32 frequency )
    : frequency(frequency), mTimeShift(0)
{
}

// Заполнение буфера гармоническим звуком.
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

// Создание нового источника гармонического звука с частотой frequency Гц.
reference<Sine> Sine::create( u32 frequency )
{
    return reference<Sine>(new Sine(frequency));
}

// Конструктор
Noise::Noise()
{
}

// Заполнение буфера шумом
void Noise::process( s16 * left, s16 * right )
{
    for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
    {
        *left++  = static_cast<s16>(rand());
        *right++ = static_cast<s16>(rand());
    }
}

// Создание источника шума
reference<Noise> Noise::create()
{
    return reference<Noise>(new Noise());
}

// 