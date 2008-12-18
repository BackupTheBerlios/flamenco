/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ѕотокобезопасные типы.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

namespace flamenco
{

// ÷елочисленный потокобезопасный тип.
//  опирование запрещено т.к. тип не предназначен дл€ полноценной замены u32.
class atomic_u32 : noncopyable
{
public:
    atomic_u32( u32 value )
        : mValue(value) // «десь можно не беспокоитьс€ о доступе из другого потока :)
        {}
    
    // ¬озвращает значение переменной.
    inline u32 value() const
    {
        return _InterlockedCompareExchange(&mValue, 0, 0);
    }
    // ”станавливает новое значение, возвращает старое.
    inline u32 set( u32 value )
    {
        return _InterlockedExchange(&mValue, value);
    }

private:
    // Mutable, т.к. получение значени€ сделано через присваивание.
    mutable __declspec(align(32)) volatile s32 mValue;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_ATOMICTYPES_H_
