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
class sound_stream_base : public Pin
{
public:
    // ���� ������������� �����.
    atomic<bool> looping;
    
    // ���������� ������, ������� ����� ��������� ����.
    atomic_readonly<u32, sound_stream_base> loop_count;
    
    // ����� ��������� ������ � �������. �������������� ��� ������ �������.
    virtual u32 length() const = 0;
    
    // ����� �������� �������������� ������ (� ��������� �� LATENCY).
    atomic_readonly<u32, sound_stream_base> position;
    
protected:
    sound_stream_base()
        : looping(false), loop_count(0), position(0)
        {}
    
    // ��������� ������. ������� �����, �.�. loop_count � position ����� ��������� set().
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
     struct initializer;
     
     // �����������.
     decoder( const initializer & );
     
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
class stream : public Pin
{
public:
    // ��������� ��������.
    typedef typename decoderT::initializer initializer;
    
    // �������� ������.
    static reference<stream> create( const initializer & initializer );
    
    
private:
    stream( const initializer & initializer )
        : mDecoder(initializer)
        {}
    
    // ��������� ������ ������� ��������� �������, ����������� �� ��������.
    // ���� ������� ������ �������� �� ��������� � �������� ����������,
    // ��������� ��������������.
    void process( f32 * left, f32 * right );
    
    // ����� ��������� ������ � �������.
    inline u32 length() const
    {
        return mDecoder.length();
    }
    
    // �������.
    decoderT mDecoder;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_STREAM_H_
