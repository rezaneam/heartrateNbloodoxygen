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

    // void scale()
    // {
    //     uint8_t maxw = 0;
    //     uint8_t minw = 255;
    //     for (int i = 0; i < MAXWAVE; i++)
    //     {
    //         maxw = waveform[i] > maxw ? waveform[i] : maxw;
    //         minw = waveform[i] < minw ? waveform[i] : minw;
    //     }
    //     uint8_t scale8 = (maxw - minw) / 4 + 1; //scale * 8 to preserve precision
    //     uint8_t index = wavep;
    //     for (int i = 0; i < MAXWAVE; i++)
    //     {
    //         disp_wave[i] = 31 - ((uint16_t)(waveform[index] - minw) * 8) / scale8;
    //         index = (index + 1) % MAXWAVE;
    //     }
    // }

    // void draw(uint8_t X)
    // {

    //     int MAXWAVE = 128;
    //     for (int i = 0; i < MAXWAVE; i++)
    //     {
    //         uint8_t y = disp_wave[i];
    //         oled.drawPixel(X + i, y * 3 / 4);
    //         if (i < MAXWAVE - 1)
    //         {
    //             uint8_t nexty = disp_wave[i + 1];
    //             if (nexty > y)
    //             {
    //                 for (uint8_t iy = y + 1; iy < nexty; ++iy)
    //                     oled.drawPixel(X + i, iy * 3 / 4);
    //             }
    //             else if (nexty < y)
    //             {
    //                 for (uint8_t iy = nexty + 1; iy < y; ++iy)
    //                     oled.drawPixel(X + i, iy * 3 / 4);
    //             }
    //         }
    //     }
    // }

private:
    uint8_t waveform[MAX_LEN] = {0};
    uint8_t position = 0;

    void shift_left(uint8_t *a, uint8_t len, uint8_t newVal);
};