/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Источник данных для декодера.
 */
#pragma once
#ifndef _FLAMENCO_CORE_SOURCE_H_
#define _FLAMENCO_CORE_SOURCE_H_

namespace flamenco
{

// Базовый класс для источников данных для декодера.
// В случае ошибок бросает исключения-наследники std::exception.
class source : noncopyable
{
public:
    virtual ~source() = 0 {}
    
    // Читает count элементов размером size байт в destination.
    // Возвращает число полностью прочитанных элементов. В случае
    // ошибки бросает исключение, т.ч. завершение операции с результатом,
    // отличным от count, автоматически означает конец файла.
    virtual u32 read( void * destination, u32 size, u32 count ) = 0;
    
    // Перемещение курсора чтения. Аналог функции fseek().
    // Параметр origin может принимать значения SEEK_CUR, SEEK_END и SEEK_SET.
    virtual void seek( s32 offset, s32 origin ) = 0;
    
    // Получение текущего положения курсора чтения (смещение от начала файла).
    virtual u32 tell() const = 0;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_SOURCE_H_
