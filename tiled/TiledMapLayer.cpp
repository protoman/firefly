#include "TiledMapLayer.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <map>

#include <tmxlite/TileLayer.hpp>
#include "view/imageview.h"

TiledMapLayer::TiledMapLayer() {}

#define PIXELS_PER_METER 40

/*

map
    layers
        - camada de blocos
            - size (40x20)
        - collision
    tileCount (40x20)
    tileSize (32x32)
    tilesets [1]
        tileIndex [970]
            int -> int (faz uma correspondência estranha, aqui pode estar o segredo
    tiles [970] - tem imagePosition, que é o origem que estou procurando


map_chunk.tiles[800] - só tem ids
ts.tiles[970] - imagePosition


Falta descobrir: onde fica a dest-pos.

*/
bool TiledMapLayer::init(const tmx::Map& map, uint32_t layerIndex, const std::vector<std::unique_ptr<TiledMapTexture>> &textures) {
    const auto& layers = map.getLayers();
    assert(layers[layerIndex]->getType() == tmx::Layer::Type::Tile);

    const auto& tileSets = map.getTilesets();
    const auto mapSize = map.getTileCount();

    int layer_n = -1;
    for (int layer_i=0; layer_i < layers.size(); layer_i++) {
        if (layers[layer_i]->getType() == tmx::Layer::Type::Tile) {
            layer_n = layer_i;
        }
        if (layers[layer_i]->getType() == tmx::Layer::Type::Image) {
            std::cout << "layer[" << layer_i << "].type = Image" << std::endl;
        }
    }
    if (layer_n == -1) {
        return false;
    }
    const tmx::TileLayer& layer = layers[layer_n]->getLayerAs<tmx::TileLayer>();

    const auto layer_tiles = layer.getTiles();
    const auto layer_chunks = layer.getChunks();
    const auto mapTileSize = map.getTileSize();

    std::cout << "layer_tiles.size[" << layer_tiles.size() << "]" << std::endl; // finite
    std::cout << "layer_chunks.size[" << layer_chunks.size() << "]" << std::endl; // infinite


    const auto& tileIDs = layer.getTiles();
    //std::vector<tiled_tile_data> tile_list;
    std::map<int, tiled_tile_data> tile_map;

    for (auto i = 0u; i < tileSets.size(); ++i) {
        //check tile ID to see if it falls within the current tile set
        const auto& ts = tileSets[i];

        // ignore tilesets with no image
        if (ts.getImagePath().empty()) {
            continue;
        }

        const auto texSize = textures[i]->getSize();

        unsigned int tileset_w_tiles = texSize.x / mapTileSize.y;

        for (const auto& tile_piece : ts.getTiles()) {
            //push back to vert array
            tiled_tile_data tile;
            tile.origin_x = tile_piece.imagePosition.x;
            tile.origin_y = tile_piece.imagePosition.y;

            unsigned int tile_pos_x = tile.origin_x / ts.getTileSize().x;
            unsigned int tile_pos_y = tile.origin_y / mapTileSize.y;
            unsigned int calculated_array_pos = tile_pos_x + (tile_pos_y * tileset_w_tiles);

            tmx::ObjectGroup objectGroup = tile_piece.objectGroup;
            unsigned int objectSize = objectGroup.getObjects().size();

            for (int i_object=0; i_object<objectSize; i_object++) {
                tmx::Object object = objectGroup.getObjects().at(0);
                tmx::Object::Shape objectShape = object.getShape();

                if (objectShape == tmx::Object::Shape::Polygon) {
                    //std::cout << "#### Tile[" << tile_pos_x << "][" << tile_pos_y << "].shape[Polygon]" << std::endl;
                    std::vector<tmx::Vector2f> objectPoints = object.getPoints();
                } else if (objectShape == tmx::Object::Shape::Rectangle) {
                    //std::cout << "#### Tile[" << tile_pos_x << "][" << tile_pos_y << "].shape[Rectangle]" << std::endl;
                    tileset_collision_rectangles_map[calculated_array_pos] = st_rectangle(0, 0, tile.w, tile.h);
                }
            }

            /*
            tmx::Object object1 = objectGroup.getObjects().at(0);
            tmx::Object object2 = objectGroup.getObjects().at(1);

            tmx::Object::Shape objectShape1 = object1.getShape();
            tmx::Object::Shape objectShape2 = object2.getShape();

            std::vector<tmx::Vector2f> objectPoints1 = object1.getPoints();
            std::vector<tmx::Vector2f> objectPoints2 = object2.getPoints();
            */

            tile_map[calculated_array_pos] = tile;
        }

        unsigned int map_x = 0;
        unsigned int map_y = 0;
        std::vector<tiled_tile_data> map_tile_list;

        // TODO - não está dando correspondência de map_tiles com a lista de tiles.
        for (const auto& map_chunk : layer_chunks) {

            std::cout << "### map_chunk.size[" << map_chunk.size.x << "][" << map_chunk.size.y << "], position[" << map_chunk.position.x << "][" << map_chunk.position.y << "], map_x[" << map_x << "], map_y[" << map_y << "]" << std::endl;

            for (auto map_tile : map_chunk.tiles) {
                if (map_tile.ID != 0) {
                    int map_tile_index = map_tile.ID - 1;

                    tiled_tile_data tile;

                    tile.origin_x = tile_map[map_tile_index].origin_x;
                    tile.origin_y = tile_map[map_tile_index].origin_y;

                    tile.w = mapTileSize.x;
                    tile.h = mapTileSize.y;
                    tile.dest_x = (map_chunk.position.x * tile.w) + (map_x * tile.w);
                    tile.dest_y = (map_chunk.position.y * tile.h) + (map_y * tile.h);
                    map_tile_list.push_back(tile);

                    // trying to make simple, just points in the map for the check
                    if (tiles_collision_adjacent_check_list.find(map_y) == std::end(tiles_collision_adjacent_check_list)) {
                        tiles_collision_adjacent_check_list.insert(std::pair<int, std::vector<st_collision_data>>(map_y, std::vector<st_collision_data>()));
                    }
                    st_collision_data collision_data;
                    collision_data.pos_x = (map_chunk.position.x * tile.w) + map_x;
                    collision_data.map_rect = st_rectangle(tile.dest_x, tile.dest_y, tile.w, tile.h);

                    tiles_collision_adjacent_check_list.at(map_y).push_back(collision_data);

                    /*
                    if (map_tile.ID == 490) {
                        std::cout << "map_tile[" << map_x << "][" << map_y << "].id[" << map_tile.ID << "], origin_x[" << tile.origin_x << "], origin_y[" << tile.origin_y << "], tile.dest_x[" << tile.dest_x << "], tile.dest_y[" << tile.dest_y << "], tile.w[" << tile.w << "], tile.h[" << tile.h << "]" << std::endl;
                    }
                    */
                }

                map_x++;
                if (map_x >= mapSize.x) {
                    map_x = 0;
                    map_y++;
                }
                if (map_y >= mapSize.y) {
                    map_y = 0;
                }
            }
        }

        if (!map_tile_list.empty())
        {
            map_tiles.emplace_back();
            std::cout << "### TiledMapLayer::init - i[" << i << "], textures.size[" << textures.size() << "]" << std::endl;
            map_tiles.back().texture = *textures[i];
            map_tiles.back().tile_array.swap(map_tile_list);
        }
    }

    optimize_tiles_collision_rectangles_list();
    return true;
}

void TiledMapLayer::draw(SDL_Renderer *renderer, st_float_position map_scroll) const
{
    assert(renderer);
    for (const auto& s : map_tiles)
    {
        for (const auto& t : s.tile_array) {
            // TODO: only draw tiles that are inside resolution window.
            //std::cout << "draw.tile - origin.x[" << t.origin_x << "], origin.y[" << t.origin_y << "], dest.x[" << t.dest_x << "], dest.y[" << t.dest_y << "]" << std::endl;
            ImageView::get_instance()->renderTexturePortionAt(t.origin_x, t.origin_y, t.w, t.h, t.dest_x - map_scroll.x, t.dest_y - map_scroll.y, s.texture);
        }
    }
    //SDL_Delay(20000);
}

std::vector<st_rectangle> TiledMapLayer::get_tiles_collision_rectangles_list()
{
    std::cout << "TiledMap::get_tiles_collision_rectangles_list - number of tiles [" << tiles_collision_rectangles_list.size() << "]" << std::endl;
    return tiles_collision_rectangles_list;
}

void TiledMapLayer::optimize_tiles_collision_rectangles_list() {
    for (const auto& pair : tiles_collision_adjacent_check_list) {
        int initial_pos_x = -1;
        int adjacent_pos = -1;
        float real_initial_pos_x = -1.0f;
        std::vector<st_collision_data> sorted_list = pair.second;
        std::sort(sorted_list.begin(), sorted_list.end(), [](const st_collision_data& a, const st_collision_data& b) {
            return a.pos_x < b.pos_x;
        });
        for (int i=0; i<sorted_list.size(); i++) {
            int pos_x = sorted_list.at(i).pos_x;
            //std::cout << "TiledMapLayer::optimize_tiles_collision_rectangles_list - checking tile[" << pos_x << "][" << pair.first << "]" << std::endl;
            if (initial_pos_x == -1) {
                initial_pos_x = pos_x;
                real_initial_pos_x = sorted_list.at(i).map_rect.x;
                adjacent_pos = pos_x + 1;
            }
            if (i < sorted_list.size()-1) {
                if (sorted_list.at(i+1).pos_x != pos_x+1) { // broken continuity
                    if (adjacent_pos >= pos_x) {
                        std::cout << "#1 Sequence - initial_pos_x[" << initial_pos_x << "], pos_x[" << pos_x << "], adjacent_pos[" << adjacent_pos << "]" << std::endl;
                        tiles_collision_rectangles_list.push_back(st_rectangle(real_initial_pos_x, sorted_list.at(i).map_rect.y, (sorted_list.at(i).map_rect.x - real_initial_pos_x + sorted_list.at(i).map_rect.w), sorted_list.at(i).map_rect.h));
                    }
                    initial_pos_x = -1;
                } else {
                    adjacent_pos++;
                }
            } else {
                if (adjacent_pos >= pos_x) {
                    std::cout << "#2 Sequence - initial_pos_x[" << initial_pos_x << "], pos_x[" << pos_x << "], adjacent_pos[" << adjacent_pos << "]" << std::endl;
                    tiles_collision_rectangles_list.push_back(st_rectangle(real_initial_pos_x, sorted_list.at(i).map_rect.y, (sorted_list.at(i).map_rect.x - real_initial_pos_x + sorted_list.at(i).map_rect.w), sorted_list.at(i).map_rect.h));
                }
                initial_pos_x = -1;
                adjacent_pos = pos_x + 1;
            }
        }
    }
    std::cout << "FINISH" << std::endl;
}


