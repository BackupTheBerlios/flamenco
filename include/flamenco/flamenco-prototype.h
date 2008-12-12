// Примерный вид классов библиотеки.

namespace flamenco
{

// Задержка в милисекундах при выводе звука.
// Влияет на размер буфера микширования и нагрузку на процессор.
const u32 LATENCY_MSEC = 10;

// Производные константы.

// Размер буфера микширования для одного канала в семплах.
// (LATENCY_MSEC / 1000.0) * 44100
const u32 CHANNEL_BUFFER_SIZE_IN_SAMPLES = LATENCY_MSEC * 44100 / 1000;

// Размер буфера микширования, передаваемого микшеру (с чередующимися каналами), в семплах.
const u32 MIXER_BUFFER_SIZE_IN_SAMPLES = CHANNEL_BUFFER_SIZE_IN_SAMPLES * 2;


// Устройство вывода звука. Определено пользователем.
class OutputDevice : noncopyable
{
protected:
    // Получение порции смикшированных данных. (Интерфейс к приватной функции Mixer::mix)
    __forceinline void mix( const Mixer & mixer, u16 * buffer )
    {
        assert(buffer);
        mixer.mix(buffer);
    }
};

// Пин (элемент цепочки звуковых фильтров).
class Pin : noncopyable
{
public:
    virtual ~Pin() = 0;
    
private:
    // Заполняет буферы каналов звуковыми данными.
    // Размер каждого буфера - CHANNEL_BUFFER_SIZE_IN_SAMPLES семплов.
    virtual void process( u16 * left, u16 * right ) = 0;
    friend class Mixer;
};
// Умный указатель на пин.
typedef std::tr1::shared_ptr<Pin> PinRef;

// Эффект - пин, обрабатывающий данные другого пина.
class Effect : public Pin
{
public:
    // Создает эффект и присоединяет к нему входной пин.
    Effect( PinRef input );
    
private:
    // Обычно вызывает mInputPin.process() и обрабатывает
    // заполненные им буферы. Но может и заполнять их самостоятельно.
    virtual void process( u16 * left, u16 * right ) = 0;
};

// Микшер.
class Mixer : noncopyable
{
public:
    // Присоединение пина. assert(!pin.connected()).
    void attach( PinRef pin );
    // Отсоединение пина. Если он не присоединен, warning в лог.
    void detach( PinRef pin );
    
private:
    // Заполняет буфер смикшированными данными. Формат буфера - PCM 44100 16bit interleaved stereo.
    // Размер буфера фиксирован - равен MIXER_BUFFER_SIZE_IN_SAMPLES семплам.
    //
    // Функция в цикле пробегает по подключенным пинам и вызывает их функцию process() от своих
    // внутренних буферов. Затем смешивает получившиеся данные в итоговый буфер.
    //
    // Пара слов о реализации: внутри микшера должно быть 2 буфера - по одному на канал для передачи
    // в Pin::process. В таком случае после получения данных пина они должны быть примешаны к итоговому
    // буферу. (Возможно, это будет медленно, тогда нужно будет еще 2 буфера для смешивания отдельно каналов,
    // а затем уже их копировать с чередованием в переданный буфер).
    // Для проверки выхода за границы массива хорошо делать внутренние буферы чуть больше, чем нужно
    // и забивать их известными значениями.
    void mix( u16 * buffer );
    
    friend class OutputDevice;
};


// Примеры пинов и эффектов.

// Пин, считывающий данные из wav-файла.
class WavInput : public Pin
{
};

// Эффект, изменяющий громкость и баланс.


} // namespace flamenco
