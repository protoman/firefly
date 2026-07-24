#ifndef FILE_OBJECT_V6_H
#define FILE_OBJECT_V6_H

#include "defines.h"
#include "data/st_common.h"
#include <cstdio>

#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

struct v6_file_object { // DONE - Game
    char name[CHAR_NAME_SIZE];
    char graphic_filename[FS_CHAR_NAME_SIZE];
    int type;                                                					// object type identifier
    int timer;                                                				// used as time for disapearing block
    int speed;                                                				// used as speed for moving platform
    int limit;                                                				// used as range (pixels) for moving platform, visible time for disapearing block
    int direction;                                            				// used to check if moving away from oiginalpoint or moving to it
    int distance;                                           // max distance object can reach/move
    st_size size;                                           // frame-size of the graphics
    int frame_duration;                                        			// indicates what is the used frame
    bool animation_auto_start;                              // false means it will not be animated until started
    bool animation_reverse;                                 // start animating from end to begin
    bool animation_loop;                                    // true = animation will loop, false = animation will run once
    int given_ability;                                      // when object is of type ability, will give this to player
    int key_id;                                             // the id of the object the unlocks this one (used for doors), 0 means none
    int sub_type;                                           // used to determine how the object will react when his key is used to "open" it

    v6_file_object() {
        sprintf(name, "%s", "Object");
        graphic_filename[0] = '\0';
        animation_auto_start = true;
        animation_reverse = false;
        animation_loop = true;
        frame_duration = 100;
        type = -1;
        timer = 0;
        speed = 2;
        limit = 0;
        animation_auto_start = true;
        animation_reverse = false;
        animation_loop = true;
        given_ability = -1;
        key_id = 0;
        sub_type = 0;
    }

    template <class Archive>
    void save(Archive & ar) const {
        std::string name_s(name);
        std::string graphic_s(graphic_filename);
        ar(CEREAL_NVP(name_s), CEREAL_NVP(graphic_s), CEREAL_NVP(type), CEREAL_NVP(timer), CEREAL_NVP(speed), CEREAL_NVP(limit), CEREAL_NVP(direction), CEREAL_NVP(distance), CEREAL_NVP(size), CEREAL_NVP(frame_duration), CEREAL_NVP(animation_auto_start), CEREAL_NVP(animation_reverse), CEREAL_NVP(animation_loop), CEREAL_NVP(given_ability), CEREAL_NVP(key_id), CEREAL_NVP(sub_type));
    }

    template <class Archive>
    void load(Archive & ar) {
        std::string name_s;
        std::string graphic_s;
        ar(name_s, graphic_s, type, timer, speed, limit, direction, distance, size, frame_duration, animation_auto_start, animation_reverse, animation_loop, given_ability, key_id, sub_type);
        strncpy(name, name_s.c_str(), CHAR_NAME_SIZE);
        name[CHAR_NAME_SIZE-1] = '\0';
        strncpy(graphic_filename, graphic_s.c_str(), FS_CHAR_NAME_SIZE);
        graphic_filename[FS_CHAR_NAME_SIZE-1] = '\0';
    }
};




struct v6_stage_object {
    int uuid = -1;
    int currentMap = -1;
    int id_object = -1;
    struct st_position start_point;
    unsigned int direction = ANIM_DIRECTION_RIGHT;
    st_position dest_position; // teleporter data, also used for objects that have sizes/orientation defined while placing them in editor //
    int dest_map = -1;
    int dest_level = -1;

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(uuid), CEREAL_NVP(currentMap), CEREAL_NVP(id_object), CEREAL_NVP(start_point), CEREAL_NVP(direction), CEREAL_NVP(dest_position), CEREAL_NVP(dest_map), CEREAL_NVP(dest_level));
    }
};


#endif // FILE_OBJECT_V6_H
