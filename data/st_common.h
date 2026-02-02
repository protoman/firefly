#ifndef ST_COMMON_H
#define ST_COMMON_H

// some small structures that do not need a separated file for each one
#include <vector>
#include <iostream>
#include <stdio.h>

#include "defines.h"
#include "data/st_imageData.hpp"
#include "cereal/cereal.hpp"
#include <SDL3/SDL.h>

extern SDL_Renderer* gRenderer;

/**
 * @brief
 *
 */
struct st_position {
    int x;
    int y;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y));
    }

    st_position() {
        x = 0;
        y = 0;
    }

    st_position (int setX, int setY) {
        x = setX;
        y = setY;
    }
    st_position& operator=(const st_position &set_pt) {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    bool operator==(const st_position &comp_pt) const {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_position &comp_pt) const {
        if (x != comp_pt.x || y != comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator<(const st_position &comp_pt) const {
        if (y == comp_pt.y && x < comp_pt.x) {
            return true;
        }
        if (y < comp_pt.y) {
            return true;
        }
        return false;
    }
};



/**
 * @brief
 *
 */
struct st_float_position {
    float x;
    float y;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y));
    }

    st_float_position() {
        x = 0.0;
        y = 0.0;
    }

    st_float_position (float setX, float setY) {
        x = setX;
        y = setY;
    }

    st_float_position& operator=(const st_position &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }

    bool operator==(const st_position &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }

    st_float_position& operator=(const st_float_position &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }

    bool operator==(const st_float_position &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
};

struct st_size {
    int width;
    int height;

	st_size()
	{
            width = 0;
            height = 0;
	}

    st_size(int w, int h)
	{
		width = w;
        height = h;
	}

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(width), CEREAL_NVP(height));
    }
};


struct st_rectangle {
    float x;
    float y;
    float w;
    float h;
	st_rectangle () {
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}

    st_rectangle (float setX, float setY, float setW, float setH) {
		x = setX;
		y = setY;
		w = setW;
		h = setH;
	}

    bool is_empty() {
        if (x == 0 && y == 0 && w == 0 && h == 0) {
            return true;
        }
        return false;
    }
    bool operator==(const st_rectangle &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y && w == comp_pt.w && h == comp_pt.h) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_rectangle &comp_pt) const
    {
        if (x != comp_pt.x || y != comp_pt.y || (w != comp_pt.w && h != comp_pt.h)) {
            return true;
        }
        return false;
    }

    bool contains(int x_test, int y_test) {
        if (x_test >= x && x_test < x+w && y_test >= y && y_test < y+h) {
            return true;
        }
        return false;
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(w), CEREAL_NVP(h));
    }


};





/**
 * @brief
 *
 */
struct used_teleporter {
    int teleporter_n; // number of the map_link (this is hardcoded, must be rebuilt in 0.2)
    bool finished; // indicates if already used
    struct st_position old_player_pos;
    bool active; // if true, then after beating the boss, the player must be teleported back to the teleporter origin
    st_float_position old_map_scroll;
    int old_map_n;
    bool is_object;
	used_teleporter() {
        teleporter_n = -1;
		finished = false;
		old_player_pos.x = 0;
		old_player_pos.y = 0;
		active = false;
		old_map_scroll.x = 0;
		old_map_scroll.y = 0;
		old_map_n = 0;
        is_object = false;
	}
};




struct st_position_int8 {
    int x;
    int y;
    st_position_int8() {
        x = 0;
        y = 0;
    }
    st_position_int8 (int setX, int setY) {
        x = setX;
        y = setY;
    }
    st_position_int8& operator=(const st_position_int8 &set_pt)
    {
        x = set_pt.x;
        y = set_pt.y;
        return *this;
    }
    bool operator==(const st_position_int8 &comp_pt) const
    {
        if (x == comp_pt.x && y == comp_pt.y) {
            return true;
        }
        return false;
    }
    bool operator!=(const st_position_int8 &comp_pt) const
    {
        if (x != comp_pt.x || y != comp_pt.y) {
            return true;
        }
        return false;
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y));
    }

};


struct st_map_collision {
    int block;
    int terrain_type;
    st_map_collision() {
        block = 0;
        terrain_type = 0;
    }
    st_map_collision(int set_block, int set_terrain_type) {
        block = set_block;
        terrain_type = set_terrain_type;
    }
};


struct st_menu_option {
    std::string text;
    bool disabled;

    st_menu_option(std::string line) {
        text = line;
        disabled = false;
    }

    st_menu_option(std::string line, bool set_disabled) {
        text = line;
        disabled = set_disabled;
    }
};

struct st_input_button_config {
    int type;                                           // use joystick_input_types
    int value;                                          // stores button-n or hat-n or witch axis (0 horizontal1, 1 vertical 1, 2 hoz 2...)
    int axis_type;                                      // only for axis, tell is positive (+1) or negative (-1) movement
    st_input_button_config() {
        type = JOYSTICK_INPUT_TYPE_BUTTON;
        value = -1;
        axis_type = 0;
    }
};




struct st_surface_with_direction {
    st_imageData surface[2];
};


struct st_dialog {
    unsigned long timer = 0;                // if greater than zero, instead of waiting keypress, dialog will wait this time to vanish
    std::string music_filename = "";        // plays a tune when dialog shows up
    std::string face_name = "";             // if provided, shows a character portrair in dialog window
    bool show_close_button = true;          // show a button on dialog window to close itself
    std::vector<std::string> msgs;          // text in the dialog
};


struct st_light_point {
    int x = 0;
    int y = 0;
    e_LIGHT_POINT_COLOR color;

    st_light_point(int set_x, int set_y, e_LIGHT_POINT_COLOR set_color) {
        x = set_x;
        y = set_y;
        color = set_color;
    }
};

struct st_dialog_status {

    bool started = false;
    unsigned long timer = 0;

    st_dialog_status() {
        reset();
    }

    void reset() {
        started = false;
        timer = 0;
    }
};


#endif // ST_COMMON_H
