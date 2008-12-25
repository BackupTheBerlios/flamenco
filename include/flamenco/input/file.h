/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Источник данных из обычного файла.
 */
#pragma once
#ifndef _FLAMENCO_INPUT_FILE_H_
#define _FLAMENCO_INPUT_FILE_H_

namespace flamenco
{

// Источник данных из обычного файла без буферизации чтения.
class file_source : public source
{
public:
    file_source( const char * path );
    ~file_source();
    
    // Читает count элементов размером size байт в destination.
    // Возвращает число полностью прочитанных элементов. В случае
    // ошибки бросает исключение, т.ч. завершение операции с результатом,
    // отличным от count, автоматически означает конец файла.
    u32 read( void * destination, u32 size, u32 count );
    
    // Перемещение курсора чтения. Аналог функции fseek().
    // Параметр origin может принимать значения SEEK_CUR, SEEK_END и SEEK_SET.
    void seek( s32 offset, s32 origin );
    
    // Получение текущего положения курсора чтения (смещение от начала файла).
    u32 tell() const;
    
private:
    struct FILE * mFile;
};

} // namespace flamenco

#endif // _FLAMENCO_INPUT_FILE_H_
