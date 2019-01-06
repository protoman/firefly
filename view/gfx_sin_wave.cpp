#include "data/st_common.h"
#include "gfx_sin_wave.h"
#include <math.h>

#include "view/imageview.h"
#include "view/timerview.h"

#define SIN_STEPS 6


gfx_sin_wave::gfx_sin_wave(st_imageData *set_surface)
{
    angle = 0;
    speed = 1;
    max_amplitude = TILESIZE*2;
    amplitude = max_amplitude;
    surface = set_surface;
}

void gfx_sin_wave::set_speed(int set_speed)
{
    speed = set_speed;
}

void gfx_sin_wave::set_max_amplitude(int set_max_amplitude)
{
    max_amplitude = set_max_amplitude;
}

void gfx_sin_wave::show(int x, int y)
{
    float angle_max = 3.14 * SIN_STEPS;
    float angle_step = angle_max / surface->surface->w;
    float angle = 0;
    for (int j=0; j<max_amplitude; j++) {
        // TODO::IURI //
        //ImageView::get_instance()->clear_area(x, y, surface->width, surface->height, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        for (int i=0; i<surface->surface->h; i++) {
            float pos_x = (sin(angle) + x)*amplitude;
            int pos_y = i + y;
            angle += angle_step;
            ImageView::get_instance()->renderTexturePortionAt(0, i, surface->surface->w, 1, pos_x, pos_y, surface->texture);
        }
        amplitude--;
        ImageView::get_instance()->updateScreen();
        TimerView::get_instance()->delay(40);
    }
}

