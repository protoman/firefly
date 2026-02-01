#ifndef SHAREDMAPDATA_H
#define SHAREDMAPDATA_H

#include <vector>
#include <map>

#include "data/st_common.h"
#include "data/st_imageData.hpp"

#include <SDL2/SDL.h>

namespace map_data {

enum map_layer_type {
    map_layer_type_tileset,
    map_layer_type_image,
    map_layer_type_collision,
    map_layer_type_object,
    map_layer_type_other
};

enum enum_tile_collision_shape {
    tile_collision_shape_none,
    tile_collision_shape_rectangle,
    tile_collision_shape_polygon
};

struct map_tile_data {
    int origin_x;
    int origin_y;
    int w;
    int h;
    int dest_x;
    int dest_y;
    enum_tile_collision_shape collision_shape;
    std::vector<st_float_position> points;
};

struct map_image_layer_data {
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
    st_size image_size = st_size(0, 0);
    std::string image_filename = "";
    st_imageData image_data;
};

struct map_tileset {
    std::vector<map_tile_data> map_tiles;
    st_imageData image_data;
};

class SharedMapData
{
public:
    static SharedMapData* get_instance();

private:
    SharedMapData();
    SharedMapData(SharedMapData const&){};                              // copy constructor is private
    SharedMapData& operator=(SharedMapData const&){ return *this; };    // assignment operator is private

public:
    std::map<std::string, map_tileset> tileset_layer_data;
    std::map<std::string, map_image_layer_data> layer_data_map;   // index is the layer name
    std::map<std::string, map_layer_type> layer_order_map;
    st_float_position scroll;
    st_float_position scrolled;                                             // stores the value the map scrolled in this cycle. used for character movement control (it should move taking the scroll in account)

private:
    static SharedMapData* instance;
};

}

#endif // SHAREDMAPDATA_H
