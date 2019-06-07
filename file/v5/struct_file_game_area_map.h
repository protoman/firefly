#ifndef STRUCT_FILE_GAME_AREA_MAP_H
#define STRUCT_FILE_GAME_AREA_MAP_H

#include "defines.h"
#include "data/st_common.h"

enum e_MAP_WALL_TYPES {
    MAP_WALL_TYPE_UNLOCKED,
    MAP_WALL_TYPE_DOOR,
    MAP_WALL_TYPE_LOCKED,
    MAP_WALL_TYPE_COUNT
};

struct st_file_game_area_room {

    int link[4];                // each link is a direction to the another linked room
    int item_number;            // if room contains a special item
    int map_n;
    bool visited;               // indicates if player already visited the room
    Uint8 wall_left;
    Uint8 wall_right;
    Uint8 wall_top;
    Uint8 wall_bottom;

    st_file_game_area_room() {
        for (int i=0; i<4; i++) {
            link[i] = -1;
        }
        item_number = -1;       // if room contains an item
        map_n = -1;
        visited = false;               // indicates if player already visited the room
        wall_left = MAP_WALL_TYPE_UNLOCKED;
        wall_right = MAP_WALL_TYPE_UNLOCKED;
        wall_top = MAP_WALL_TYPE_UNLOCKED;
        wall_bottom = MAP_WALL_TYPE_UNLOCKED;
    }
};


#endif // STRUCT_FILE_GAME_AREA_MAP_H
