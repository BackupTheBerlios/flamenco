/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ������ ��� ��������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_SOURCE_H_
#define _FLAMENCO_CORE_SOURCE_H_

namespace flamenco
{

// ������� ����� ��� ���������� ������ ��� ��������.
// � ������ ������ ������� ����������-���������� std::exception.
class source : noncopyable
{
public:
    virtual ~source() = 0 {}
    
    // ������ count ��������� �������� size ���� � destination.
    // ���������� ����� ��������� ����������� ���������. � ������
    // ������ ������� ����������, �.�. ���������� �������� � �����������,
    // �������� �� count, ������������� �������� ����� �����.
    virtual u32 read( void * destination, u32 size, u32 count ) = 0;
    
    // ����������� ������� ������. ������ ������� fseek().
    // �������� origin ����� ��������� �������� SEEK_CUR, SEEK_END � SEEK_SET.
    virtual void seek( s32 offset, s32 origin ) = 0;
    
    // ��������� �������� ��������� ������� ������ (�������� �� ������ �����).
    virtual u32 tell() const = 0;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_SOURCE_H_
