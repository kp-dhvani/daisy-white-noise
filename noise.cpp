#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod   hw;
WhiteNoise noise;

dsy_gpio cv_out_pin;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    float cv_level = hw.knob1.Process();
    noise.SetAmp(cv_level);
    for(size_t i = 0; i < size; i++)
    {
        float noise_value = noise.Process();
        float cv_out      = ((noise_value + 1.0f) * 2.5f) * cv_level;
        dsy_gpio_write(&cv_out_pin, cv_out);
        out[0][i] = noise_value * cv_level;
        out[1][i] = noise_value * cv_level;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    noise.Init();
    noise.SetSeed(1337);
    dsy_gpio_pin pin = {DSY_GPIOA, 6};
    cv_out_pin.pin   = pin;
    cv_out_pin.mode  = DSY_GPIO_MODE_ANALOG;
    dsy_gpio_init(&cv_out_pin);

    hw.StartAdc();
    hw.StartAudio(AudioCallback);
    Color led1_color;
    led1_color.Init(Color::PresetColor::BLUE);

    while(1)
    {
        hw.led1.SetColor(led1_color);
        hw.UpdateLeds();
        hw.DelayMs(50);
    }
}
