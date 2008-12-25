/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� ������ �� �������� �����.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_FILE_H_
#define _FLAMENCO_INPUT_FILE_H_

namespace flamenco
{

// �������� ������ �� �������� ����� ��� ����������� ������.
class file_source : public source
{
public:
    file_source( const char * path );
    ~file_source();
    
    // ������ count ��������� �������� size ���� � destination.
    // ���������� ����� ��������� ����������� ���������. � ������
    // ������ ������� ����������, �.�. ���������� �������� � �����������,
    // �������� �� count, ������������� �������� ����� �����.
    u32 read( void * destination, u32 size, u32 count );
    
    // ����������� ������� ������. ������ ������� fseek().
    // �������� origin ����� ��������� �������� SEEK_CUR, SEEK_END � SEEK_SET.
    void seek( s32 offset, s32 origin );
    
    // ��������� �������� ��������� ������� ������ (�������� �� ������ �����).
    u32 tell() const;
    
private:
    struct FILE * mFile;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_FILE_H_
