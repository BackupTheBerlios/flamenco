/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Пин (элемент цепочки звуковых фильтров).
 */
#pragma once
#ifndef _FLAMENCO_CORE_PIN_H_
#define _FLAMENCO_CORE_PIN_H_

namespace flamenco
{

// Пин (элемент цепочки звуковых фильтров).
class Pin : public refcountable
{
public:
    Pin();
    virtual ~Pin() = 0 {}
    
protected:
    // Заполняет буферы каналов звуковыми данными.
    // Размер каждого буфера - CHANNEL_BUFFER_SIZE_IN_SAMPLES семплов.
    virtual void process( u16 * left, u16 * right ) = 0;
    friend class Mixer;
    
    // Присоединен ли пин к другому пину или микшеру.
    inline bool connected() const
    {
        return mIsConnected;
    }
    
private:
    bool mIsConnected;
    
    // Пометить пин как присоединенный или отоединенный.
    inline void setConnected( bool connected )
    {
        mIsConnected = connected;
    }
    friend class Effect;
    friend class Mixer;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_PIN_H_
