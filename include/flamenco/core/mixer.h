/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Микшер.
 */
#pragma once
#ifndef _FLAMENCO_CORE_MIXER_H_
#define _FLAMENCO_CORE_MIXER_H_

namespace flamenco
{

// Микшер.
class Mixer : noncopyable
{
public:
    // Присоединение пина. assert(!pin.connected()).
    void attach( reference<Pin> pin );
    // Отсоединение пина. Если он не присоединен, warning в лог.
    void detach( reference<Pin> pin );
    
    // Заполняет буфер смикшированными данными. Формат буфера - PCM 44100 16bit interleaved stereo.
    // Размер буфера фиксирован - равен MIXER_BUFFER_SIZE_IN_SAMPLES семплам.
    //
    // Функция в цикле пробегает по подключенным пинам и вызывает их функцию process() от своих
    // внутренних буферов. Затем смешивает получившиеся данные в итоговый буфер.
    //
    // Пара слов о реализации: внутри микшера должно быть 2 буфера - по одному на канал для передачи
    // в Pin::process. В таком случае после получения данных пина они должны быть примешаны к итоговому
    // буферу. (Возможно, это будет медленно, тогда нужно будет еще 2 буфера для смешивания отдельно каналов,
    // а затем уже их копировать с чередованием в переданный буфер).
    // Для проверки выхода за границы массива хорошо делать внутренние буферы чуть больше, чем нужно
    // и забивать их известными значениями.
    void mix( u16 * buffer );
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_MIXER_H_
