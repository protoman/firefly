#ifndef FILE_STYLE_V6_H
#define FILE_STYLE_V6_H

#include "defines.h"
#include "data/st_common.h"

struct file_v6_style_layer {
    int adjust_y;                               // distance of the background from the screen top
    int speed;                                  // 0 means will scroll in the same speed as foreground (so the bg is used only to ease map-creation)
    int auto_scroll;                            // automatic move background even if player isn't moving. 0: don't move, 1: horizontal, 2: vertical
    bool repeatX;                               // if true, will repear on X axis to fill the screen
    bool repeatY;                               // if true, will repear on X axis to fill the screen
    int alpha;
    int animation_width;                        // tells what is the image width we need to use as frame-size
    int animation_timer;                        // defines the time between animation frames
    int gfx;
    char filename[FS_CHAR_FILENAME_SIZE];

    file_v6_style_layer()
    {
        filename[0] = '\0';
        adjust_y = 0;
        speed = 0;
        auto_scroll = 0;
        sprintf(filename, "%s", "");
        repeatX = true;
        repeatY = false;
        alpha = SDL_ALPHA_OPAQUE;
        animation_width = 0;
        animation_timer = 0;
        gfx = 0;
    }
};

struct file_v6_style {
    char style_name[V5_CHAR_NAME];
    int gfx_effect;                                     // indicates that the map have an gfx executed by drawlib (rain, snow, train, etc)
    int gfx_mode;
    st_color background_color;
    file_v6_style_layer layers[LAYERS_COUNT];
    char tileset_filename[FS_CHAR_FILENAME_SIZE];
    char music_filename[FS_CHAR_FILENAME_SIZE];         //optional
    st_color bg_color;


    file_v6_style() {
        sprintf(style_name, "%s", "");
        gfx_effect = 0;
        gfx_mode = 0;
        sprintf(tileset_filename, "%s", "");
        sprintf(music_filename, "%s", "");
    }
};


#endif // FILE_STYLE_V6_H
