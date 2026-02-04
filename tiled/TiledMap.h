#ifndef TILEDMAP_H
#define TILEDMAP_H

#include "TiledMapTexture.h"
#include "TiledMapLayer.h"

#include <tmxlite/Map.hpp>

class TiledMap
{
public:
    TiledMap();

    void initialize(std::string filename, SDL_Renderer *renderer);
    void draw(SDL_Renderer *renderer, st_float_position map_scroll);
    std::vector<st_rectangle> get_tiles_collision(int layer_n);

    std::vector<std::vector<st_float_position>> get_objects_collision();

    void build_map_image_layer_data(const tmx::Layer::Ptr &layer);
    st_color get_map_background_color();

    void build_map_player_data(const tmx::Layer::Ptr &layer);

private:
    tmx::Map map;
    std::vector<std::unique_ptr<TiledMapTexture>> textures;
    std::vector<std::unique_ptr<TiledMapLayer>> renderLayers;
    std::vector<std::vector<st_float_position>> objects_collision;

};

#endif // TILEDMAP_H
