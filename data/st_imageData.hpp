#ifndef ST_IMAGEDATA_HPP
#define ST_IMAGEDATA_HPP

#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>

#include "defines.h"
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
            SDL_DestroySurface(surface);
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
            surface = SDL_CreateSurface(original.surface->w, original.surface->h, SDL_GetPixelFormatForMasks(VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000));
            SDL_Rect srcRect = {0, 0, original.surface->w, original.surface->h};
            SDL_BlitSurface(original.surface, &srcRect, surface, nullptr);

            SDL_PropertiesID sdlPropertiesId = SDL_GetTextureProperties(original.texture);
            Sint64 access_mode = 0;
            if (sdlPropertiesId != 0) {
                access_mode = SDL_GetNumberProperty(sdlPropertiesId, SDL_PROP_TEXTURE_ACCESS_NUMBER, 0);
            }
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
            surface = SDL_CreateSurface(original.surface->w, original.surface->h, SDL_GetPixelFormatForMasks(VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000));

            SDL_Rect srcRect = {0, 0, original.surface->w, original.surface->h};
            SDL_BlitSurface(original.surface, &srcRect, surface, nullptr);
            texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        }
        return *this;
    }

    void freeGraphic()
    {
        if (is_null() == false) {
            SDL_DestroySurface(surface);
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





};


#endif // ST_IMAGEDATA_HPP
