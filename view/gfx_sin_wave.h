#ifndef GFX_SIN_WAVE_H
#define GFX_SIN_WAVE_H


#include "defines.h"
#include "data/st_common.h"

class gfx_sin_wave
{
public:
    gfx_sin_wave(st_imageData *set_surface);
    void set_speed(int set_speed);
    void set_max_amplitude(int set_max_amplitude);
    void show(int x, int y);

private:
    int angle;
    int speed;
    int amplitude;
    int max_amplitude;
    st_imageData *surface;
};

#endif // GFX_SIN_WAVE_H
