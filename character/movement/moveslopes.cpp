#include "moveslopes.h"
#include "GameManager.h"

#define ADJUST_Y 3


moveSlopes::moveSlopes()
{

}

int moveSlopes::checkSlope(int xinc, int yinc, st_rectangle hitbox)
{
    slope_tile_info tile_info = searchSlopeTile(hitbox, xinc, yinc);
    if (tile_info.tile.tile_underlay.type == TILE_TYPE_SLOPE) {
        if (xinc != 0 || yinc > 0) {
            int calc_bottom = calcExpectedVericalPosForSlope(xinc, yinc, hitbox, tile_info);
            if (calc_bottom != -1) {
                is_on_slope = true;
                int calc_y = calc_bottom - hitbox.h;
                //int res = hitbox.y - calc_y;
                //std::cout << "MOVESLOPES::checkSlope - calc_y[" << calc_y << "], hitbox.y[" << hitbox.y << "]" << std::endl;
                return calc_y;
            }
        }
    } else {
        if (is_on_slope == true) {
            is_on_slope = false;
            //std::cout << ">>>>>>> CHAR::searchSlopeTile - left slopes, x-center[" << (hitbox.x + xinc) + hitbox.w/2 << "], y[" << hitbox.y + hitbox.h << "], tyle-x[" << ((hitbox.x + xinc) + hitbox.w/2)/TILESIZE <<"], tile-y[" << (hitbox.y + hitbox.h)/TILESIZE << "]" << std::endl;
            return hitbox.y - ADJUST_Y; // ensure the player is set on the right y-pos to he won't fall nor get inside block
        }
        is_on_slope = false;
    }
    return 0;
}


slope_tile_info moveSlopes::searchSlopeTile(st_rectangle hitbox, int xinc, int yinc)
{
    int check_steps = 4;
    int check_step_diff = 2;
    for (int adjust_x=-check_steps; adjust_x<check_steps; adjust_x+=check_step_diff) {
        for (int adjust_y =- check_steps; adjust_y<check_steps; adjust_y+=check_step_diff) {
            for (int i=yinc; i>=0; i--) {
                int check_x = (hitbox.x + xinc) + hitbox.w/2 + adjust_x;
                int check_y = (hitbox.y + i) + hitbox.h + adjust_y;
                int tile_x = check_x/TILESIZE;
                int tile_y = check_y/TILESIZE;

                //std::cout << ">>> #1 - moveslopes::checkSlope - bottom[" << check_y << "], check[" << check_x << "][" << check_y << "], hitbox[" << hitbox.x  << "][" << hitbox.y  << "], inc[" << xinc << "][" << yinc << "], hitbox[" << hitbox.x << "][" << hitbox.y << "][" << hitbox.w << "][" << hitbox.h << "], tile[" << tile_x << "][" << tile_y << "]" << std::endl;

                // TODO: put tested points into a (unique) list, so we don't test twice the same one

                file_v6_room_tile tile_center = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(tile_x, tile_y);
                if (tile_center.tile_underlay.type == TILE_TYPE_SLOPE) {
                    //int expectedVerticalPos = calcExpectedVericalPosForSlope(check_x, check_y, tile_x, tile_y);

                    //std::cout << ">>> #1 - MOVESLOPES::checkSlope::TRUE - bottom[" << check_y << "], adjust_y[" << adjust_y <<"], check[" << check_x << "][" << check_y << "], tile_y[" << tile_y << "], hitbox[" << hitbox.x  << "][" << hitbox.y  << "], inc[" << xinc << "][" << yinc << "], hitbox[" << hitbox.x << "][" << hitbox.y << "][" << hitbox.w << "][" << hitbox.h << "], tile[" << tile_x << "][" << tile_y << "]" << std::endl;
                    slope_tile_info res;
                    res.x = tile_x;
                    res.y = tile_y;
                    res.tile = tile_center;
                    return res;
                }
            }

        }
    }
    slope_tile_info empty;
    return empty;
}

int moveSlopes::calcExpectedVericalPosForSlope(int xinc, int yinc, st_rectangle hitbox, slope_tile_info tile_info)
{

    if (tile_info.tile.tile_underlay.x == -1 || tile_info.tile.tile_underlay.y == -1) {
        std::cout << "character::adjust_slope_y - LEAVE #1" << std::endl;
        return -1;
    }
    if (tile_info.tile.tile_underlay.type != TILE_TYPE_SLOPE) {
        std::cout << "character::adjust_slope_y - LEAVE #2" << std::endl;
        return -1;
    }

    slope_data data = GameData::get_instance()->slope_list.at(tile_info.tile.tile_underlay.x).slope[tile_info.tile.tile_underlay.y];

    int left = TILESIZE - data.left;
    int right = TILESIZE - data.right;

    bool is_left_smaller = true;
    int smaller_y = left;
    int bigger_y = right;
    if (right < left) {
        smaller_y = right;
        bigger_y = left;
        is_left_smaller = false;
    }
    double diff_h = (double)(right-left);
    current_slope_step = diff_h/TILESIZE; // represents how much changes for x for each x pixel in the slope
    int center_x = hitbox.x + xinc + hitbox.w/2;

    //std::cout << "MOVESLOPES::calcExpectedVericalPosForSlope - tileY[" << tile_info.y << "], hitbox.y[" << hitbox.y << "], hitbox.h[" << hitbox.h << "], yinc[" << yinc << "]" << std::endl;

    int diff_x = center_x - tile_info.x*TILESIZE;
    int y_adjust = smaller_y + diff_x*current_slope_step;
    int pos_y = (tile_info.y+1)*TILESIZE;

    int calc_y = pos_y - y_adjust - ADJUST_Y;
    if (!is_left_smaller) {
        calc_y -= bigger_y;
    }
    //std::cout << "MOVESLOPES::calcExpectedVericalPosForSlope - tileY[" << tile_info.y << "], y_adjust[" << y_adjust << "], calc_y[" << calc_y << "]" << std::endl;

    return calc_y;

    /*
    double diff_h = (double)(right-left);
    current_slope_step = diff_h/TILESIZE; // represents how much changes for x for each x pixel in the slope
    int tileX = (hitbox.x + xinc) / TILESIZE;
    int tileY = (hitbox.y + hitbox.h + yinc + ADJUST_Y) / TILESIZE;
    int x = hitbox.x + xinc;
    int y = tileY*TILESIZE;
    int xDiff = abs(tileX*TILESIZE - x);

    int diff_x_calc = tileX*TILESIZE - x;
    std::cout << "MOVESLOPES::calcExpectedVericalPosForSlope - diff_h[" << diff_h << "], current_slope_step[" << current_slope_step << "], diff_x_calc[" << diff_x_calc << "], hitbox.x[" << hitbox.x << "], x[" << x << "]" << std::endl;


    int calc_y = y - (current_slope_step * xDiff);
    //std::cout << "MOVESLOPES::calcExpectedVericalPosForSlope - y[" << y << "], calc_y[" << calc_y << "], current_slope_step[" << current_slope_step << "], xDiff[" << xDiff << "]" << std::endl;
    //std::cout << "MOVESLOPES::calcExpectedVericalPosForSlope - tileY[" << tileY << "], calc_y[" << calc_y << "], current_y[" << (hitbox.y+hitbox.h) << "], next_y[" << (hitbox.y+hitbox.h+yinc) << "], yinc[" << yinc << "]" << std::endl;
    return calc_y;
    */
}

/*
bool moveSlopes::isOutOfSlopes(int xinc, st_rectangle hitbox)
{
    int test_point_x = (hitbox.x + xinc + hitbox.w/2);
    int test_point_y = (hitbox.y + hitbox.h - 1);

    // check all diagonals from the player feet
    std::vector<st_position> point_list;
    int dist_x = 1;
    int dist_y = 4;
    point_list.push_back(st_position(test_point_x-dist_x, test_point_y+dist_y)); // left down
    point_list.push_back(st_position(test_point_x-dist_x, test_point_y-dist_y)); // left up
    point_list.push_back(st_position(test_point_x+dist_x, test_point_y+dist_y)); // right down
    point_list.push_back(st_position(test_point_x+dist_x, test_point_y-dist_y)); // right up
    point_list.push_back(st_position(test_point_x+dist_x, test_point_y)); // right
    point_list.push_back(st_position(test_point_x+dist_x, test_point_y)); // left
    point_list.push_back(st_position(test_point_x, test_point_y+dist_y)); // down
    point_list.push_back(st_position(test_point_x, test_point_y-dist_y)); // up

    for (unsigned int i=0; i<point_list.size(); i++) {
        file_v6_room_tile tile_test = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(point_list.at(i).x/TILESIZE, point_list.at(i).y/TILESIZE);
        //std::cout << "CHAR::isOutOfSlopes - type[" << tile_test.tile_underlay.type << "], test.x[" << point_list.at(i).x << "], test.y[" << point_list.at(i).y << "]" << std::endl;
        if (tile_test.tile_underlay.type == TILE_TYPE_SLOPE) {
            return false;
        }
    }

    // checks player ground for any slopes around
    std::cout << "CHAR::isOutOfSlopes[TRUE]" << std::endl;
    return true;
}

int moveSlopes::checkSlope(int xinc, int yinc, st_rectangle hitbox)
{
    int check_steps = 4;
    int check_step_diff = 2;
    for (int adjust_x=-check_steps; adjust_x<check_steps; adjust_x+=check_step_diff) {
        for (int adjust_y=-check_steps; adjust_y<check_steps; adjust_y+=check_step_diff) {
            int check_x = (hitbox.x + xinc) + hitbox.w/2 + adjust_x;
            int check_y = (hitbox.y + yinc) + hitbox.h + adjust_y;
            int tile_x = check_x/TILESIZE;
            int tile_y = check_y/TILESIZE;
            // TODO: put tested points into a (unique) list, so we don't test twice the same one

            file_v6_room_tile tile_center = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(tile_x, tile_y);
            if (tile_center.tile_underlay.type == TILE_TYPE_SLOPE) {
                int expectedVerticalPos = calcExpectedVericalPosForSlope(check_x, check_y, tile_x, tile_y);
                //std::cout << ">>> #1 - CHAR::checkSlope::TRUE - bottom[" << check_y << "], expected[" << expectedVerticalPos << "], check[" << check_x << "][" << check_y << "], hitbox[" << hitbox.x  << "][" << hitbox.y  << "], inc[" << xinc << "][" << yinc << "], hitbox[" << hitbox.x << "][" << hitbox.y << "][" << hitbox.w << "][" << hitbox.h << "], tile[" << tile_x << "][" << tile_y << "]" << std::endl;
                return expectedVerticalPos;
            }

        }
    }
    return 0;
}


bool moveSlopes::isOnSlope()
{
    return is_on_slope;
}

void moveSlopes::calc_slope_diff_h(st_position map_pos)
{
    file_v6_room_tile tile = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(map_pos.x, map_pos.y);

    if (tile.tile_underlay.x == -1 || tile.tile_underlay.y == -1) {
        std::cout << "character::adjust_slope_y - LEAVE #1" << std::endl;
        return;
    }
    if (tile.tile_underlay.type != TILE_TYPE_SLOPE) {
        std::cout << "character::adjust_slope_y - LEAVE #2" << std::endl;
        return;
    }
    slope_data data = SharedData::get_instance()->slope_list.at(tile.tile_underlay.x).slope[tile.tile_underlay.y];


    int left = TILESIZE - data.left;
    int right = TILESIZE - data.right;

    double diff_h = (double)(right-left);
    current_slope_step = diff_h/TILESIZE; // represents how much changes for x for each x pixel in the slope
}

int moveSlopes::adjust_slope_y(int xinc, int incy, st_position map_pos, st_rectangle hitbox)
{
    file_v6_room_tile tile = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(map_pos.x, map_pos.y);

    if (tile.tile_underlay.x == -1 || tile.tile_underlay.y == -1) {
        std::cout << "character::adjust_slope_y - LEAVE #1" << std::endl;
        return BLOCK_UNBLOCKED;
    }
    if (tile.tile_underlay.type != TILE_TYPE_SLOPE) {
        std::cout << "character::adjust_slope_y - LEAVE #2" << std::endl;
        return BLOCK_UNBLOCKED;
    }
    slope_data data = SharedData::get_instance()->slope_list.at(tile.tile_underlay.x).slope[tile.tile_underlay.y];


    int left = TILESIZE - data.left;
    int right = TILESIZE - data.right;

    int diff_x = position.x+frameSize.width/2 - map_pos.x*TILESIZE;
    if (diff_x >= TILESIZE) {
        diff_x = TILESIZE-1;
    }
    double diff_h = (double)(right-left);


    double step_x_diff = abs(diff_h/TILESIZE);
    double calc_diff_y = abs(diff_x*step_x_diff);
    int plus_y = std::min(left, right);
    if (left > right) {
        calc_diff_y = abs((TILESIZE-diff_x)*step_x_diff);
    }

    int new_y = (map_pos.y+1)*TILESIZE - frameSize.height - calc_diff_y - plus_y + 1;


    // novo cálculo - test //
    if (state.animation_type == ANIM_TYPE_JUMP && position.y < new_y) {
        std::cout << "character::adjust_slope_y - LEAVE #3" << std::endl;
        return BLOCK_UNBLOCKED;
    }

    std::cout << "CHAR::ADJUST_SLOPE_Y - current_y[" << position.y << "], new_y[" << new_y << "], map_pos.y[" << map_pos.y << "], frameSize.height[" << frameSize.height << "], calc_diff_y[" << calc_diff_y << "], plus_y[" << plus_y << "]" << std::endl;

    position.y =  new_y;
    if (state.animation_type == ANIM_TYPE_JUMP) {
        set_animation_type(ANIM_TYPE_STAND);
    }
    return BLOCK_Y;
}
*/
