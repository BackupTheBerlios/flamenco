/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация источника данных из файла.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Открытие файла.
file_source::file_source( const char * path )
    : mFile(NULL)
{
    if (NULL == (mFile = fopen(path, "rb")))
        throw std::runtime_error("Unable to open file.");
}

// Деструктор.
file_source::~file_source()
{
    if (NULL != mFile)
        fclose(mFile);
}

// Читает count элементов размером size байт в destination.
u32 file_source::read( void * destination, u32 size, u32 count )
{
    assert(NULL != mFile);
    assert(NULL != destination);
    return fread(destination, size, count, mFile);
}

// Перемещение курсора чтения. Аналог функции fseek().
void file_source::seek( s32 offset, s32 origin )
{
    assert(NULL != mFile);
    fseek(mFile, offset, origin);
}

// Получение текущего положения курсора чтения (смещение от начала файла).
u32 file_source::tell() const
{
    assert(NULL != mFile);
    return ftell(mFile);
}
