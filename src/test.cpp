/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Пример использования библиотеки.
 */
#include <flamenco/flamenco.h>

int main()
{
    using namespace flamenco;
    reference<Pin> sine = Sine::create(1000);
    
    /*
    reference<Pin> sine = Kto-to->newSine(1000);// reference<Pin>(new Sine(1000));
    reference<Effect> volumepan = reference<Effect>(new VolumePan(sine));
    Mixer.attach(volumepan);
    
    input.process(left, right);
    for i in CHANNEL_BUFFER_SIZE_IN_SAMPLES:
        left[i] *= volumeLeftPan;
        right[i] *= volumeRightPan;
    
    setSilence(leftMixed, rightMixed);
    for e in effects:
        setSilence(left, right);
        e.process(left, right);
        add(left, leftMixed);
        add(right, rightMixed);
    */
}
