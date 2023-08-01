#ifndef MOVESLOPES_H
#define MOVESLOPES_H

#include "data/st_common.h"
#include "file/v6/file_room_v6.h"

enum SLOPE_STATE {
    SLOPE_STATE_LEFT_DOWN,
    SLOPE_STATE_LEFT_UP,
    SLOPE_STATE_RIGHT_DOWN,
    SLOPE_STATE_RIGHT_UP,
    SLOPE_STATE_NO_SLOPE,
    SLOPE_STATE_COUNT
};

struct slope_tile_info {
    file_v6_room_tile tile;
    int x;
    int y;
    slope_tile_info() {
        x = 0;
        y = 0;
    }
};


class moveSlopes
{
public:
    moveSlopes();

    int checkSlope(int xinc, int yinc, st_rectangle hitbox);
    slope_tile_info searchSlopeTile(st_rectangle hitbox, int xinc, int yinc);
    int calcExpectedVericalPosForSlope(int xinc, int yinc, st_rectangle hitbox, slope_tile_info tile_info);

    //bool isOutOfSlopes(int xinc, st_rectangle hitbox);
    //int checkSlope(int xinc, int yinc, st_rectangle hitbox);

    //bool isOnSlope();
    //void calc_slope_diff_h(st_position map_pos);
    //int adjust_slope_y(int xinc, int incy, st_position map_pos, st_rectangle hitbox);

private:

private:
    bool is_on_slope = false;
    float current_slope_step = 0.0;

};

#endif // MOVESLOPES_H
