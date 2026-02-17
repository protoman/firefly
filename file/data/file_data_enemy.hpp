//
// Created by iuri on 14/02/2026.
//

#ifndef FIREFLYGAME_FILE_DATA_ENEMY_HPP
#define FIREFLYGAME_FILE_DATA_ENEMY_HPP

#include <string>
#include <fstream>
#include <vector>
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"



namespace data {

    enum frame_types {
        frame_type_stand,
        frame_type_walk,
        frame_type_walk_attack,
        frame_type_walk_on_air,
        frame_type_flying,
        frame_type_jump,
        frame_type_jump_attack,
        frame_type_attack1,
        frame_type_attack2,
        frame_type_attack3,
        frame_type_attack4,
        frame_type_attack5,
        frame_type_attack6,
        frame_type_attack7,
        frame_type_attack8,
        frame_type_teleport,
        frame_type_hit,
        frame_type_ladder,
        frame_type_ladder_semi,
        frame_type_ladder_attack,
        frame_type_ladder_move,
        frame_type_throw,
        frame_type_turning,
        frame_type_presenting,
        frame_type_sliding,
        frame_type_dashing,
        frame_type_shielded,
        frame_type_looking_up,
        frame_type_looking_down,
        frame_type_wall_grabbing,
        frame_type_damaged,
        frame_type_celebrating,
    };

    enum enemy_shield_modes {
        enemy_shield_mode_none,
        enemy_shield_mode_full,
        enemy_shield_mode_front,
        enemy_shield_mode_stand,
        enemy_shield_mode_disguise,
        enemy_shield_mode_stand_front,
        enemy_shield_mode_stand_and_walk,
        enemy_shield_mode_back,
        enemy_shield_mode_using_shield
    };

    struct file_enemy_size {
        int w;
        int h;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(w), CEREAL_NVP(h));
        }
    };

    struct file_enemy_point {
        int x;
        int y;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(x), CEREAL_NVP(y));
        }

    };

    struct file_enemy_frame {
        bool is_key_frame;
        long duration;
        int origin;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(is_key_frame), CEREAL_NVP(duration), CEREAL_NVP(origin));
        }

    };

    struct file_enemy_frameset {
        frame_types type;
        std::vector<file_enemy_frame> frames;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(type), CEREAL_NVP(frames));
        }
    };

    struct file_enemy {
        int id;
        std::string name;
        int hp;

        enemy_shield_modes shield_mode;
        float speed;
        float jump_speed;
        int range;
        std::string graphic_filename;
        file_enemy_size sprite_size;
        file_enemy_point projectile_origin_point;
        std::vector<file_enemy_frameset> framesets;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(id),
                CEREAL_NVP(name),
                CEREAL_NVP(hp),
                CEREAL_NVP(shield_mode),
                CEREAL_NVP(speed),
                CEREAL_NVP(jump_speed),
                CEREAL_NVP(range),
                CEREAL_NVP(graphic_filename),
                CEREAL_NVP(sprite_size),
                CEREAL_NVP(projectile_origin_point),
                CEREAL_NVP(framesets));
        }
    };

    struct file_enemies {
        std::vector<file_enemy> enemy_list;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(enemy_list));
        }
    };


    inline void saveEnemies(std::string base_directory, file_enemies enemies) {
        //std::stringstream ss; // any stream can be used
        //cereal::JSONOutputArchive oarchive(std::cout);
        //cereal::JSONOutputArchive oarchive(ss);
        std::ofstream os(base_directory + "/data/enemies/data_enemies.json");
        cereal::JSONOutputArchive oarchive(os);
        oarchive(enemies.enemy_list);
    }

    inline file_enemies loadEnemies(std::string base_directory) {
        std::string filename = base_directory + "/data/enemies/data_enemies.json";
        std::vector<file_enemy> enemies;
        std::ifstream is(filename);
        file_enemies result;

        if (is.is_open()) {
            cereal::JSONInputArchive iarchive(is); // Create an input archive
            iarchive(enemies); // Read the data from the archive
            result.enemy_list = enemies;
        }

        return result;
    }
}

#endif //FIREFLYGAME_FILE_DATA_ENEMY_HPP
