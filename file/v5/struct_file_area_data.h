#ifndef STRUCT_FILE_AREA_DATA_H
#define STRUCT_FILE_AREA_DATA_H

#include "file/v5/struct_file_area.h"
#include "file/v5/struct_file_area_link.h"
#include "file/v5/struct_file_map.h"

// groups of screens with common data such as background, tileset and bg-music //
struct file_v5_level_groupmap_header {
    file_v5_map_background backgrounds[BACKGROUND_LAYERS_MAX];  // first 10 are background layers, last 5 are foreground layers
    st_color background_color;
    unsigned int gfx;                                           // indicates that the map have an gfx executed by drawlib (rain, snow, train, etc)
    char tileset_filename[FS_CHAR_FILENAME_SIZE];
    bool autoscroll;
    char music_filename[FS_CHAR_FILENAME_SIZE];

    file_v5_level_groupmap_header() {
        gfx = 0;
        autoscroll = false;
        sprintf(tileset_filename, "%s", "");
        sprintf(music_filename, "%s", "");
    }
};

// stores the data for one "screen" of tiles
struct file_v5_level_screen_data {
    int groupmap_id = -1;
    bool visited = false;
    bool walls[WALL_DIRECTION_COUNT];
    file_v5_map_tile tiles[AREA_ROOM_W][AREA_ROOM_H];

    file_v5_level_screen_data() {
        std::cout << ">>>>>>>>>>> file_v5_level_screen_data" << std::endl;
        for (int i=0; i<WALL_DIRECTION_COUNT; i++) {
            walls[i] = false;
        }
    }

    void reset() {
        std::cout << ">>>>>>>>>>> file_v5_level_screen_data::reset" << std::endl;
        groupmap_id = -1;
        visited = false;
        for (int i=0; i<WALL_DIRECTION_COUNT; i++) {
            walls[i] = false;
        }
        for (int n=0; n<AREA_ROOM_W; n++) {
            for (int m=0; m<AREA_ROOM_H; m++) {
                tiles[n][m].locked = 0;
                tiles[n][m].tile_overlay.x = -1;
                tiles[n][m].tile_overlay.y = -1;
                tiles[n][m].tile_overlay.type = TILE_TYPE_SOLID;
                tiles[n][m].tile_underlay.x = -1;
                tiles[n][m].tile_underlay.y = -1;
                tiles[n][m].tile_underlay.type = TILE_TYPE_SOLID;

            }
        }

    }
};



// agregação dos dados de uma área, não é salvo diretamente no disco, mas em múltiplos arquivos //
struct struct_file_level_header {
    char name[V5_CHAR_NAME];

    // TODO: tem que separar esses vetores em arquivos próprios //
    std::vector<file_v5_level_groupmap_header> header_vector;
    std::vector<struct_file_v5_area_link> link_vector;

    struct_file_level_header() {
        sprintf(name, "%s", "");
    }
};




#endif // STRUCT_FILE_AREA_DATA_H
