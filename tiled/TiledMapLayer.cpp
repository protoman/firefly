#include "TiledMapLayer.h"

#include <iostream>
#include <cassert>
#include <map>

#include <tmxlite/TileLayer.hpp>
#include "view/imageview.h"

TiledMapLayer::TiledMapLayer() {}

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

    int tileCountX = 0;
    int tileCountY = 0;

    for (auto i = 0u; i < tileSets.size(); ++i) {
        //check tile ID to see if it falls within the current tile set
        const auto& ts = tileSets[i];

        // ignore tilkesets with no image
        if (ts.getImagePath().length() == 0) {
            continue;
        }

        auto terrains = ts.getTerrainTypes();

        const auto texSize = textures[i]->getSize();

        int tileset_w_tiles = texSize.x / mapTileSize.y;

        for (auto tile_piece : ts.getTiles()) {
            tileCountX = texSize.x / mapTileSize.x;
            tileCountY = texSize.y / mapTileSize.y;

            //push back to vert array
            tiled_tile_data tile;
            tile.origin_x = tile_piece.imagePosition.x;
            tile.origin_y = tile_piece.imagePosition.y;

            int tile_pos_x = tile.origin_x / ts.getTileSize().x;
            int tile_pos_y = tile.origin_y / mapTileSize.y;
            int calculated_array_pos = tile_pos_x + (tile_pos_y * tileset_w_tiles);

            tmx::ObjectGroup objectGroup = tile_piece.objectGroup;
            int objectSize = objectGroup.getObjects().size();

            for (int i_object=0; i_object<objectSize; i_object++) {
                tmx::Object object = objectGroup.getObjects().at(0);
                tmx::Object::Shape objectShape = object.getShape();

                if (objectShape == tmx::Object::Shape::Polygon) {
                    std::cout << "#### Tile[" << tile_pos_x << "][" << tile_pos_y << "].shape[Polygon]" << std::endl;
                    std::vector<tmx::Vector2f> objectPoints = object.getPoints();
                } else if (objectShape == tmx::Object::Shape::Rectangle) {
                    std::cout << "#### Tile[" << tile_pos_x << "][" << tile_pos_y << "].shape[Rectangle]" << std::endl;
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
        for (auto map_chunk : layer_chunks) {
            std::cout << "### map_chunk.size[" << map_chunk.size.x << "][" << map_chunk.size.y << "], position[" << map_chunk.position.x << "][" << map_chunk.position.y << "]" << std::endl;


            for (auto map_tile : map_chunk.tiles) {
                if (map_tile.ID != 0) {
                    int map_tile_index = map_tile.ID - 1;

                    tiled_tile_data tile;

                    tile.origin_x = tile_map[map_tile_index].origin_x;
                    tile.origin_y = tile_map[map_tile_index].origin_y;

                    tile.w = mapTileSize.x;
                    tile.h = mapTileSize.y;
                    tile.dest_x = map_x * tile.w;
                    tile.dest_y = map_y * tile.h;
                    map_tile_list.push_back(tile);
                    tiles_collision_rectangles_list.push_back(st_rectangle(tile.dest_x, tile.dest_y, tile.w, tile.h));

                    // add box2d shapes
                    //BodyDef bodyDef = getBodyDef(x * tileSize + tileSize / 2f + rectangle.getX() - (tileSize - rectangle.getWidth()) / 2f, y * tileSize + tileSize / 2f + rectangle.getY() - (tileSize - rectangle.getHeight()) / 2f);
                    //float box2d_x = tile.dest_x + float(tile.w/2);
                    //float box2d_y = tile.dest_y + float(tile.h/2);
                    //b2BodyDef bodyDef = getBodyDef(box2d_x, box2d_y);

                    //std::cout << "map_tile[" << map_x << "][" << map_y << "].id[" << map_tile.ID << "], origin_x[" << tile.origin_x << "], origin_y[" << tile.origin_y << "]" << std::endl;



                }

                map_x++;
                if (map_x >= mapSize.x) {
                    map_x = 0;
                    map_y++;
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
    return tiles_collision_rectangles_list;
}
