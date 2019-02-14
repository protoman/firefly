#ifndef STRUCT_FILE_AREA_H
#define STRUCT_FILE_AREA_H

#include "defines.h"
#include "data/st_common.h"

struct struct_file_v5_area {
    char name[V5_CHAR_NAME];
    int point[GAME_AREA_SIZE][GAME_AREA_SIZE];
    int map[GAME_AREA_SIZE];
    char music_filename[FS_CHAR_FILENAME_SIZE];
    st_color color;

    struct_file_v5_area() {
        name[0] = '\0';
        for (int i=0; i<GAME_AREA_SIZE; i++) {
            map[i] = -1;
            for (int j=0; j<GAME_AREA_SIZE; j++) {
                point[i][j] = -1;
            }
        }
    }
};


#endif // STRUCT_FILE_AREA_H
