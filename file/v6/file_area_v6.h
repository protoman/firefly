#ifndef FILE_MAP_H
#define FILE_MAP_H

#include "defines.h"
#include "data/st_common.h"
#include "file_room_v6.h"
#include "file/v6/file_style_v6.h"


#define BG_LAYER_MAX 5
#define FG_LAYER_MAX 5




// MAPA é um grupo de salas com valores como fundo, música e tileset em comum //
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

struct file_v6_stage {
    char stage_name[V5_CHAR_NAME];
    int style;
    char music_filename[FS_CHAR_FILENAME_SIZE];
    file_v6_room rooms[AREA_ROOM_MAX_NUMBER];

    file_v6_stage() {
        sprintf(stage_name, "%s", "");
        sprintf(music_filename, "%s", "");
    }

    template <class Archive>
    void save(Archive & ar) const {
        std::string stage_name_s(stage_name);
        std::string music_s(music_filename);
        std::vector<file_v6_room> rooms_vec;
        rooms_vec.reserve(AREA_ROOM_MAX_NUMBER);
        for (int i=0;i<AREA_ROOM_MAX_NUMBER;i++) rooms_vec.push_back(rooms[i]);
        ar(CEREAL_NVP(stage_name_s), CEREAL_NVP(style), CEREAL_NVP(music_s), CEREAL_NVP(rooms_vec));
    }

    template <class Archive>
    void load(Archive & ar) {
        std::string stage_name_s;
        std::string music_s;
        std::vector<file_v6_room> rooms_vec;
        ar(stage_name_s, style, music_s, rooms_vec);
        strncpy(stage_name, stage_name_s.c_str(), V5_CHAR_NAME);
        stage_name[V5_CHAR_NAME-1]='\0';
        strncpy(music_filename, music_s.c_str(), FS_CHAR_FILENAME_SIZE);
        music_filename[FS_CHAR_FILENAME_SIZE-1]='\0';
        for (int i=0;i<AREA_ROOM_MAX_NUMBER;i++) {
            if (i < (int)rooms_vec.size()) rooms[i] = rooms_vec[i];
            else rooms[i] = file_v6_room();
        }
    }
};

struct file_v6_area {
    int style;
    char area_name[V5_CHAR_NAME];

    file_v6_area() {
        style = 0;
        sprintf(area_name, "%s", "New Area");
    }

    template <class Archive>
    void serialize(Archive & ar) {
        std::string area_name_s(area_name);
        ar(CEREAL_NVP(style), CEREAL_NVP(area_name_s));
        strncpy(area_name, area_name_s.c_str(), V5_CHAR_NAME);
        area_name[V5_CHAR_NAME-1]='\0';
    }
};


#endif // FILE_MAP_H
