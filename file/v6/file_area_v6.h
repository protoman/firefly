#ifndef FILE_MAP_H
#define FILE_MAP_H

#include "defines.h"
#include "data/st_common.h"
#include "file_room_v6.h"
#include "file/v6/file_style_v6.h"


#define BG_LAYER_MAX 5
#define FG_LAYER_MAX 5




// MAPA é um grupo de salas com valores como fundo, música e tileset em comum //
struct file_v6_stage {
    char stage_name[V5_CHAR_NAME];
    int style;
    char music_filename[FS_CHAR_FILENAME_SIZE];
    file_v6_room rooms[AREA_ROOM_MAX_NUMBER];

    file_v6_stage() {
        sprintf(stage_name, "%s", "");
        sprintf(music_filename, "%s", "");
    }
};

struct file_v6_area {
    int style;
    char area_name[V5_CHAR_NAME];

    file_v6_area() {
        style = 0;
        sprintf(area_name, "%s", "New Area");
    }
};


#endif // FILE_MAP_H
