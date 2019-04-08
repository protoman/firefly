#ifndef STRUCT_FILE_AREA_LINK_H
#define STRUCT_FILE_AREA_LINK_H

#include "defines.h"
#include "data/st_common.h"

struct struct_file_v5_area_link {
    int link_type;
    st_position p1;
    st_position p2;

    struct_file_v5_area_link(int type, st_position point1, st_position point2) {
        link_type = type;
        p1 = point1;
        p2 = point2;
    }

    struct_file_v5_area_link() {}
};

#endif // STRUCT_FILE_AREA_LINK_H
