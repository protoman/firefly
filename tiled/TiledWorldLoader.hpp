//
// Created by iuri on 12/02/2026.
//

#ifndef FIREFLYGAME_TILEDWORLDLOADER_HPP
#define FIREFLYGAME_TILEDWORLDLOADER_HPP

#include <list>
#include <map>
#include <string>
#include <tmxlite/Map.hpp>
#include <utility>

enum e_tiled_world_layer_type {
    e_tiled_world_layer_type_image,
    e_tiled_world_layer_type_tileset,
    e_tiled_world_layer_type_other
};

struct tiled_world_tileset_origin_data {
    int origin_x;
    int origin_y;
    int w;
    int h;
    int dest_x;
    int dest_y;
    int tile_pos;
    std::string filename;
};


struct tiled_world_tileset_data {
    std::string name;
    std::string filename;
    int size;

    tiled_world_tileset_data() {
        size = 0;
    }

    tiled_world_tileset_data(std::string paramName, std::string paramFilename, int paramSize) {
        name = std::move(paramName);
        filename = std::move(paramFilename);
        size = paramSize;
    }
};

struct tiled_world_size {
    long width;
    long height;

    tiled_world_size()
    {
        width = 0;
        height = 0;
    }

    tiled_world_size(long w, long h)
    {
        width = w;
        height = h;
    }
};

struct tiled_world_image_layer_data {
    bool visible = true;
    float opacity = 1.00f;
    float shift_x = 0.00f;
    float shift_y = 0.00f;
    float parallax_x = 0.00f;
    float parallax_y = 0.00f;
    bool repeat_x = true;
    bool repeat_y = false;
    float current_pos_x = 0.0f;
    float current_pos_y = 0.0f;
    tiled_world_size image_size = tiled_world_size(0, 0);
    std::string image_filename;
};

class TiledWorldLoader {
public:
    TiledWorldLoader() = default;
    void loadMap(std::string filename);
    std::vector<tiled_world_tileset_origin_data> buildTileLayerData(unsigned int layerIndex);
    tiled_world_image_layer_data buildImageLayerData(unsigned int layerIndex);


    std::map<int, e_tiled_world_layer_type> getMapLayers();
    std::map<int, tiled_world_image_layer_data> getMapImageLayers();
    std::map<int, std::vector<tiled_world_tileset_origin_data>> getMapTilesetLayers();
    std::vector<tiled_world_tileset_data> getTilesetsInfo();

private:
    tmx::Map map;
    std::map<std::string, std::string> tileset_files_map;
    std::map<int, std::vector<tiled_world_tileset_origin_data>> map_layers_tiles;
    std::map<int, tiled_world_image_layer_data> map_layers_images;
    std::map<int, e_tiled_world_layer_type> world_map_layers;
    std::vector<tiled_world_tileset_data> tilesets;
};


#endif //FIREFLYGAME_TILEDWORLDLOADER_HPP
