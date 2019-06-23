#ifndef FILE_LEVEL_H
#define FILE_LEVEL_H

#define FILE_AREA_W 30
#define FILE_AREA_H 20

#include "file_room_v6.h"

struct file_v6_level_visited {
    bool visited[FILE_AREA_W][FILE_AREA_H];
    // add here items and such //

    file_v6_level_visited() {
        for (int i=0; i<FILE_AREA_W; i++) {
            for (int j=0; j<FILE_AREA_H; j++) {
                visited[i][j] = false;
            }
        }
    }
};

struct file_v6_level {
    char level_name[255];
    file_v6_room rooms[FILE_AREA_W][FILE_AREA_H];
};

#endif // FILE_LEVEL_H
