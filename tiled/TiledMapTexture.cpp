#include "TiledMapTexture.h"

#include <iostream>
#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TiledMapTexture::TiledMapTexture() {}

TiledMapTexture::~TiledMapTexture()
{
    SDL_DestroyTexture(m_texture);
}

//public
bool TiledMapTexture::loadFromFile(const std::string& path, SDL_Renderer* renderer)
{
    std::cout << "### TiledMapTexture::loadFromFile[" << path << "]" << std::endl;
    assert(renderer != nullptr);
    assert(!path.empty());

    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t c = 0;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &c, 0);

    if (data)
    {
        std::int32_t pitch = x * c;
        pitch = (pitch + 3) & ~3;

        constexpr std::int32_t rmask = 0x000000ff;
        constexpr std::int32_t gmask = 0x0000ff00;
        constexpr std::int32_t bmask = 0x00ff0000;
        const std::int32_t amask = c == 4 ? 0xff000000 : 0;

        //SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
        //auto* surface = SDL_CreateRGBSurfaceFrom(data, x, y, c * 8, pitch, rmask, gmask, bmask, amask);
        // width = x, height = y, depth = c*8, pitch = pitch, rmask = rmask, gmask = gmask, bmask = bmask, amask = amask

        //SDL_CreateSurfaceFrom(int width, int height, SDL_PixelFormat format, void *pixels, int pitch);
        SDL_PixelFormat format = SDL_PIXELFORMAT_BGRA64;
        auto* surface = SDL_CreateSurfaceFrom(x, y, format, data, pitch);

        if (!surface)
        {
            std::cerr << "Unable to create texture surface: " << SDL_GetError() << "\n";
            stbi_image_free(data);
            return false;
        }

        m_texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_DestroySurface(surface);
        stbi_image_free(data);

        if (!m_texture)
        {
            std::cerr << "Failed to create texture for " << path << "\n";
            return false;
        }

        //sets alpha blending
        //SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
        m_size.x = x;
        m_size.y = y;

        return true;
    }

    return false;
}

