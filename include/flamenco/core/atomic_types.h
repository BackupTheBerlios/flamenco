/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������������� ����.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

// TODO: ���������� ��� ������ (��. atomic_f32). �������� - ������������ ��������������.

namespace flamenco
{

// ������������� ���������������� ���.
// ����������� ��������� �.�. ��� �� ������������ ��� ����������� ������ u32.
class atomic_u32 : noncopyable
{
public:
    atomic_u32( u32 value )
        : mValue(value) // ����� ����� �� ������������ � ������� �� ������� ������ :)
        {}
    
    // ���������� �������� ����������.
    inline u32 value() const
    {
        return _InterlockedCompareExchange(&mValue, 0, 0);
    }
    // ������������� ����� ��������, ���������� ������.
    inline u32 set( u32 value )
    {
        return _InterlockedExchange(&mValue, value);
    }

private:
    // Mutable, �.�. ��������� �������� ������� ����� ������������.
    mutable __declspec(align(32)) volatile s32 mValue;
};

// ������� ���������������� ���.
// ����������� ��������� �.�. ��� �� ������������ ��� ����������� ������ bool.
class atomic_bool : noncopyable
{
public:
    atomic_bool( bool value )
        : mValue(value) // ����� ����� �� ������������ � ������� �� ������� ������ :)
        {}
    
    // ���������� �������� ����������.
    inline bool value() const
    {
        return 0 != _InterlockedCompareExchange(&mValue, 0, 0);
    }
    // ������������� ����� ��������, ���������� ������.
    inline bool set( bool value )
    {
        return 0 != _InterlockedExchange(&mValue, value);
    }

private:
    // Mutable, �.�. ��������� �������� ������� ����� ������������.
    mutable __declspec(align(32)) volatile s32 mValue;
};

// ���������������� ��� � ��������� ������.
// ����������� ��������� �.�. ��� �� ������������ ��� ����������� ������ f32.
class atomic_f32 : noncopyable
{
public:
    atomic_f32( f32 value )
        // ����� ����� �� ������������ � ������� �� ������� ������ :)
        : mValue(*reinterpret_cast<u32 *>(&value))
        {}
    
    // ���������� �������� ����������.
    inline f32 value() const
    {
        u32 value = _InterlockedCompareExchange(&mValue, 0, 0);
        return *reinterpret_cast<f32 *>(&value);
    }
    // ������������� ����� ��������, ���������� ������.
    inline f32 set( f32 value )
    {
        u32 oldValue = _InterlockedExchange(&mValue, *reinterpret_cast<u32 *>(&value));
        return *reinterpret_cast<f32 *>(&oldValue);
    }
    
private:
    // Mutable, �.�. ��������� �������� ������� ����� ������������.
    mutable __declspec(align(32)) volatile s32 mValue;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_ATOMICTYPES_H_
