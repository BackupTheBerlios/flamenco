/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 ���������� �������.
 */
#include <flamenco/flamenco.h>
#include <algorithm>
#include <iostream>

using namespace flamenco;

// ����������� ������ ��� ������� � �������.
critical_section Mixer::mCriticalSection;

// ��������� ������ �� ������������ ����� �������.
Mixer & Mixer::singleton()
{
    static Mixer * mixer = NULL;
    if (NULL == mixer)
    {
        critical_section::lock lock(mCriticalSection);
        if (NULL == mixer)
            mixer = new Mixer();
    }
    return *mixer;
}

// �����������.
Mixer::Mixer()
{
    // � ����� ������� ������� ������� ���������� ��������,
    // ����� �������� ����� �� ������� ������.
    mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
    mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES] = MAGIC;
}

// ������������� ����.
void Mixer::attach( reference<Pin> pin )
{
    assert(!pin->connected());
    mPins.push_back(pin);
    pin->setConnected(true);
}

// ������������ ����. ���� �� �� �����������, warning � ���.
void Mixer::detach( reference<Pin> pin )
{
    PinList::iterator i = std::find(mPins.begin(), mPins.end(), pin);
    if (mPins.end() != i)
    {
        assert((*i)->connected());
        (*i)->setConnected(false);
        mPins.erase(i);
    }
    else
        // TODO: ����� � user-supplied log.
        std::clog << "Mixer::detach(): pin is not attached";
}

// ������������.
void Mixer::mix( s16 * buffer )
{
    // ��������� �������� ����� �������.
    set_silence(buffer, MIXER_BUFFER_SIZE_IN_SAMPLES);
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        // ��������� ������� ������ ������ � ������� �������.
        set_silence(mBufferL, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        set_silence(mBufferR, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        
        // ��������� ������.
        (*i)->process(mBufferL, mBufferR);
        // ��������� ����� �� �������.
        assert(MAGIC == mBufferL[CHANNEL_BUFFER_SIZE_IN_SAMPLES] &&
               MAGIC == mBufferR[CHANNEL_BUFFER_SIZE_IN_SAMPLES]);
        
        // ����������� ������ � �������� �����.
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i)
        {
            buffer[i << 1]       += mBufferL[i];
            buffer[(i << 1) + 1] += mBufferR[i];
        }
    }
}

// ����������. ����� ��� ������������ �����.
Mixer::~Mixer()
{
    for (PinList::iterator i = mPins.begin(); mPins.end() != i; ++i)
    {
        assert((*i)->connected());
        (*i)->setConnected(false);
    }
}
