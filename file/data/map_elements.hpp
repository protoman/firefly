//
// Created by iuri on 14/02/2026.
//

#ifndef FIREFLYGAME_MAP_ELEMENTS_HPP
#define FIREFLYGAME_MAP_ELEMENTS_HPP

#include <fstream>
#include <vector>
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/vector.hpp"

namespace data {
    struct map_element_pos {
        unsigned int id = 0;
        long tile_x = 0;
        long tile_y = 0;
        bool is_stage_boss = false;
        bool is_sub_boss = false;

        map_element_pos(int paramId, int paramTileX, int paramTileY, int paramIsStageBoss, bool paramIsSubBoss) {
            id = paramId;
            tile_x = paramTileX;
            tile_y = paramTileY;
            is_stage_boss = paramIsStageBoss;
            is_sub_boss = paramIsSubBoss;
        }

        template <class Archive>
        void serialize(Archive & ar) {
            ar(CEREAL_NVP(id), CEREAL_NVP(tile_x), CEREAL_NVP(tile_y), CEREAL_NVP(is_stage_boss), CEREAL_NVP(is_sub_boss));
        }
    };

    struct map_elements {
        std::vector<map_element_pos> enemies;
        std::vector<map_element_pos> npcs;
        std::vector<map_element_pos> objects;

        template <class Archive>
        void serialize( Archive & ar ) {
            ar( CEREAL_NVP(enemies), CEREAL_NVP(npcs), CEREAL_NVP(objects));
        }
    };

    struct map_basic {
        //map_element_pos um = map_element_pos(1, 1, 1, false, false);
        std::vector<int> lista;

        template <class Archive>
        void serialize( Archive & ar ) {
            ar( CEREAL_NVP(lista));
        }
    };

    void saveMapElements(map_elements elements) {
        cereal::JSONOutputArchive oarchive(std::cout);
        std::stringstream ss; // any stream can be used
        std::ofstream os("data_map_elements.json");
        //cereal::JSONOutputArchive oarchive(ss);
        oarchive(elements.enemies, elements.npcs, elements.objects);
    }

    map_elements loadMapElements() {
        std::stringstream ss; // any stream can be used
        cereal::JSONOutputArchive iarchive(ss); // Create an input archive
        std::vector<map_element_pos> enemies, npcs, objects;
        iarchive(enemies, npcs, objects); // Read the data from the archive

        map_elements result;
        result.enemies = enemies;
        result.npcs = npcs;
        result.objects = objects;

        return result;
    }
}

#endif //FIREFLYGAME_MAP_ELEMENTS_HPP