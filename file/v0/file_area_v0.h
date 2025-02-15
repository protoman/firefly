#ifndef FILE_AREA_V0_H
#define FILE_AREA_V0_H

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"

#include "defines.h"
#include "data/st_common.h"

struct file_tile_piece_v0 {
    int x = -1;
    int y = -1;
    e_TILE_TYPE type = TILE_TYPE_SOLID;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(type));
    }
};

struct file_room_tile_v0 {
    int locked = 0;
    struct file_tile_piece_v0 tile_underlay;                  // position of tile (x and y) in the graphic file (each tile is 16*16)
    struct file_tile_piece_v0 tile_overlay;                  // tird level, to be shown over player

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(tile_underlay), CEREAL_NVP(tile_overlay));
    }

    file_room_tile_v0() {
        locked = 0;
        tile_overlay.x = -1;
        tile_overlay.y = -1;
        tile_overlay.type = TILE_TYPE_SOLID;

        tile_underlay.x = -1;
        tile_underlay.y = -1;
        tile_underlay.type = TILE_TYPE_SOLID;
    }
};

struct file_room_v0 {
    std::map<st_position, file_room_tile_v0> tiles;
    int stage_n = 0;
    int area_n = 0;
    st_position position;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(tiles), CEREAL_NVP(stage_n), CEREAL_NVP(area_n), CEREAL_NVP(position));
    }

};

struct file_stage_v0 {
    std::string stage_name;
    int style;
    std::string music_filename;
    std::vector<file_room_v0> rooms;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(stage_name), CEREAL_NVP(style), CEREAL_NVP(music_filename), CEREAL_NVP(rooms));
    }

    file_stage_v0() {
        stage_name = "";
        music_filename = "";
    }
};

struct file_area_v0 {
    int style;
    std::string area_name;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(style), CEREAL_NVP(area_name));
    }

    file_area_v0() {
        style = 0;
        area_name = "New Area";
    }
};


struct stage_object_v0 {
    int uuid = -1;
    int currentMap = -1;
    int id_object = -1;
    st_position start_point;
    ANIM_DIRECTION direction = ANIM_DIRECTION_RIGHT;
    st_position dest_position; // teleporter data, also used for objects that have sizes/orientation defined while placing them in editor //
    int dest_map = -1;
    int dest_level = -1;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(uuid), CEREAL_NVP(currentMap), CEREAL_NVP(id_object), CEREAL_NVP(start_point),
              CEREAL_NVP(direction), CEREAL_NVP(dest_position), CEREAL_NVP(dest_map), CEREAL_NVP(dest_level));
    }
};


struct file_map_npc_v0 {
    int id_npc;
    st_position start_point;
    ANIM_DIRECTION direction;

    file_map_npc_v0() {
        id_npc = -1;
        direction = ANIM_DIRECTION_LEFT;
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(id_npc), CEREAL_NVP(direction));
    }
};



#endif // FILE_AREA_V0_H
