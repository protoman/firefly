#ifndef STRUCT_FILE_MAP_H
#define STRUCT_FILE_MAP_H

#include "defines.h"
#include "data/st_common.h"

/*
cada mapa terá seu próprio diretório
map_links - arquivo universal, contém a ligação entra os mapas. sempre é bi-direcional.

MAPA:
header - contém "instruções" do mapa, como tamanho e nome, backgrounds e overlays
tiles - contém os tiles na forma de uma matriz, de acordo com o tamanho definido
enemies - inimigos colocados no mapa
objects - objectos colocados no mapa

*/



#define BACKGROUND_LAYERS_MAX 15
#define BACKGROUND_LAYERS_BG_COUNT 10
#define BACKGROUND_LAYERS_FG_COUNT 5

struct st_tile_piece {
    int x;
    int y;
    int type;

    st_tile_piece() {
        x = -1;
        y = -1;
        type = TILE_TYPE_SOLID;
    }
};


struct file_v5_map_tile {
    int locked;
    struct st_tile_piece tile_underlay;                  // position of tile (x and y) in the graphic file (each tile is 16*16)
    struct st_tile_piece tile_overlay;                  // tird level, to be shown over player
    file_v5_map_tile() {
        locked = 0;
        tile_overlay.x = -1;
        tile_overlay.y = -1;
    }
};

struct file_v5_map_background {
    char filename[FS_CHAR_FILENAME_SIZE];
    int adjust_y;                             // distance of the background from the screen top
    int speed;                                // 0 means will scroll in the same speed as foreground (so the bg is used only to ease map-creation)
    unsigned int auto_scroll;                 // automatic move background even if player isn't moving. 0: don't move, 1: horizontal, 2: vertical
    bool repeatX;                            // if true, will repear on X axis to fill the screen
    bool repeatY;                            // if true, will repear on X axis to fill the screen
    int alpha;

    file_v5_map_background()
    {
        filename[0] = '\0';
        adjust_y = 0;
        speed = 0;
        auto_scroll = 0;
        sprintf(filename, "%s", "");
        repeatX = true;
        repeatY = false;
        alpha = SDL_ALPHA_OPAQUE;
    }
};

struct file_v5_map_header {
    char map_name[V5_CHAR_NAME];
    int tiles_w;
    int tiles_h;
    // first 10 are background layers, last 5 are foreground layers
    file_v5_map_background backgrounds[BACKGROUND_LAYERS_MAX];
    st_color background_color;
    unsigned int gfx;                                  // indicates that the map have an gfx executed by drawlib (rain, snow, train, etc)
    char tileset_filename[FS_CHAR_FILENAME_SIZE];
    bool autoscroll;
    char music_filename[FS_CHAR_FILENAME_SIZE];

    file_v5_map_header() {
        tiles_w = 100;
        tiles_h = 20;
        sprintf(map_name, "%s", "");
        gfx = 0;
        sprintf(tileset_filename, "%s", "");
        autoscroll = false;
        sprintf(music_filename, "%s", "");
    }
};

struct file_v5_map_room {
    file_v5_map_tile tiles[MAP_ROOM_W][MAP_ROOM_H];
    int area_x = -1;
    int area_y = -1;
};

// this is used to save/load from disk (serialized)
struct file_v5_map_room_data {
    int area = -1;
    int map = -1;
    int area_x = -1;
    int area_y = -1;
    file_v5_map_room room;
};


struct file_v5_map_link { // DONE - Stage
    Sint8 id_map_origin;
    Sint8 id_map_destiny;
    st_rectangle pos_origin;
    st_position pos_destiny;
    e_LINK_DIRECTION link_direction;
    int type;
    file_v5_map_link() {
        id_map_origin = -1;
        id_map_destiny = -1;
        type = -1;
        link_direction = LINK_DIRECTION_HORIZONTAL;
    }
};

struct file_v5_map_npc {
    Sint8 id_npc;
    struct st_position start_point;
    int direction;

    file_v5_map_npc() {
        id_npc = -1;
        direction = 0;
    }
};

struct file_v5_map_teleporter_object {
    st_position link_dest;                          // used for teleporter
    Sint8 map_dest;
    file_v5_map_teleporter_object() {
        map_dest = -1;
        link_dest.x = -1;
        link_dest.y = -1;
    }

};

struct file_v5_map_object {
    Sint8 id_object;
    struct st_position start_point;
    Uint8 direction;
    file_v5_map_teleporter_object teleporter_data;

    file_v5_map_object() {
        id_object = -1;
        direction = 0;
    }
};

struct slope_data {
    int left = 0;
    int right = 0;
};

struct file_v5_slope_tile {
    char filename[FS_CHAR_FILENAME_SIZE];
    slope_data slope[SLOPE_MAX_TILES];

    file_v5_slope_tile() {
        filename[0] = '\0';
    }

};



#endif // STRUCT_FILE_MAP_H
