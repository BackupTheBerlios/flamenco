/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 �������� �������� ������ �� ����� ��� ����� ��������� ������.
 */
#pragma once
#ifndef _FLAMENCO_CORE_STREAM_H_
#define _FLAMENCO_CORE_STREAM_H_

namespace flamenco
{

// ������� ����� ��� �������� �������.
// ����� ��� ����, ����� � ������� ������ ����� ����� ���� ����������
// �� ������ ����������.
class sound_stream_base : public pin
{
public:
    // ���� ������������� �����.
    atomic<bool> looping;
    
    // ������������� �� ���� (������������ � false, ����� ������������� ���� ������� � �����).
    atomic_readonly<bool, sound_stream_base> playing;
    
    // ���������� ������, ������� ����� ��������� ����. ���� ���� ����������, ������� ����� 0.
    atomic_readonly<u32, sound_stream_base> loop_count;
    
    // ����� ��������� ������ � �������. �������������� ��� ������ �������.
    virtual u32 length() const = 0;
    
    // ����� �������� �������������� ������ (� ��������� �� LATENCY).
    atomic_readonly<u32, sound_stream_base> position;
    
protected:
    sound_stream_base()
        : looping(false), playing(true), loop_count(0), position(0)
        {}
    
    // ��������� ������. ������� �����, �.�. ���������� ����� ��������� set().
    inline bool set_playing( bool value )
    {
        return playing.set(value);
    }
    inline u32 set_loop_count( u32 loopCount )
    {
        return loop_count.set(loopCount);
    }
    inline u32 set_position( u32 pos )
    {
        return position.set(pos);
    }
};

/*
 ��� �������� ������ ��������� ������ ����������:
 struct decoder
 {
     // �����������.
     decoder( std::auto_ptr<source> source );
     
     // �������� � ����� � ������ ������ count �������������� �������.
     // ���������� ���������� ������������� �������, ��� ����� ���� ������ count,
     // ���� ����� ����������.
     u32 unpack( f32 * left, f32 * right, u32 count );
     
     // ��������� ������� ������ ������������� �� �������� �����.
     void seek( u32 sample );
     
     // ����� ������ � �������. ������� �������������� ��� ������ �������.
     u32 length() const;
     
     // ������� ��������� ������ (��� �������������� ������� �������).
     u32 frequency() const;
 }
 */

// �������� �������� ������ �� ������ ��������� ��������.
template <class decoderT>
class stream : public sound_stream_base
{
public:
    // �������� ������.
    static reference<stream> create( std::auto_ptr<source> source );
    
    
private:
    stream( std::auto_ptr<source> source )
        : mDecoder(source)
    {
        if (!frequency_is_supported(mDecoder.frequency()))
            throw std::runtime_error("Unsupported sample rate.");
    }
    
    // ��������� ������ ������� ��������� �������, ����������� �� ��������.
    // ���� ������� ������ �������� �� ��������� � �������� ����������,
    // ��������� ��������������.
    void process( f32 * left, f32 * right );
    
    // ����� ��������� ������ � �������.
    inline u32 length() const
    {
        return mDecoder.length();
    }
    
    // �������������� �� ������� ����� �����������.
    bool frequency_is_supported( u32 freq )
    {
        return freq == 44100 || freq == 32000 || freq == 22050;
    }
    
    // �������.
    decoderT mDecoder;
};


// ����������.

template <class decoderT>
reference<stream<decoderT> > stream<decoderT>::create( std::auto_ptr<source> source )
{
    return reference<stream<decoderT> >(new stream<decoderT>(source));
}

template <class decoderT>
void stream<decoderT>::process( f32 * left, f32 * right )
{
    bool looping = this->looping();
    if (!looping && !playing())
        return;
    set_playing(true);
    
    // ��������� ������ �������. ���� ����������� ������, ��� �����, ������ ����� ����������.
    u32 count = mDecoder.unpack(left, right, CHANNEL_BUFFER_SIZE_IN_SAMPLES);
    if (count < CHANNEL_BUFFER_SIZE_IN_SAMPLES)
    {
        if (looping)
        {
            // ��������� �����, ����� ���� ����� � �����.
            u32 loopCount = 0, samplesRead = 0;
            while (count < CHANNEL_BUFFER_SIZE_IN_SAMPLES)
            {
                mDecoder.seek(0);
                samplesRead = mDecoder.unpack(left, right, CHANNEL_BUFFER_SIZE_IN_SAMPLES - count);
                count += samplesRead;
                loopCount++;
            }
            assert(count == CHANNEL_BUFFER_SIZE_IN_SAMPLES);
            set_loop_count(loop_count() + loopCount);
            set_position(samplesRead);
        }
        else // ���� ������� �� �����, ���������� �����.
        {
            set_loop_count(0);
            mDecoder.seek(0);
            set_position(0);
            set_playing(false);
        }
    }
    else
    {
        assert(count == CHANNEL_BUFFER_SIZE_IN_SAMPLES);
        set_position(position() + CHANNEL_BUFFER_SIZE_IN_SAMPLES);
    }
}

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
