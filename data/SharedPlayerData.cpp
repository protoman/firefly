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

    void SharedPlayerData::addPlayerCollision(int anim_type, st_size collision) {
        collision_map[anim_type] = collision;
    }

    st_size SharedPlayerData::getPlayerCollision(int anim_type) {
        if (!collision_map.contains(anim_type)) {
            if (collision_map.contains(ANIM_TYPE_WALK)) {
                return collision_map[ANIM_TYPE_WALK];
            } else if (collision_map.contains(ANIM_TYPE_STAND)) {
                return collision_map[ANIM_TYPE_STAND];
            } else {
                return {54, 160}; // default size for player
            }
        }
        return collision_map[anim_type];
    }

    SharedPlayerData::SharedPlayerData() = default;

}