#include <Waveform.h>

void Waveform::shift_left(uint8_t *a, uint8_t len, uint8_t newVal)
{
    memmove(a, a + 1, sizeof(uint8_t) * (len - 1));
    a[len - 1] = newVal;
}

void Waveform::ResetMeasurment()
{
    std::fill(waveform, waveform + MAX_LEN, 0);
}

void Waveform::AddSample(int16_t value)
{
    value = (value / 8) + 128;
    value = value < MIN_VALUE ? MIN_VALUE : value;
    value = (uint8_t)(value > MAX_VALUE) ? MAX_VALUE : value;
    shift_left(waveform, length, value);
}

void Waveform::GetWave(uint8_t *displayArray, uint8_t amplitude)
{
    uint8_t min = 255;
    uint8_t max = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        max = waveform[i] > max ? waveform[i] : max;
        min = waveform[i] < min ? waveform[i] : min;
    }

    if (max == min)
        return;
    uint8_t scale8 = (max - min) / 4 + 1;
    for (uint8_t i = 0; i < length; i++)
        displayArray[i] = (amplitude - 1) - ((uint16_t)(waveform[i] - min) * (amplitude / 4)) / scale8;
}
