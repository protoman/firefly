#ifndef FILE_NPC_STATE_V6_H
#define FILE_NPC_STATE_V6_H

#include "cereal/cereal.hpp"

struct file_npc_state {
    int npc_id = -1;
    short state = 0;

    file_npc_state() {
        npc_id = -1;
        state = 0;
    }

    file_npc_state(int set_npc_id, short set_state) {
        npc_id = set_npc_id;
        state = set_state;
    }

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(npc_id), CEREAL_NVP(state));
    }
};


#endif // FILE_NPC_STATE_V6_H
