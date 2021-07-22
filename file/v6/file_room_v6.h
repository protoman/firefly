#ifndef FILE_ROOM_H
#define FILE_ROOM_H

#include "defines.h"
#include "data/st_common.h"


struct file_v6_tile_piece {
    int x = -1;
    int y = -1;
    e_TILE_TYPE type = TILE_TYPE_SOLID;

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
};

struct file_v6_room {
    file_v6_room_tile tiles[AREA_ROOM_TILES_W][AREA_ROOM_TILES_H];
    int area_n = -1;
    st_position position;
};

#endif // FILE_ROOM_H
