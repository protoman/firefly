#ifndef FILE_LEVEL_H
#define FILE_LEVEL_H

#include "cereal/cereal.hpp"

struct file_v6_level_point {
    int x;
    int y;
    int stage_number = 0;
    int area_number = 0;
    int room_number = 0;

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(stage_number), CEREAL_NVP(area_number), CEREAL_NVP(room_number));
    }
};

#endif // FILE_LEVEL_H
