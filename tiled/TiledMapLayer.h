#ifndef TILEDMAPLAYER_H
#define TILEDMAPLAYER_H


#include "TiledMapTexture.h"

#include "defines.h"
#include <vector>
#include <SDL.h>
#include <data/st_common.h>
#include <tmxlite/Map.hpp>

class TiledMapLayer
{
public:
    TiledMapLayer();

    bool init(const tmx::Map&map, std::uint32_t layerIndex, const std::vector<std::unique_ptr<TiledMapTexture>>& textures);
    void draw(SDL_Renderer* renderer, st_float_position map_scroll) const;
    std::vector<st_rectangle> get_tiles_collision_rectangles_list();

private:
    struct tiled_tile_data {
        int origin_x;
        int origin_y;
        int w;
        int h;
        int dest_x;
        int dest_y;
        E_TILE_COLLISION_SHAPE collision_shape;
        std::vector<st_float_position> points;
    };


    struct TilePieces final
    {
        std::vector<tiled_tile_data> tile_array;
        SDL_Texture* texture = nullptr;
    };
    std::vector<TilePieces> map_tiles;

    std::map<int, st_rectangle> tileset_collision_rectangles_map;
    std::vector<st_rectangle> tiles_collision_rectangles_list;

};

#endif // TILEDMAPLAYER_H
