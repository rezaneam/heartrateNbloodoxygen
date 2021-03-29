#include <Arduino.h>

class Waveform
{
public:
#define MAX_LEN 128
#define MAX_VALUE 255
#define MIN_VALUE 0
    uint8_t length;

    void AddSample(int16_t value);
    void GetWave(uint8_t *a, uint8_t amplitude);
    void ResetMeasurment();

private:
    uint8_t waveform[MAX_LEN] = {0};

    void shift_left(uint8_t *a, uint8_t len, uint8_t newVal);
};