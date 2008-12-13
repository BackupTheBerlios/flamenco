/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Ёффект (пин, имеющий вход и выход).
 */
#pragma once
#ifndef _FLAMENCO_CORE_EFFECT_H_
#define _FLAMENCO_CORE_EFFECT_H_

namespace flamenco
{

// Ёффект - пин, обрабатывающий данные другого пина.
class Effect : public Pin
{
public:
    // —оздает эффект и присоедин€ет к нему входной пин.
    Effect( reference<Pin> input );
    
private:
    // ќбычно вызывает mInputPin.process() и обрабатывает
    // заполненные им буферы. Ќо может и заполн€ть их самосто€тельно.
    virtual void process( u16 * left, u16 * right ) = 0;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_EFFECT_H_
