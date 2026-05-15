#include "TiledMap.h"

#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ImageLayer.hpp>

#include <iostream>

#include "TiledMapTexture.h"
#include "data/sharedmapdata.h"

#include <data/sharedmapdata.h>

#include "TiledMapObjectLayer.hpp"
#include "view/imageview.h"

TiledMap::TiledMap() = default;

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
                textures.back()->set_image_data(ImageView::get_instance()->imageFromFile(ts.getImagePath()));
            }
        }

        //load the layers
        const auto& mapLayers = map.getLayers();
        for (auto i = 0u; i < mapLayers.size(); ++i) {
            map_data::map_layer_type map_layer_type = map_data::map_layer_type::map_layer_type_other;
            if (mapLayers[i]->getType() == tmx::Layer::Type::Tile) {
                renderLayers.emplace_back(std::make_unique<TiledMapLayer>());
                renderLayers.back()->init(map, i, textures); //just cos we're using C++14
                map_layer_type = map_data::map_layer_type::map_layer_type_tileset;
            } else if (mapLayers[i]->getType() == tmx::Layer::Type::Image) {
                build_map_image_layer_data(mapLayers[i]);
                map_layer_type = map_data::map_layer_type::map_layer_type_image;
            } else if (mapLayers[i]->getType() == tmx::Layer::Type::Object) {
                map_layer_type = map_data::map_layer_type::map_layer_type_object;
                if (mapLayers[i]->getName() == "COLLISION") {
                    TiledMapObjectLayer objectLayer;
                    objects_collision = objectLayer.init(map, i);
                }
            }
            map_data::SharedMapData::get_instance()->layer_order_map[mapLayers[i]->getName()] = map_layer_type;
        }

    }
}

// TODO - remove. The map should be entirely loaded into sharedmapdata,
// and the draw class should just init (load any images) and then use it.
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
        std::cout << "TiledMap::get_tiles_collision - no tiles" << std::endl;
        return {};
    }
    return renderLayers.at(layer_n)->get_tiles_collision_rectangles_list();
}

std::vector<std::vector<st_float_position>> TiledMap::get_objects_collision() {
    return objects_collision;
}

void TiledMap::build_map_image_layer_data(const tmx::Layer::Ptr& layer)
{
    tmx::ImageLayer image_layer = layer->getLayerAs<tmx::ImageLayer>();
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()] = map_data::map_image_layer_data();
    /*
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
    std::string image_filename = "";
    SDL_Texture* texture = nullptr;
    */
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].visible = layer->getVisible();
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].opacity = layer->getOpacity();
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].shift_x = image_layer.getOffset().x;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].shift_y = layer->getOffset().y;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].current_pos_x = image_layer.getOffset().x;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].current_pos_y = layer->getOffset().y;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].parallax_x = layer->getParallaxFactor().x;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].parallax_y = layer->getParallaxFactor().y;
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].repeat_x = image_layer.hasRepeatX();
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].repeat_y = image_layer.hasRepeatY();
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].image_size = st_size(image_layer.getImageSize().x, image_layer.getImageSize().y);
    map_data::SharedMapData::get_instance()->layer_data_map[layer->getName()].image_filename = image_layer.getImagePath();


}

st_color TiledMap::get_map_background_color()
{
    return st_color(map.getBackgroundColour().r, map.getBackgroundColour().g, map.getBackgroundColour().b);
}

void TiledMap::build_map_player_data(const tmx::Layer::Ptr& layer) {
    tmx::ObjectGroup object_layer = layer->getLayerAs<tmx::ObjectGroup>();
    std::vector<tmx::Object> object_list = object_layer.getObjects();
    for (tmx::Object object : object_list) {
        //object.getPoints()
    }
}
