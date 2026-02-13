//
// Created by iuri on 12/02/2026.
//

#include "TiledWorldLoader.hpp"

#include <iostream>
#include <tmxlite/ImageLayer.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "data/sharedmapdata.h"

void TiledWorldLoader::loadMap(std::string filename) {
    std::cout << "### TiledMap::initialize.filename[" << filename << "]" << std::endl;
    if (!map.load(filename)) {
        std::cout << "Error loading map" << std::endl;
        return;
    }

    map_properties.chunk_w = map.getTileCount().x;
    map_properties.chunk_h = map.getTileCount().y;
    // TODO - those are meant to be different, need to foind out witch is witch
    map_properties.map_w = map.getTileCount().x;
    map_properties.map_h = map.getTileCount().y;
    map_properties.tile_w = map.getTileSize().x;
    map_properties.tile_h = map.getTileSize().y;
    map_properties.bg_color.r = map.getBackgroundColour().r;
    map_properties.bg_color.g = map.getBackgroundColour().g;
    map_properties.bg_color.b = map.getBackgroundColour().b;
    map_properties.directory = map.getWorkingDirectory();

    //load the textures as they're shared between layers
    const auto& tileSets = map.getTilesets();
    assert(!tileSets.empty());
    for (const auto& ts : tileSets) {
        if (ts.getTileCount() > 0 && ts.getImagePath().length() > 0) {
            tileset_files_map[ts.getName()] = ts.getImagePath();
        }
    }

    //load the layers
    const auto& mapLayers = map.getLayers();
    for (auto i = 0u; i < mapLayers.size(); ++i) {
        if (mapLayers[i]->getType() == tmx::Layer::Type::Tile) {
            world_map_layers[i] = e_tiled_world_layer_type_tileset;
            map_layers_tiles[i] = buildTileLayerData(i);
        } else if (mapLayers[i]->getType() == tmx::Layer::Type::Image) {
            world_map_layers[i] = e_tiled_world_layer_type_image;
            map_layers_images[i] = buildImageLayerData(i);
        } else {
            world_map_layers[i] = e_tiled_world_layer_type_other;
        }
    }
}

std::vector<tiled_world_tileset_origin_data> TiledWorldLoader::buildTileLayerData(unsigned int layerIndex) {
    const auto& layers = map.getLayers();
    assert(layers[layerIndex]->getType() == tmx::Layer::Type::Tile);

    const auto& tileSets = map.getTilesets();
    const auto mapSize = map.getTileCount();

    int layer_n = -1;
    for (int layer_i=0; layer_i < layers.size(); layer_i++) {
        if (layers[layer_i]->getType() == tmx::Layer::Type::Tile) {
            const tmx::TileLayer& tempLayer = layers[layer_i]->getLayerAs<tmx::TileLayer>();
            long tempLayerX = tempLayer.getSize().x;
            long tempLayerY = tempLayer.getSize().y;
            layer_n = layer_i;
        }
    }
    if (layer_n == -1) {
        return std::vector<tiled_world_tileset_origin_data>();
    }
    const tmx::TileLayer& layer = layers[layer_n]->getLayerAs<tmx::TileLayer>();

    const auto layer_tiles = layer.getTiles();
    const auto layer_chunks = layer.getChunks();
    const auto mapTileSize = map.getTileSize();

    std::cout << "layer_tiles.size[" << layer_tiles.size() << "]" << std::endl; // finite
    std::cout << "layer_chunks.size[" << layer_chunks.size() << "]" << std::endl; // infinite


    const auto& tileIDs = layer.getTiles();
    std::map<int, tiled_world_tileset_origin_data> tile_map;

    // This part build the tiles from the tileset
    unsigned int tiles_size = tileSets.size();
    for (auto i = 0u; i < tiles_size; ++i) {
        //check tile ID to see if it falls within the current tile set
        const auto& ts = tileSets[i];
        long tileCount = ts.getTileCount();


        // ignore tilesets with no image
        if (ts.getImagePath().empty()) {
            continue;
        }
        tilesets.emplace_back(ts.getName(), ts.getImagePath(), tileCount);

        std::string tilesetImageFilename = ts.getImagePath();
        unsigned int tileset_w_tiles = ts.getImageSize().x / mapTileSize.y;

        for (const auto& tile_piece : ts.getTiles()) {
            //push back to vert array
            tiled_world_tileset_origin_data tile;
            tile.filename = tilesetImageFilename;
            tile.origin_x = tile_piece.imagePosition.x;
            tile.origin_y = tile_piece.imagePosition.y;

            // TODO - fill data

            unsigned int tile_pos_x = tile.origin_x / ts.getTileSize().x;
            unsigned int tile_pos_y = tile.origin_y / mapTileSize.y;
            unsigned int calculated_array_pos = tile_pos_x + (tile_pos_y * tileset_w_tiles);

            tile_map[calculated_array_pos] = tile;
        }
    }

    unsigned int map_x = 0;
    unsigned int map_y = 0;

    std::vector<tiled_world_tileset_origin_data> map_tile_list;

    // This part builds the map tiles
    int map_x_multi = 0;
    for (const auto& map_chunk : layer_chunks) {
        //std::cout << "### map_chunk.size[" << map_chunk.size.x << "][" << map_chunk.size.y << "], position[" << map_chunk.position.x << "][" << map_chunk.position.y << "]" << std::endl;

        for (auto map_tile : map_chunk.tiles) {
            if (map_tile.ID != 0) {
                int map_tile_index = map_tile.ID - 1;
                tiled_world_tileset_origin_data tile;
                tile.tile_pos = map_tile_index;
                tile.origin_x = tile_map[map_tile_index].origin_x;
                tile.origin_y = tile_map[map_tile_index].origin_y;
                tile.w = mapTileSize.x;
                tile.h = mapTileSize.y;


                int map_x_adjust = map_x_multi * mapSize.x * tile.w;
                tile.dest_x = map_x * tile.w + map_x_adjust;
                tile.dest_y = map_y * tile.h;
                map_tile_list.push_back(tile);
                //std::cout << "map_tile_index[" << map_tile_index << "], map_x_multi[" << map_x_multi << "], map_x_adjust[" << map_x_adjust << "], ""map_tile[" << map_x << "][" << map_y << "].id[" << map_tile.ID << "], origin_x[" << tile.origin_x << "], origin_y[" << tile.origin_y << "], dest[" << tile.dest_x << "][" << tile.dest_y << "]" << std::endl;
            }

            map_x++;
            if (map_x >= mapSize.x) {
                map_x = 0;
                map_y++;
            }
            if (map_y >= mapSize.y) {
                // TODO - need to add an X multiplier because the map moved to the next one on the right
                map_y = 0;
                map_x_multi++;
            }
        }
    }
    std::cout << "END" << std::endl;
    return map_tile_list;
}

tiled_world_image_layer_data TiledWorldLoader::buildImageLayerData(unsigned int layerIndex) {
    const auto& layers = map.getLayers();
    assert(layers[layerIndex]->getType() == tmx::Layer::Type::Image);

    tmx::ImageLayer image_layer = layers[layerIndex]->getLayerAs<tmx::ImageLayer>();
    tiled_world_image_layer_data image_layer_data;
    image_layer_data.visible = image_layer.getVisible();
    image_layer_data.opacity = image_layer.getOpacity();
    image_layer_data.shift_x = image_layer.getOffset().x;
    image_layer_data.shift_y = image_layer.getOffset().y;
    image_layer_data.current_pos_x = image_layer.getOffset().x;
    image_layer_data.current_pos_y = image_layer.getOffset().y;
    image_layer_data.parallax_x = image_layer.getParallaxFactor().x;
    image_layer_data.parallax_y = image_layer.getParallaxFactor().y;
    image_layer_data.repeat_x = image_layer.hasRepeatX();
    image_layer_data.repeat_y = image_layer.hasRepeatY();
    image_layer_data.image_size = tiled_world_size(image_layer.getImageSize().x, image_layer.getImageSize().y);
    image_layer_data.image_filename = image_layer.getImagePath();

    return image_layer_data;
}

std::map<int, e_tiled_world_layer_type> TiledWorldLoader::getMapLayers()
{
    return world_map_layers;
}

std::map<int, tiled_world_image_layer_data> TiledWorldLoader::getMapImageLayers()
{
    return map_layers_images;
}

std::map<int, std::vector<tiled_world_tileset_origin_data> > TiledWorldLoader::getMapTilesetLayers()
{
    return map_layers_tiles;
}

std::vector<tiled_world_tileset_data> TiledWorldLoader::getTilesetsInfo() {
    return tilesets;
}

tiled_world_map_properties TiledWorldLoader::getMapProperties() {
    return map_properties;
}