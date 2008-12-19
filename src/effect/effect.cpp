/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Реализация эффекта.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Создает эффект и присоединяет к нему входной пин.
Effect::Effect( reference<Pin> input )
{
    assert(!input->connected());
    mInput = input;
    mInput->setConnected(true);
}

// Деструктор.
Effect::~Effect()
{
    assert(mInput->connected());
    mInput->setConnected(false);
}
