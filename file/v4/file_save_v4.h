#ifndef FILE_SAVE_301_H
#define FILE_SAVE_301_H

#include <sys/types.h>
#include <sys/stat.h>
#include "defines.h"

struct st_items {
    int energy_tanks;
    bool heart_pieces[HEART_PIECES];                        // one heart-piece for each stage
    bool item_upgrades[ITEM_UPGRADES];
    int item_slots[ITEM_SLOTS];
    int weapon_slots[WEAPON_SLOTS];

    st_items() {
        energy_tanks = 0;
        for (int i=0; i<HEART_PIECES; i++) {
            heart_pieces[i] = false;
        }
        for (int i=0; i<ITEM_UPGRADES; i++) {
            item_upgrades[i] = false;
        }
        for (int i=0; i<ITEM_SLOTS; i++) {
            item_slots[i] = -1;
        }
        for (int i=0; i<WEAPON_SLOTS; i++) {
            weapon_slots[i] = -1;
        }
    }
};



// this stores any "generic" items you could want to add to game, such as parts od beta to fix him (like in mmx zero)
// or anything we want to expand later, like shop items
struct st_collect_items {
    int id;
    int number;
    st_collect_items() {
        id = 0;
        number = 0;
    }
};


struct st_save_checkpoint {
    unsigned int area = 0;
    unsigned int map = 0;
    unsigned int x = 0;
    unsigned int y = 0;
};


struct game_item_slot {
    int obj_id = -1;
    int uuid = -1;
};

struct st_save {
    st_items items;
    int selected_player;
    int hp;
    int mp;
    st_save_checkpoint checkpoint;
    bool abilities[ABILITIES_SLOTS];
    bool keys[KEY_TYPE_COUNT];
    game_item_slot game_item_list[GAME_ITEM_SLOTS];


    st_save() {
        reset();
    }

    void reset() {
        selected_player = 0;
        hp = PLAYER_INITIAL_HP;
        mp = PLAYER_INITIAL_HP;
        for (int i=0; i<ABILITIES_SLOTS; i++) {
            abilities[i] = false;
        }
        for (int i=0; i<KEY_TYPE_COUNT; i++) {
            keys[i] = false;
        }
    }
};


#endif // FILE_SAVE_H
