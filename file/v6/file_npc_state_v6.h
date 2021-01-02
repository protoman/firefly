#ifndef FILE_NPC_STATE_V6_H
#define FILE_NPC_STATE_V6_H

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
};


#endif // FILE_NPC_STATE_V6_H
