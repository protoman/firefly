//
// Created by iuri on 07/02/2026.
//

#include "SharedPlayerData.hpp"

namespace player_data {
    SharedPlayerData* SharedPlayerData::instance = nullptr; // Definition/Allocation

    SharedPlayerData * SharedPlayerData::get_instance() {
        if (!instance) {
            instance = new player_data::SharedPlayerData();
        }
        return instance;
    }

    st_float_position SharedPlayerData::getPosition() {
        return position;
    }

    void SharedPlayerData::setPosition(const st_float_position &position) {
        this->position = position;
    }

    SharedPlayerData::SharedPlayerData() = default;

}