#ifndef FILE_ANIM_BLOCK_H
#define FILE_ANIM_BLOCK_H

#include "defines.h"

struct file_map_anim_block {
    int x;
    int y;
    file_map_anim_block() {
        x = 0;
        y = 0;
    }
    file_map_anim_block(int set_x, int set_y) {
        x = set_x;
        y = set_y;
    }
};

struct file_anim_block {
    char filename[FS_CHAR_FILENAME_SIZE];
    int frame_delay[FS_ANIM_TILE_MAX_FRAMES];
    file_anim_block() {
        filename[0] = '\0';
        for (int i=0; i<FS_ANIM_TILE_MAX_FRAMES; i++) {
            frame_delay[i] = 200;
        }
    }

    template <class Archive>
    void save(Archive & ar) const {
        std::string filename_s(filename);
        std::vector<int> frame_delay_v(frame_delay, frame_delay + FS_ANIM_TILE_MAX_FRAMES);
        ar(cereal::make_nvp("filename", filename_s), cereal::make_nvp("frame_delay", frame_delay_v));
    }
    template <class Archive>
    void load(Archive & ar) {
        std::string filename_s;
        std::vector<int> frame_delay_v;
        ar(cereal::make_nvp("filename", filename_s), cereal::make_nvp("frame_delay", frame_delay_v));
        strncpy(filename, filename_s.c_str(), FS_CHAR_FILENAME_SIZE); filename[FS_CHAR_FILENAME_SIZE-1]='\0';
        for (size_t i = 0; i < frame_delay_v.size() && i < (size_t)FS_ANIM_TILE_MAX_FRAMES; ++i) frame_delay[i] = frame_delay_v[i];
        for (size_t i = frame_delay_v.size(); i < (size_t)FS_ANIM_TILE_MAX_FRAMES; ++i) frame_delay[i] = 200;
    }
};

#endif // FILE_ANIM_BLOCK_H

