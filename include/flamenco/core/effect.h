/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Эффект (пин, имеющий вход и выход).
 */
#pragma once
#ifndef _FLAMENCO_CORE_EFFECT_H_
#define _FLAMENCO_CORE_EFFECT_H_

namespace flamenco
{

// Эффект - пин, обрабатывающий данные другого пина.
class Effect : public Pin
{
protected:
    // Создает эффект и присоединяет к нему входной пин.
    Effect( reference<Pin> input );
    ~Effect();
    
    // Обычно вызывает mInputPin.process() и обрабатывает
    // заполненные им буферы. Но может и заполнять их самостоятельно.
    virtual void process( f32 * left, f32 * right ) = 0;
    
    // Вызов process() для входного пина.
    inline void process_input_pin( f32 * left, f32 * right )
    {
        mInput->process(left, right);
    }
    
private:
    // Входной пин.
    reference<Pin> mInput;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_EFFECT_H_
