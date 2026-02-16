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
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/vector.hpp"


namespace data {
    struct file_enemy {
        int id;
        std::string name;
        int hp;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(id),
                CEREAL_NVP(name),
                CEREAL_NVP(hp));
        }
    };

    struct file_enemies {
        std::vector<file_enemy> enemy_list;

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(enemy_list));
        }
    };


    inline void saveEnemies(file_enemies enemies) {
        //std::stringstream ss; // any stream can be used
        //cereal::JSONOutputArchive oarchive(std::cout);
        //cereal::JSONOutputArchive oarchive(ss);
        std::ofstream os("data_enemies.json");
        cereal::JSONOutputArchive oarchive(os);
        oarchive(enemies.enemy_list);
    }

    inline file_enemies loadEnemies() {
        std::string filename = "data_enemies.json";
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
