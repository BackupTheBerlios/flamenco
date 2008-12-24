/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Пин (элемент цепочки звуковых фильтров).
 */
#pragma once
#ifndef _FLAMENCO_CORE_PIN_H_
#define _FLAMENCO_CORE_PIN_H_

namespace flamenco
{

// Пин (элемент цепочки звуковых фильтров).
class pin : public refcountable
{
protected:
    pin();
    virtual ~pin() = 0;
    friend class reference<pin>;
    
    // Заполняет буферы каналов звуковыми данными.
    // Размер каждого буфера - CHANNEL_BUFFER_SIZE_IN_SAMPLES семплов.
    virtual void process( f32 * left, f32 * right ) = 0;
    
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
    friend class effect;
    friend class mixer;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_PIN_H_
