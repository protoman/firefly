#ifndef FILE_GAME_OBJECT_STATE_H
#define FILE_GAME_OBJECT_STATE_H

struct v6_file_game_object_state {
    int uuid = -1;
    int obj_id = -1;
    int area_n = -1;
    bool finished = false;
    bool picked = false;
    int x = -1;
    int y = -1;
};

#endif // FILE_GAME_OBJECT_STATE_H
