#ifndef TILEDMAPTEXTURE_H
#define TILEDMAPTEXTURE_H

#include <SDL3/SDL.h>

#include "data/st_imageData.hpp"

class TiledMapTexture
{
public:
    TiledMapTexture();
    ~TiledMapTexture();


    TiledMapTexture(const TiledMapTexture&) = delete;
    TiledMapTexture(TiledMapTexture&&) = delete;

    TiledMapTexture& operator = (const TiledMapTexture&) = delete;
    TiledMapTexture& operator = (TiledMapTexture&&) = delete;

    void set_image_data(const st_imageData& new_image_data);

    SDL_Point getSize() const {
        return SDL_Point(image_data.surface->w, image_data.surface->h);
    }

    operator SDL_Texture* () {
        return image_data.texture;
    }

private:
    st_imageData image_data;
};

#endif // TILEDMAPTEXTURE_H
