#ifndef FILE_GAME_OBJECT_STATE_H
#define FILE_GAME_OBJECT_STATE_H

#include "cereal/cereal.hpp"

struct v6_file_game_object_state {
    int uuid = -1;
    int obj_id = -1;
    int area_n = -1;
    bool finished = false;
    bool picked = false;
    int x = -1;
    int y = -1;

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(uuid), CEREAL_NVP(obj_id), CEREAL_NVP(area_n), CEREAL_NVP(finished), CEREAL_NVP(picked), CEREAL_NVP(x), CEREAL_NVP(y));
    }
};

#endif // FILE_GAME_OBJECT_STATE_H
