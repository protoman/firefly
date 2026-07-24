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

    template <class Archive>
    void save(Archive & ar) const {
        std::string fname(filename);
        ar(CEREAL_NVP(adjust_y), CEREAL_NVP(speed), CEREAL_NVP(auto_scroll), CEREAL_NVP(repeatX), CEREAL_NVP(repeatY), CEREAL_NVP(alpha), CEREAL_NVP(animation_width), CEREAL_NVP(animation_timer), CEREAL_NVP(gfx), CEREAL_NVP(fname));
    }
    template <class Archive>
    void load(Archive & ar) {
        std::string fname;
        ar(adjust_y, speed, auto_scroll, repeatX, repeatY, alpha, animation_width, animation_timer, gfx, fname);
        strncpy(filename, fname.c_str(), FS_CHAR_FILENAME_SIZE);
        filename[FS_CHAR_FILENAME_SIZE-1] = '\0';
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

    template <class Archive>
    void save(Archive & ar) const {
        std::string name_s(style_name);
        std::string tileset_s(tileset_filename);
        std::string music_s(music_filename);
        std::vector<file_v6_style_layer> layers_v;
        layers_v.reserve(LAYERS_COUNT);
        for (int i=0;i<LAYERS_COUNT;i++) layers_v.push_back(layers[i]);
        ar(CEREAL_NVP(name_s), CEREAL_NVP(gfx_effect), CEREAL_NVP(gfx_mode), CEREAL_NVP(background_color), CEREAL_NVP(layers_v), CEREAL_NVP(tileset_s), CEREAL_NVP(music_s), CEREAL_NVP(bg_color));
    }
    template <class Archive>
    void load(Archive & ar) {
        std::string name_s, tileset_s, music_s;
        std::vector<file_v6_style_layer> layers_v;
        ar(name_s, gfx_effect, gfx_mode, background_color, layers_v, tileset_s, music_s, bg_color);
        strncpy(style_name, name_s.c_str(), V5_CHAR_NAME); style_name[V5_CHAR_NAME-1]='\0';
        strncpy(tileset_filename, tileset_s.c_str(), FS_CHAR_FILENAME_SIZE); tileset_filename[FS_CHAR_FILENAME_SIZE-1]='\0';
        strncpy(music_filename, music_s.c_str(), FS_CHAR_FILENAME_SIZE); music_filename[FS_CHAR_FILENAME_SIZE-1]='\0';
        for (int i=0;i<LAYERS_COUNT;i++) {
            if (i < (int)layers_v.size()) layers[i] = layers_v[i];
            else layers[i] = file_v6_style_layer();
        }
    }
};


#endif // FILE_STYLE_V6_H
