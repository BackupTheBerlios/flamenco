/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� �������.
 */
#include <flamenco/flamenco.h>

using namespace flamenco;

// ������� ������ � ������������ � ���� ������� ���.
Effect::Effect( reference<Pin> input )
{
    assert(!input->connected());
    mInput = input;
    mInput->setConnected(true);
}

// ����������.
Effect::~Effect()
{
    assert(mInput->connected());
    mInput->setConnected(false);
}
