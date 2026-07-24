#ifndef FILE_ROOM_H
#define FILE_ROOM_H

#include "defines.h"
#include "data/st_common.h"


#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

struct file_v6_tile_piece {
    int x = -1;
    int y = -1;
    e_TILE_TYPE type = TILE_TYPE_SOLID;

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(type));
    }
};

struct file_v6_room_tile {
    int locked = 0;
    struct file_v6_tile_piece tile_underlay;                  // position of tile (x and y) in the graphic file (each tile is 16*16)
    struct file_v6_tile_piece tile_overlay;                  // tird level, to be shown over player
    file_v6_room_tile() {
        locked = 0;
        tile_overlay.x = -1;
        tile_overlay.y = -1;
        tile_overlay.type = TILE_TYPE_SOLID;

        tile_underlay.x = -1;
        tile_underlay.y = -1;
        tile_underlay.type = TILE_TYPE_SOLID;
    }

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(locked), CEREAL_NVP(tile_underlay), CEREAL_NVP(tile_overlay));
    }
};

struct file_v6_room {
    file_v6_room_tile tiles[AREA_ROOM_TILES_W][AREA_ROOM_TILES_H];
    int stage_n = 0;
    int area_n = 0;
    st_position position;

    template <class Archive>
    void save(Archive & ar) const {
        // convert tiles C-array to vector for serialization
        std::vector<file_v6_room_tile> tiles_vec;
        tiles_vec.reserve(AREA_ROOM_TILES_W * AREA_ROOM_TILES_H);
        for (int x = 0; x < AREA_ROOM_TILES_W; ++x) for (int y = 0; y < AREA_ROOM_TILES_H; ++y) tiles_vec.push_back(tiles[x][y]);
        ar(CEREAL_NVP(tiles_vec), CEREAL_NVP(stage_n), CEREAL_NVP(area_n), CEREAL_NVP(position));
    }

    template <class Archive>
    void load(Archive & ar) {
        std::vector<file_v6_room_tile> tiles_vec;
        ar(tiles_vec, stage_n, area_n, position);
        // copy back into C-array (truncate if needed)
        int idx = 0;
        for (int x = 0; x < AREA_ROOM_TILES_W; ++x) {
            for (int y = 0; y < AREA_ROOM_TILES_H; ++y) {
                if (idx < (int)tiles_vec.size()) tiles[x][y] = tiles_vec[idx++];
                else tiles[x][y] = file_v6_room_tile();
            }
        }
    }
};



#endif // FILE_ROOM_H
