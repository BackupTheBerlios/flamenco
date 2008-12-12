// ��������� ��� ������� ����������.

namespace flamenco
{

// �������� � ������������ ��� ������ �����.
// ������ �� ������ ������ ������������ � �������� �� ���������.
const u32 LATENCY_MSEC = 10;

// ����������� ���������.

// ������ ������ ������������ ��� ������ ������ � �������.
// (LATENCY_MSEC / 1000.0) * 44100
const u32 CHANNEL_BUFFER_SIZE_IN_SAMPLES = LATENCY_MSEC * 44100 / 1000;

// ������ ������ ������������, ������������� ������� (� ������������� ��������), � �������.
const u32 MIXER_BUFFER_SIZE_IN_SAMPLES = CHANNEL_BUFFER_SIZE_IN_SAMPLES * 2;


// ���������� ������ �����. ���������� �������������.
class OutputDevice : noncopyable
{
protected:
    // ��������� ������ �������������� ������. (��������� � ��������� ������� Mixer::mix)
    __forceinline void mix( const Mixer & mixer, u16 * buffer )
    {
        assert(buffer);
        mixer.mix(buffer);
    }
};

// ��� (������� ������� �������� ��������).
class Pin : noncopyable
{
public:
    virtual ~Pin() = 0;
    
private:
    // ��������� ������ ������� ��������� �������.
    // ������ ������� ������ - CHANNEL_BUFFER_SIZE_IN_SAMPLES �������.
    virtual void process( u16 * left, u16 * right ) = 0;
    friend class Mixer;
};
// ����� ��������� �� ���.
typedef std::tr1::shared_ptr<Pin> PinRef;

// ������ - ���, �������������� ������ ������� ����.
class Effect : public Pin
{
public:
    // ������� ������ � ������������ � ���� ������� ���.
    Effect( PinRef input );
    
private:
    // ������ �������� mInputPin.process() � ������������
    // ����������� �� ������. �� ����� � ��������� �� ��������������.
    virtual void process( u16 * left, u16 * right ) = 0;
};

// ������.
class Mixer : noncopyable
{
public:
    // ������������� ����. assert(!pin.connected()).
    void attach( PinRef pin );
    // ������������ ����. ���� �� �� �����������, warning � ���.
    void detach( PinRef pin );
    
private:
    // ��������� ����� ��������������� �������. ������ ������ - PCM 44100 16bit interleaved stereo.
    // ������ ������ ���������� - ����� MIXER_BUFFER_SIZE_IN_SAMPLES �������.
    //
    // ������� � ����� ��������� �� ������������ ����� � �������� �� ������� process() �� �����
    // ���������� �������. ����� ��������� ������������ ������ � �������� �����.
    //
    // ���� ���� � ����������: ������ ������� ������ ���� 2 ������ - �� ������ �� ����� ��� ��������
    // � Pin::process. � ����� ������ ����� ��������� ������ ���� ��� ������ ���� ��������� � ���������
    // ������. (��������, ��� ����� ��������, ����� ����� ����� ��� 2 ������ ��� ���������� �������� �������,
    // � ����� ��� �� ���������� � ������������ � ���������� �����).
    // ��� �������� ������ �� ������� ������� ������ ������ ���������� ������ ���� ������, ��� �����
    // � �������� �� ���������� ����������.
    void mix( u16 * buffer );
    
    friend class OutputDevice;
};


// ������� ����� � ��������.

// ���, ����������� ������ �� wav-�����.
class WavInput : public Pin
{
};

// ������, ���������� ��������� � ������.


} // namespace flamenco
