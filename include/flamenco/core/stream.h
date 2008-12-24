/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Источник звуковых данных из файла или иного источника данных.
 */
#pragma once
#ifndef _FLAMENCO_CORE_STREAM_H_
#define _FLAMENCO_CORE_STREAM_H_

namespace flamenco
{

// Базовый класс для звуковых потоков.
// Нужен для того, чтобы к потокам разных типов можно было обращаться
// по общему интерфейсу.
class sound_stream_base : public Pin
{
public:
    // Флаг зацикленности звука.
    atomic<bool> looping;
    
    // Количество циклов, которые успел проиграть звук.
    atomic_readonly<u32, sound_stream_base> loop_count;
    
    // Длина звукового потока в семплах. Оптимизирована для частых вызовов.
    virtual u32 length() const = 0;
    
    // Номер текущего проигрываемого семпла (с точностью до LATENCY).
    atomic_readonly<u32, sound_stream_base> position;
    
protected:
    sound_stream_base()
        : looping(false), loop_count(0), position(0)
        {}
    
    // Установка флагов. Функции нужны, т.к. loop_count и position имеют приватный set().
    inline u32 set_loop_count( u32 loopCount )
    {
        return loop_count.set(loopCount);
    }
    inline u32 set_position( u32 pos )
    {
        return position.set(pos);
    }
};

/*
 Все декодеры должны следовать одному интерфейсу:
 struct decoder
 {
     struct initializer;
     
     // Конструктор.
     decoder( const initializer & );
     
     // Копирует в левый и правый каналы count декодированных семплов.
     // Возвращает количество скопированных семплов, оно может быть меньше count,
     // если поток закончился.
     u32 unpack( f32 * left, f32 * right, u32 count );
     
     // Установка курсора начала декодирования на заданный семпл.
     void seek( u32 sample );
     
     // Длина потока в семплах. Функция оптимизирована для частых вызовов.
     u32 length() const;
     
     // Частота звукового потока (для преобразования частоты потоком).
     u32 frequency() const;
 }
 */

// Источник звуковых данных из файлов различных форматов.
template <class decoderT>
class stream : public Pin
{
public:
    // Параметры декодера.
    typedef typename decoderT::initializer initializer;
    
    // Создание потока.
    static reference<stream> create( const initializer & initializer );
    
    
private:
    stream( const initializer & initializer )
        : mDecoder(initializer)
        {}
    
    // Заполняет буферы каналов звуковыми данными, полученными от декодера.
    // Если частота данных декодера не совпадает с частотой библиотеки,
    // выполняет преобразование.
    void process( f32 * left, f32 * right );
    
    // Длина звукового потока в семплах.
    inline u32 length() const
    {
        return mDecoder.length();
    }
    
    // Декодер.
    decoderT mDecoder;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
