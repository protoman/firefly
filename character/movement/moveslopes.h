#ifndef MOVESLOPES_H
#define MOVESLOPES_H

#include "data/st_common.h"
#include "file/v6/file_room_v6.h"

// Slope angle above which minimal speed compensation is applied
#define SLOPE_ANGLE_THRESHOLD 45.0f
// Minimum speed factor when on very steep slopes (ensures movement is never blocked)
#define MINIMAL_SPEED_FACTOR 0.2f

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

    /**
     * @brief Calculate the slope angle in degrees based on height difference
     * @param height_diff The height difference between right and left of the slope
     * @return float The slope angle in degrees (0-90)
     */
    float calculate_slope_angle(float height_diff) const;

    /**
     * @brief Calculate minimal movement speed for a given slope angle
     * When on steep slopes (> 45 degrees), the movement speed is reduced proportionally
     * but never becomes zero, ensuring the player can always move left/right
     * @param slope_angle The slope angle in degrees
     * @param base_speed The normal movement speed on flat ground
     * @return float The minimal speed to use on the slope
     */
    float calculate_minimal_speed(float slope_angle, float base_speed) const;

    /**
     * @brief Get the current slope angle (from last checkSlope call)
     * @return float The last calculated slope angle in degrees
     */
    float get_current_slope_angle() const;

    //bool isOutOfSlopes(int xinc, st_rectangle hitbox);
    //int checkSlope(int xinc, int yinc, st_rectangle hitbox);

    //bool isOnSlope();
    //void calc_slope_diff_h(st_position map_pos);
    //int adjust_slope_y(int xinc, int incy, st_position map_pos, st_rectangle hitbox);

private:

private:
    bool is_on_slope = false;
    float current_slope_step = 0.0;
    float current_slope_angle = 0.0f;

};

#endif // MOVESLOPES_H
