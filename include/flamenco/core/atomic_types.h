/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ѕотокобезопасные типы.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

// TODO: переписать под шаблон (см. atomic_f32). ѕроблема - безопасность преобразований.

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

// Ѕулевый потокобезопасный тип.
//  опирование запрещено т.к. тип не предназначен дл€ полноценной замены bool.
class atomic_bool : noncopyable
{
public:
    atomic_bool( bool value )
        : mValue(value) // «десь можно не беспокоитьс€ о доступе из другого потока :)
        {}
    
    // ¬озвращает значение переменной.
    inline bool value() const
    {
        return 0 != _InterlockedCompareExchange(&mValue, 0, 0);
    }
    // ”станавливает новое значение, возвращает старое.
    inline bool set( bool value )
    {
        return 0 != _InterlockedExchange(&mValue, value);
    }

private:
    // Mutable, т.к. получение значени€ сделано через присваивание.
    mutable __declspec(align(32)) volatile s32 mValue;
};

// ѕотокобезопасный тип с плавающей точкой.
//  опирование запрещено т.к. тип не предназначен дл€ полноценной замены f32.
class atomic_f32 : noncopyable
{
public:
    atomic_f32( f32 value )
        // «десь можно не беспокоитьс€ о доступе из другого потока :)
        : mValue(*reinterpret_cast<u32 *>(&value))
        {}
    
    // ¬озвращает значение переменной.
    inline f32 value() const
    {
        u32 value = _InterlockedCompareExchange(&mValue, 0, 0);
        return *reinterpret_cast<f32 *>(&value);
    }
    // ”станавливает новое значение, возвращает старое.
    inline f32 set( f32 value )
    {
        u32 oldValue = _InterlockedExchange(&mValue, *reinterpret_cast<u32 *>(&value));
        return *reinterpret_cast<f32 *>(&oldValue);
    }
    
private:
    // Mutable, т.к. получение значени€ сделано через присваивание.
    mutable __declspec(align(32)) volatile s32 mValue;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_ATOMICTYPES_H_
