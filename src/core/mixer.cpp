/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация микшера.
 */
#include <flamenco/flamenco.h>
#include <algorithm>
#include <iostream>

using namespace flamenco;

namespace
{

// Версия функции set_silence для буфера 16 bit.
inline void set_silence( s16 * buffer, u32 sizeInSamples )
{
    memset(buffer, 0, sizeInSamples << 1);
}

// Преобразование s32 в s16 с насыщением.
inline s16 clamp_sample( s32 value )
{
    return static_cast<s16>(value < -32768 ? -32768 : value > 32767 ? 32767 : value);
}

} // namespace

// Критическая секция для доступа к микшеру.
critical_section mixer::mCriticalSection;

// Волшебное значение для проверки выхода за границы буферов.
const f32 mixer::MAGIC = 5.9742e24f; // Масса Земли :)


// Получение ссылки на единственную копию микшера.
mixer & mixer::singleton()
{
    static flamenco::mixer * mixer = NULL;
    if (NULL == mixer)
    {
        critical_section::lock lock(mCriticalSection);
        if (NULL == mixer)
            mixer = new flamenco::mixer();
    }
    return *mixer;
}

// Конструктор.
mixer::mixer()
{
    // В конец буферов каналов запишем магическое значение,
    // чтобы отловить выход за границы памяти.
    mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
}

// Присоединение пина.
void mixer::attach( reference<pin> pin )
{
    assert(!pin->connected());
    mPins.push_back(pin);
    pin->setConnected(true);
}

// Отсоединение пина. Если он не присоединен, warning в лог.
void mixer::detach( reference<pin> pin )
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
        std::clog << "mixer::detach(): pin is not attached";
}

// Микширование.
void mixer::mix( s16 * buffer )
{
    // Заполняем выходной буфер тишиной.
    ::set_silence(buffer, MIXER_BUFFER_SIZE_IN_SAMPLES);
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
        s16 *left = buffer, *right = buffer + 1;
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i, left += 2, right += 2)
        {
            *left  = clamp_sample(*left  + static_cast<s32>(mBufferL[i] * (1 << 15)));
            *right = clamp_sample(*right + static_cast<s32>(mBufferR[i] * (1 << 15)));
        }
    }
}

// Деструктор. Нужен для отсоединения пинов.
mixer::~mixer()
{
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        (*i)->setConnected(false);
    }
}
