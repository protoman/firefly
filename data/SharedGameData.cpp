//
// Created by iuri on 01/02/2026.
//


#include "SharedGameData.hpp"

namespace game_data {
    SharedGameData* SharedGameData::instance = nullptr; // Definition/Allocation

    SharedGameData * SharedGameData::get_instance() {
        if (!instance) {
            instance = new game_data::SharedGameData();
        }
        return instance;
    }

    SharedGameData::SharedGameData() = default;
}
