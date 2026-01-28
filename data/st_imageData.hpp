#ifndef ST_IMAGEDATA_HPP
#define ST_IMAGEDATA_HPP

#include <SDL2/SDL.h>

#include "defines.h";
#include "data/st_color.hpp"

extern SDL_Renderer* gRenderer;

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
        freeGraphic();
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
        if (is_null() == false) {
            SDL_FreeSurface(surface);
            surface = nullptr;
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
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


#endif // ST_IMAGEDATA_HPP
