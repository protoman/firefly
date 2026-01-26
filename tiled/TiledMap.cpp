#include "TiledMap.h"

#include <tmxlite/TileLayer.hpp>

#include <iostream>

#include "TiledMapTexture.h"

TiledMap::TiledMap() {}

void TiledMap::initialize(std::string filename, SDL_Renderer* renderer)
{
    if (map.load(filename)) {
        std::cout << "### TiledMap::initialize.filename[" << filename << "]" << std::endl;
        //load the textures as they're shared between layers
        const auto& tileSets = map.getTilesets();
        assert(!tileSets.empty());
        for (const auto& ts : tileSets) {
            if (ts.getTileCount() > 0 && ts.getImagePath().length() > 0) {
                textures.emplace_back(std::make_unique<TiledMapTexture>());
                if (!textures.back()->loadFromFile(ts.getImagePath(), renderer)) {
                    std::cerr << "Failed opening " << ts.getImagePath() << "\n";
                }
            }
        }

        //load the layers
        const auto& mapLayers = map.getLayers();
        for (auto i = 0u; i < mapLayers.size(); ++i) {
            if (mapLayers[i]->getType() == tmx::Layer::Type::Tile)
            {
                renderLayers.emplace_back(std::make_unique<TiledMapLayer>());
                renderLayers.back()->init(map, i, textures); //just cos we're using C++14
            }
        }

    }
}

void TiledMap::draw(SDL_Renderer *renderer, st_float_position map_scroll)
{
    for (const auto& l : renderLayers)
    {
        l->draw(renderer, map_scroll);
    }
}

std::vector<st_rectangle> TiledMap::get_tiles_collision(int layer_n)
{
    if (layer_n < 0 || layer_n >= renderLayers.size()) {
        return std::vector<st_rectangle>();
    }
    return renderLayers.at(layer_n)->get_tiles_collision_rectangles_list();
}
