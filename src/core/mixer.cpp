/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Реализация микшера.
 */
#include <flamenco/flamenco.h>
#include <algorithm>
#include <iostream>

using namespace flamenco;

// Критическая секция для доступа к микшеру.
critical_section Mixer::mCriticalSection;

// Получение ссылки на единственную копию микшера.
Mixer & Mixer::singleton()
{
    static Mixer * mixer = NULL;
    if (NULL == mixer)
    {
        critical_section::lock lock(mCriticalSection);
        if (NULL == mixer)
            mixer = new Mixer();
    }
    return *mixer;
}

// Конструктор.
Mixer::Mixer()
{
    // В конец буферов каналов запишем магическое значение,
    // чтобы отловить выход за границы памяти.
    mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
}

// Присоединение пина.
void Mixer::attach( reference<Pin> pin )
{
    assert(!pin->connected());
    mPins.push_back(pin);
    pin->setConnected(true);
}

// Отсоединение пина. Если он не присоединен, warning в лог.
void Mixer::detach( reference<Pin> pin )
{
    PinList::iterator i = std::find(mPins.begin(), mPins.end(), pin);
    if (mPins.end() != i)
    {
        assert((*i)->connected());
        (*i)->setConnected(false);
        mPins.erase(i);
    }
    else
        // TODO: вывод в user-supplied log.
        std::clog << "Mixer::detach(): pin is not attached";
}

// Микширование.
void Mixer::mix( s16 * buffer )
{
    // Заполняем выходной буфер тишиной.
    set_silence(buffer, MIXER_BUFFER_SIZE_IN_SAMPLES);
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        // Заполняем тишиной буферы левого и правого каналов.
        set_silence(mBufferL, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        set_silence(mBufferR, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        
        // Заполняем каналы.
        (*i)->process(mBufferL, mBufferR);
        // Проверяем выход за границы.
        assert(MAGIC == mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] &&
               MAGIC == mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES]);
        
        // Примешиваем данные в итоговый буфер.
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
        {
            buffer[i << 1]       += mBufferL[i];
            buffer[(i << 1) + 1] += mBufferR[i];
        }
    }
}

// Деструктор. Нужен для отсоединения пинов.
Mixer::~Mixer()
{
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        (*i)->setConnected(false);
    }
}
