/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Процедурные источники звука.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_PROCEDURAL_H_
#define _FLAMENCO_INPUT_PROCEDURAL_H_

namespace flamenco
{

// Источник гармонического звука.
class Sine : public Pin
{
public:
    // Создание нового источника гармонического звука с частотой frequency Гц.
    static reference<Sine> create( u32 frequency );
    
    
    // Частота синусоиды.
    atomic_u32 frequency;
    
private:
    Sine( u32 frequency );
    
    // Заполняет буферы каналов звуковыми данными.
    void process( s16 * left, s16 * right );
    
    // Временной сдвиг синусоиды.
    u32 mTimeShift;
    
    // frequency сделана публичной переменной без get/set, т.к. atomic_u32 уже
    // содержит свои get и set, код был бы похож на лапшу.
};

// Источник шума
class Noise : public Pin
{
public:
    // Создание источника шума
    static reference<Noise> create();

private:
    Noise();

    // Заполняет буферы каналов шумом
    void process( s16 * left, s16 * right );
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_PROCEDURAL_H_
