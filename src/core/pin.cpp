/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� ����.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// �����������.
Pin::Pin()
    : mIsConnected(false)
{
}

// ����������.
Pin::~Pin()
{
    assert(!mIsConnected);
}
