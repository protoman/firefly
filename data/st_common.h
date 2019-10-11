#ifndef ST_COMMON_H
#define ST_COMMON_H

// some small structures that do not need a separated file for each one
#include <vector>
#include <iostream>
#include <stdio.h>

#include "defines.h"
#include <SDL2/SDL.h>

extern SDL_Renderer* gRenderer;

/**
 * @brief
 *
 */
struct st_position {
    int x;
    int y;
    st_position() {
        x = 0;
        y = 0;
    }

    st_position (int setX, int setY) {
        x = setX;
        y = setY;
    }
    st_position& operator=(const st_position &set_pt) {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    bool operator==(const st_position &comp_pt) const {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_position &comp_pt) const {
        if (x != comp_pt.x || y != comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator<(const st_position &comp_pt) const {
        if (y == comp_pt.y && x < comp_pt.x) {
            return true;
        }
        if (y < comp_pt.y) {
            return true;
        }
        return false;
    }
};



/**
 * @brief
 *
 */
struct st_float_position {
    float x;
    float y;
    st_float_position() {
        x = 0.0;
        y = 0.0;
    }
/**
 * @brief
 *
 * @param setX
 * @param setY
 */
    st_float_position (float setX, float setY) {
        x = setX;
        y = setY;
    }
    /**
     * @brief
     *
     * @param set_pt
     * @return st_position &operator
     */
    st_float_position& operator=(const st_position &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    /**
     * @brief
     *
     * @param comp_pt
     * @return bool operator
     */
    bool operator==(const st_position &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
    /**
     * @brief
     *
     * @param set_pt
     * @return st_position &operator
     */
    st_float_position& operator=(const st_float_position &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    /**
     * @brief
     *
     * @param comp_pt
     * @return bool operator
     */
    bool operator==(const st_float_position &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
};

/**
 * @brief
 *
 */
struct st_size {
    int width;
    int height;
	st_size()
	{
            width = 0;
            height = 0;
	}
/**
 * @brief
 *
 * @param w
 * @param h
 */
    st_size(int w, int h)
	{
		width = w;
        height = h;
	}
};


/**
 * @brief
 *
 */
struct st_rectangle {
    int x;
    int y;
    int w;
    int h;
	st_rectangle () {
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}

    st_rectangle (int setX, int setY, int setW, int setH) {
		x = setX;
		y = setY;
		w = setW;
		h = setH;
	}

    bool is_empty() {
        if (x == 0 && y == 0 && w == 0 && h == 0) {
            return true;
        }
        return false;
    }
    bool operator==(const st_rectangle &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y && w == comp_pt.w && h == comp_pt.h) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_rectangle &comp_pt) const
    {
        if (x != comp_pt.x || y != comp_pt.y || w != comp_pt.w || h != comp_pt.h) {
            return true;
        }
        return false;
    }

    bool contains(int x_test, int y_test) {
        std::cout << ">>>>>>>> contains - x_test[" << x_test << "], y_test[" << y_test << "], x[" << x << "], y[" << y << "], w[" << w << "], h[" << h << "]" << std::endl;
        if (x_test >= x && x_test < x+w && y_test >= y && y_test < y+h) {
            return true;
        }
        return false;
    }

};

struct st_color {
    int r;
    int g;
    int b;
	st_color () {
		r = -1;
		g = -1;
		b = -1;
	}

    st_color (int setR, int setG, int setB) {
		r = setR;
		g = setG;
		b = setB;
	}
};



/**
 * @brief
 *
 */
struct used_teleporter {
    int teleporter_n; // number of the map_link (this is hardcoded, must be rebuilt in 0.2)
    bool finished; // indicates if already used
    struct st_position old_player_pos;
    bool active; // if true, then after beating the boss, the player must be teleported back to the teleporter origin
    st_float_position old_map_scroll;
    int old_map_n;
    bool is_object;
	used_teleporter() {
        teleporter_n = -1;
		finished = false;
		old_player_pos.x = 0;
		old_player_pos.y = 0;
		active = false;
		old_map_scroll.x = 0;
		old_map_scroll.y = 0;
		old_map_n = 0;
        is_object = false;
	}
};




struct st_position_int8 {
    int x;
    int y;
    st_position_int8() {
        x = 0;
        y = 0;
    }
    st_position_int8 (int setX, int setY) {
        x = setX;
        y = setY;
    }
    st_position_int8& operator=(const st_position_int8 &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    bool operator==(const st_position_int8 &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_position_int8 &comp_pt) const
    {
        if (x != comp_pt.x || y != comp_pt.y) {
            return true;
        }
        return false;
    }
};


struct st_map_collision {
    int block;
    int terrain_type;
    st_map_collision() {
        block = 0;
        terrain_type = 0;
    }
    st_map_collision(int set_block, int set_terrain_type) {
        block = set_block;
        terrain_type = set_terrain_type;
    }
};


struct st_menu_option {
    std::string text;
    bool disabled;

    st_menu_option(std::string line) {
        text = line;
        disabled = false;
    }

    st_menu_option(std::string line, bool set_disabled) {
        text = line;
        disabled = set_disabled;
    }
};

struct st_input_button_config {
    int type;                                           // use joystick_input_types
    int value;                                          // stores button-n or hat-n or witch axis (0 horizontal1, 1 vertical 1, 2 hoz 2...)
    int axis_type;                                      // only for axis, tell is positive (+1) or negative (-1) movement
    st_input_button_config() {
        type = JOYSTICK_INPUT_TYPE_BUTTON;
        value = -1;
        axis_type = 0;
    }
};

struct st_imageData {
    SDL_Texture* texture;
    SDL_Surface* surface;

    st_imageData() {
        texture = nullptr;
        surface = nullptr;
    }

    ~st_imageData() {
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // copy CONSTRUCTOR
    st_imageData(const st_imageData &original)
    {

        if (original.surface == nullptr) {
            surface = nullptr;
            texture = nullptr;
        } else {
            // copy surface
            surface = SDL_CreateRGBSurface(SDL_RLEACCEL , original.surface->w, original.surface->h, VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
            SDL_Rect srcRect = {0, 0, original.surface->w, original.surface->h};
            SDL_BlitSurface(original.surface, &srcRect, surface, nullptr);

            int access_mode;
            SDL_QueryTexture(original.texture, nullptr, &access_mode, nullptr, nullptr);
            if (access_mode == SDL_TEXTUREACCESS_TARGET) {
                texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, original.surface->w, original.surface->h);
            } else {
                texture = SDL_CreateTextureFromSurface(gRenderer, surface);
            }
        }
    }

    // assign constructor
    st_imageData& operator=(const st_imageData& original)
    {

        if (original.surface == nullptr) {
            surface = nullptr;
            texture = nullptr;
        } else {
            // copy surface
            surface = SDL_CreateRGBSurface(SDL_RLEACCEL , original.surface->w, original.surface->h, VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
            SDL_Rect srcRect = {0, 0, original.surface->w, original.surface->h};
            SDL_BlitSurface(original.surface, &srcRect, surface, nullptr);
            texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        }
        return *this;
    }

    void freeGraphic()
    {
        SDL_FreeSurface(surface);
        surface = nullptr;
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    bool is_null() {
        if (surface == nullptr || texture == nullptr) {
            return true;
        }
        return false;
    }


    Uint32 get_pixel(Sint16 x, Sint16 y)
    {
        if (surface == nullptr || surface->format == nullptr) {
            return 0;
        }
        if (x >= surface->w || y >= surface->h) {
            return 0;
        }

        int bpp = surface->format->BytesPerPixel;

        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;
        switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *) p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *) p;
        default:
            return 0;               /* shouldn't happen, but avoids warnings */
        }
    }


    void put_pixel(int x, int y, Uint32 pixel)
    {
        if (surface == nullptr || surface->format == nullptr) {
            return;
        }
        int bpp = surface->format->BytesPerPixel;
        // Here p is the address to the pixel we want to set //
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

        switch(bpp) {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
        }
    }

    SDL_Color get_pixel_color(Uint32 pixel) const {
        if (!surface) {
            return SDL_Color();
        }
        /*
        if (x >= gSurface->w || y >= gSurface->h) {
            return SDL_Color();
        }
        */

        SDL_Color theKey;
        //Uint32 pixel = ((Uint32*)gSurface->pixels)[y * gSurface->pitch/4 + x];
        SDL_GetRGB(pixel, surface->format, &theKey.r, &theKey.g, &theKey.b);

        return theKey;
    }



    st_color get_point_color(int x, int y) {
        Uint32 pixel = this->get_pixel(x, y);
        SDL_Color px_color = get_pixel_color(pixel);
        /*
        if (pixel != 0) {
            std::cout << "pixel[" << pixel << "]: [" << (int)px_color.r << "][" << (int)px_color.g << "][" << (int)px_color.b << "]" << std::endl;
        }
        */
        return st_color((int)px_color.r, (int)px_color.g, (int)px_color.b);
    }

    void set_point_color(int set_x, int set_y, int set_r, int set_g, int set_b) {
        /*
        if (set_r != 0 && set_g != 0 && set_b != 0) {
            std::cout << "set_point_color[" << set_x << "][" << set_y << "]: [" << set_r << "][" << set_g << "][" << set_b << "]" << std::endl;
        }
        */
        if (surface == NULL || surface->format == NULL) {
            return;
        }
        Uint32 new_color_n = SDL_MapRGB(surface->format, set_r, set_g, set_b);
        put_pixel(set_x, set_y, new_color_n);
    }



};



struct st_surface_with_direction {
    st_imageData surface[2];
};


struct st_dialog {
    unsigned long timer = 0;                // if greater than zero, instead of waiting keypress, dialog will wait this time to vanish
    std::string music_filename = "";        // plays a tune when dialog shows up
    std::string face_name = "";             // if provided, shows a character portrair in dialog window
    bool show_close_button = true;          // show a button on dialog window to close itself
    std::vector<std::string> msgs;          // text in the dialog
};


struct st_light_point {
    int x = 0;
    int y = 0;
    e_LIGHT_POINT_COLOR color;

    st_light_point(int set_x, int set_y, e_LIGHT_POINT_COLOR set_color) {
        x = set_x;
        y = set_y;
        color = set_color;
    }
};

struct st_dialog_status {

    bool started = false;
    unsigned long timer = 0;

    st_dialog_status() {
        reset();
    }

    void reset() {
        started = false;
        timer = 0;
    }
};


#endif // ST_COMMON_H
