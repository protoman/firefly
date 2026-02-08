//
// Created by iuri on 07/02/2026.
//

#ifndef FIREFLYGAME_SHAREDPLAYERDATA_HPP
#define FIREFLYGAME_SHAREDPLAYERDATA_HPP
#include <map>

#include "st_common.h"


namespace player_data {
    class SharedPlayerData {
    public:
        static SharedPlayerData* get_instance();
        st_float_position getPosition();
        void setPosition(const st_float_position &position);
        void addPlayerCollision(int anim_type, st_size collision);
        st_size getPlayerCollision(int anim_type);

    private:
        SharedPlayerData();
        SharedPlayerData(SharedPlayerData const&)= default;                       // copy constructor is private
        SharedPlayerData& operator=(SharedPlayerData const&){ return *this; };    // assignment operator is private

    private:
        static SharedPlayerData* instance;
        st_float_position position;
        std::map<int, st_size> collision_map;

    };
}

#endif //FIREFLYGAME_SHAREDPLAYERDATA_HPP