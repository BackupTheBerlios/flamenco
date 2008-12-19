/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 ���������� �������.
 */
#include <flamenco/flamenco.h>
#include <algorithm>
#include <iostream>

using namespace flamenco;

namespace
{

// ������ ������� set_silence ��� ������ 16 bit.
inline void set_silence( s16 * buffer, u32 sizeInSamples )
{
    memset(buffer, 0, sizeInSamples << 1);
}

// �������������� s32 � s16 � ����������.
inline s16 clamp_sample( s32 value )
{
    return static_cast<s16>(value < -32768 ? -32768 : value > 32767 ? 32767 : value);
}

} // namespace

// ����������� ������ ��� ������� � �������.
critical_section Mixer::mCriticalSection;

// ��������� �������� ��� �������� ������ �� ������� �������.
const f32 Mixer::MAGIC = 5.9742e24f; // ����� ����� :)


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
    ::set_silence(buffer, MIXER_BUFFER_SIZE_IN_SAMPLES);
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
        s16 *left = buffer, *right = buffer + 1;
        for (u32 i = 0; i < CHANNEL_BUFFER_SIZE_IN_SAMPLES; ++i, left += 2, right += 2)
        {
            *left  = clamp_sample(*left  + static_cast<s32>(mBufferL[i] * (1 << 15)));
            *right = clamp_sample(*right + static_cast<s32>(mBufferR[i] * (1 << 15)));
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
