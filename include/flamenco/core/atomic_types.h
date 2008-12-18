/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������������� ����.
 */
#pragma once
#ifndef _FLAMENCO_CORE_ATOMICTYPES_H_
#define _FLAMENCO_CORE_ATOMICTYPES_H_

#include <intrin.h>

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

} // namespace flamenco

#endif // _FLAMENCO_CORE_ATOMICTYPES_H_
