/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Реализация пина.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// Конструктор.
Pin::Pin()
    : mIsConnected(false)
{
}

// Деструктор.
Pin::~Pin()
{
    assert(!mIsConnected);
}
