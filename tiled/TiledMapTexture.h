#ifndef TILEDMAPTEXTURE_H
#define TILEDMAPTEXTURE_H

#include <SDL.h>
#include <string>

class TiledMapTexture
{
public:
    TiledMapTexture();
    ~TiledMapTexture();


    TiledMapTexture(const TiledMapTexture&) = delete;
    TiledMapTexture(TiledMapTexture&&) = delete;

    TiledMapTexture& operator = (const TiledMapTexture&) = delete;
    TiledMapTexture& operator = (TiledMapTexture&&) = delete;

    bool loadFromFile(const std::string&, SDL_Renderer*);
    SDL_Point getSize() const { return m_size; }

    operator SDL_Texture* () {
        return m_texture;
    }

private:

    SDL_Texture* m_texture;
    SDL_Point m_size;
};

#endif // TILEDMAPTEXTURE_H
