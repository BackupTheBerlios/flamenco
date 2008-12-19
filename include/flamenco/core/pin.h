/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ��� (������� ������� �������� ��������).
 */
#pragma once
#ifndef _FLAMENCO_CORE_PIN_H_
#define _FLAMENCO_CORE_PIN_H_

namespace flamenco
{

// ��� (������� ������� �������� ��������).
class Pin : public refcountable
{
protected:
    Pin();
    virtual ~Pin() = 0;
    friend class reference<Pin>;
    
    // ��������� ������ ������� ��������� �������.
    // ������ ������� ������ - CHANNEL_BUFFER_SIZE_IN_SAMPLES �������.
    virtual void process( f32 * left, f32 * right ) = 0;
    friend class Mixer;
    
    // ����������� �� ��� � ������� ���� ��� �������.
    inline bool connected() const
    {
        return mIsConnected;
    }
    
private:
    bool mIsConnected;
    
    // �������� ��� ��� �������������� ��� ������������.
    inline void setConnected( bool connected )
    {
        mIsConnected = connected;
    }
    friend class Effect;
    friend class Mixer;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_PIN_H_
