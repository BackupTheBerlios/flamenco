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
class sound_stream_base : public pin
{
public:
    // Флаг зацикленности звука.
    atomic<bool> looping;
    
    // Проигрывается ли звук (сбрасывается в false, когда незацикленный звук подошел к концу).
    atomic_readonly<bool, sound_stream_base> playing;
    
    // Количество циклов, которые успел проиграть звук. Если звук незациклен, счетчик равен 0.
    atomic_readonly<u32, sound_stream_base> loop_count;
    
    // Длина звукового потока в семплах. Оптимизирована для частых вызовов.
    virtual u32 length() const = 0;
    
    // Номер текущего проигрываемого семпла (с точностью до LATENCY).
    atomic_readonly<u32, sound_stream_base> position;
    
protected:
    sound_stream_base()
        : looping(false), playing(true), loop_count(0), position(0)
        {}
    
    // Установка флагов. Функции нужны, т.к. переменные имеют приватный set().
    inline bool set_playing( bool value )
    {
        return playing.set(value);
    }
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
     // Конструктор.
     decoder( std::auto_ptr<source> source );
     
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
class stream : public sound_stream_base
{
public:
    // Создание потока.
    static reference<stream> create( std::auto_ptr<source> source );
    
    
private:
    stream( std::auto_ptr<source> source )
        : mDecoder(source)
    {
        if (!frequency_is_supported(mDecoder.frequency()))
            throw std::runtime_error("Unsupported sample rate.");
    }
    
    // Заполняет буферы каналов звуковыми данными, полученными от декодера.
    // Если частота данных декодера не совпадает с частотой библиотеки,
    // выполняет преобразование.
    void process( f32 * left, f32 * right );
    
    // Длина звукового потока в семплах.
    inline u32 length() const
    {
        return mDecoder.length();
    }
    
    // Поддерживается ли частота файла библиотекой.
    bool frequency_is_supported( u32 freq )
    {
        return freq == 44100 || freq == 32000 || freq == 22050;
    }
    
    // Декодер.
    decoderT mDecoder;
};


// Реализация.

template <class decoderT>
reference<stream<decoderT> > stream<decoderT>::create( std::auto_ptr<source> source )
{
    return reference<stream<decoderT> >(new stream<decoderT>(source));
}

template <class decoderT>
void stream<decoderT>::process( f32 * left, f32 * right )
{
    bool looping = this->looping();
    if (!looping && !playing())
        return;
    set_playing(true);
    
    // Заполняем каналы данными. Если скопировано меньше, чем нужно, значит поток закончился.
    u32 count = mDecoder.unpack(left, right, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
    if (count < CHANNEL_BUFFER_SIZE_IN_SAMPLES)
    {
        if (looping)
        {
            // Заполняем буфер, читая файл снова и снова.
            u32 loopCount = 0, samplesRead = 0;
            while (count < CHANNEL_BUFFER_SIZE_IN_SAMPLES)
            {
                mDecoder.seek(0);
                samplesRead = mDecoder.unpack(left, right, CHANNEL_BUFFER_SIZE_IN_SAMPLES - count);
                count += samplesRead;
                loopCount++;
            }
            assert(count == CHANNEL_BUFFER_SIZE_IN_SAMPLES);
            set_loop_count(loop_count() + loopCount);
            set_position(samplesRead);
        }
        else // Файл доиграл до конца, выставляем флаги.
        {
            set_loop_count(0);
            mDecoder.seek(0);
            set_position(0);
            set_playing(false);
        }
    }
    else
    {
        assert(count == CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        set_position(position() + CHANNEL_BUFFER_SIZE_IN_SAMPLES);
    }
}

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
