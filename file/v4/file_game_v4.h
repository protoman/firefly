#ifndef V_4_H
#define V_4_H

#include <stdio.h>
#include <iostream>
#include "defines.h"
#include "data/st_common.h"

#include "file/v4/file_save_v4.h"
#include "file/v4/file_config_v4.h"
#include "file/v4/file_scene_v4.h"


// @NOTE: all defines related to filesystem must contain a "FS_" prefix


//#define FORCE_CONVERT 1


struct file_projectilev3 {
    char name[CHAR_NAME_SIZE];
    char graphic_filename[FS_CHAR_NAME_SIZE];
    st_size size;
    bool is_destructible;                                   // indicates if the projectile can be shot down, if true, uses HP
    Uint8 hp;                                               // how much danage needs to be taken to destruct the projectile
    Uint8 trajectory;                                       // RENAMED from projectile_type in 2.1
    Uint8 max_shots;                                        // number of maximum simultaneous projectiles, if 0, is infinite
    Uint8 speed;                                            // now many pixels are moved into a step
    Uint8 damage;                                           // how many hitpoints will be reduced from target
    // new in 3.0.1
    char sfx_filename[FS_CHAR_NAME_SIZE];                  // allows to use different projectile shot sound
    bool can_be_reflected;                                  // if false, like in a charged-shot, can't be reflected by shield
    Uint8 spawn_npc_id;                                     // will spawn NPCs (like bird egg in mm2)
    Uint8 spawn_npc_n;
    bool is_explosive;
    bool vanishes_on_hit;                                   // if false, projectile won't be removed once hitting an enemy
    // number of NPCs that will be spawned
    file_projectilev3() {
        sprintf(name, "%s", "Projectile");
        sprintf(graphic_filename, "%s", "projectile_normal.png");
        is_destructible = false;
        hp = 1;
        size.width = 6;
        size.height = 6;
        trajectory = TRAJECTORY_LINEAR;
        max_shots = 3;
        speed = PROJECTILE_DEFAULT_SPEED;
        damage = PROJECTILE_DEFAULT_DAMAGE;
        sfx_filename[0] = '\0';
        can_be_reflected = true;
        spawn_npc_id = -1;
        spawn_npc_n = 0;
        is_explosive = false;
        vanishes_on_hit = true;
    }
};




struct st_sprite_data {
    bool used;                                                  // if true, the sprite exists
    Uint16 duration;
    Uint8 sprite_graphic_pos_x;
    st_rectangle collision_rect;
    st_sprite_data() {
        used = false;
        duration = 0;
        sprite_graphic_pos_x = 0;
    }

    st_sprite_data & operator=(st_sprite_data new_value) {
        used = new_value.used;
        duration = new_value.duration;
        sprite_graphic_pos_x = new_value.sprite_graphic_pos_x;
        collision_rect = new_value.collision_rect;
        return *this;
    }
};






// **************************** PLAYER 3.1.1 **************************** //
struct file_player_v3_1_1 {
    char name[FS_CHAR_NAME_SIZE];
    char graphic_filename[FS_CHAR_NAME_SIZE];
    char face_filename[FS_CHAR_NAME_SIZE];
    Uint8 HP;
    st_size sprite_size;                                       // size of sprite graphic
    st_rectangle sprite_hit_area;                                   // the area of the graphic where is used for hit/collision
    Uint8 move_speed;                                               // how many sprites move each step
    st_sprite_data sprites[ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT];
    // habilities part
    bool have_shield;
    Uint8 max_shots;                                                // number of maximum simultaneous projectiles
    Uint8 simultaneous_shots;                                       // number of projectiles shot at one button press
    bool can_double_jump;
    bool can_slide;                                                 // if false, dashes instead of sliding
    bool can_charge_shot;
    Sint8 full_charged_projectile_id;
    /// NEW IN FILE-FORMAT 3.0
    bool can_air_dash;
    Sint8 damage_modifier;
    bool can_shot_diagonal;
    st_position_int8 attack_arm_pos;
    Uint8 attack_frame;
    bool double_shot;
    Sint8 normal_shot_projectile_id;

    file_player_v3_1_1(int n) {
        sprintf(name, "PLAYER[%d]", n);
        sprintf(graphic_filename, "%s%d%s", "p", (n+1), ".png");
        face_filename[0] = '\0';
        HP = 0;
        sprite_size.width = 162;
        sprite_size.height = 162;
        sprite_hit_area.x = 0;
        sprite_hit_area.y = 0;
        sprite_hit_area.w = 162;
        sprite_hit_area.h = 162;
        move_speed = 2.0;
        max_shots = 3;
        simultaneous_shots = 1;
        can_double_jump = false;
        have_shield = false;
        can_slide = false;
        can_charge_shot = false;
        full_charged_projectile_id = -1;
        can_air_dash = false;
        damage_modifier = 0;
        can_shot_diagonal = false;
        attack_frame = 0;
        normal_shot_projectile_id = -1;

        /// === DEFAULT VALUES === //
        // ROCK
        if (n == 0) {
            have_shield = true;
            can_slide = true;
            can_charge_shot = true;
        } else if (n == 1) {
            // CANDY
            can_double_jump = true;
            can_shot_diagonal = true;
            damage_modifier = 1;
        } else if (n == 2) {
            // BETA
            max_shots = 4;
            damage_modifier = -1;
            simultaneous_shots = 2;
        } else {
            // KITTY
            have_shield = true;
            can_charge_shot = true;
            can_air_dash = true;
        }
    }

    file_player_v3_1_1() {
        file_player_v3_1_1(0);
    }


};
// **************************** PLAYER 3.1.1 **************************** //




// **************************** NPC 3.1.2 **************************** //
struct file_npc_v3_1_2 {
    Sint8 id;                                                  // serial number
    //unsigned int projectile_id;                              // indicates the projectile ID (relation with file_projectile)
    Sint8 projectile_id[2];                                    // indicate the id of an attack the NCP can user
    char name[CHAR_NAME_SIZE];
    char graphic_filename[FS_CHAR_NAME_SIZE];
    int hp;
    Sint8 direction;                                           // defines how it behavies (kink of AI*)
    Sint8 speed;                                               // defines the distances it can see enemies
    Sint16 walk_range;                                         // defines how long from the start point it can go
    Sint8 facing;                                              // defines the side npc is facing before start moving (also used by LINEWALK behavior)
    struct st_position start_point;
    st_sprite_data sprites[ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT];// changed in 2.0.4
    st_size frame_size;
    bool is_ghost;
    Sint8 shield_type;
    Sint8 IA_type;                                             // IA types. For custom (edited) ones, use IA_TYPES_COUNT + position (in the array)
    Sint8 fly_flag;                                            // 0 - can't fly, 1 - flyer (...)
    char bg_graphic_filename[FS_CHAR_NAME_SIZE];               // holds a static background
    st_position sprites_pos_bg;                                // holds position of sprites in relation with background
    bool is_boss;                                              // indicates if this NPC is a boss
    bool is_sub_boss;                                          // a middle-stage boss, the doors will only open after it's dead
    int respawn_delay;                                         // if > 0, will respawn even if on-screen
    st_position_int8 attack_arm_pos;
    Uint8 attack_frame;
    st_rectangle vulnerable_area;
    Sint8 gfx_effect;                                         // can cause snow, rain, darkned roon, etc


    file_npc_v3_1_2() {
        id = -1;
        projectile_id[0] = -1;
        projectile_id[1] = -1;
        sprintf(name, "%s", "Enemy Name");
        graphic_filename[0] = '\0';
        direction = ANIM_DIRECTION_LEFT;
        speed = 3;
        walk_range = 160;
        facing = ANIM_DIRECTION_LEFT;
        frame_size.width = TILESIZE;
        frame_size.height = TILESIZE;
        is_ghost = false;
        shield_type = 0;
        IA_type = 0;
        fly_flag = 0;
        bg_graphic_filename[0] = '\0';
        is_boss = false;
        is_sub_boss = false;
        respawn_delay = 0;
        attack_frame = -1;
        gfx_effect = -1;
    }
};


// **************************** NPC 3.1.2 **************************** //




struct file_object { // DONE - Game
    char name[CHAR_NAME_SIZE];
    char graphic_filename[FS_CHAR_NAME_SIZE];
    Sint8 type;													///@ TODO: use enum
    int timer;													// used as time for disapearing block
    Sint8 speed;												// used as speed for moving platform
    Sint16 limit;												// used as range (pixels) for moving platform, visible time for disapearing block
    Sint8 direction;											// used to check if moving away from oiginalpoint or moving to it
    Sint16 distance;
    st_size size;
    Sint16 frame_duration;										// indicates what is the used frame
    bool animation_auto_start;                                  // false means it will not be animated until started
    bool animation_reverse;                                     // start animating
    bool animation_loop;                                        // true = animation will loop, false = animation will run once
    Sint16 given_ability;                                       // when object is of type ability, will give this to player
    file_object() {
        sprintf(name, "%s", "Object");
        graphic_filename[0] = '\0';
        animation_auto_start = true;
        animation_reverse = false;
        animation_loop = true;
        frame_duration = 100;
        type = -1;
        timer = 0;
        speed = 2;
        limit = 0;
        animation_auto_start = true;
        animation_reverse = false;
        animation_loop = true;
        given_ability = -1;
    }
};



struct file_ai_action {
    int chance;
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int extra_parameter;										// left, right, etc

    file_ai_action()
    {
        chance = 0;
        action = 0;
        go_to = 0;
        go_to_delay = 500;
        extra_parameter = 0;
    }
};

struct reaction {
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int extra_parameter;										// left, right, etc
    reaction() {
        action = -1;
        go_to = 0;
        go_to_delay = 500;
        extra_parameter = 0;
    }
};

struct file_artificial_inteligence {
    char name[CHAR_NAME_SIZE];
    struct file_ai_action states[AI_MAX_STATES];
    struct reaction reactions[MAX_AI_REACTIONS];                    // reactions near, hit and dead

    file_artificial_inteligence() {
        sprintf(name, "%s", "A.I.");
    }
};


// *** NEW AI format *** //
struct file_ai_action_v3 {
    int chance;                                                 // percentage over 100, used to randomize next action
    int action;                                                 // what will be executed
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int param1;                         						// left, right, etc
    st_rectangle param2;                                        // coordinate or more values

    file_ai_action_v3()
    {
        chance = 0;
        action = 0;
        go_to = 0;
        go_to_delay = 500;
        param1 = 0;
    }
};

struct file_ai_reaction_v3 {
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int param1;                         						// left, right, etc
    st_rectangle param2;                                        // coordinate or more values
    file_ai_reaction_v3() {
        action = -1;
        go_to = 0;
        go_to_delay = 500;
        param1 = 0;
    }
};

struct file_artificial_inteligence_v3 {
    char name[CHAR_NAME_SIZE];
    struct file_ai_action_v3 states[AI_MAX_STATES];
    struct file_ai_reaction_v3 reactions[MAX_AI_REACTIONS];                    // reactions near, hit and dead

    file_artificial_inteligence_v3() {
        sprintf(name, "%s", "A.I.");
    }

    file_artificial_inteligence_v3(file_artificial_inteligence origin) {
        sprintf(name, "%s", origin.name);
        for (int i=0; i<AI_MAX_STATES; i++) {
            states[i].chance = origin.states[i].chance;
            states[i].action = origin.states[i].action;
            states[i].go_to = origin.states[i].go_to;
            states[i].go_to_delay = origin.states[i].go_to_delay;
            states[i].param1 = origin.states[i].extra_parameter;
        }
        for (int i=0; i<MAX_AI_REACTIONS; i++) {
            reactions[i].action = origin.reactions[i].action;
            reactions[i].go_to = origin.reactions[i].go_to;
            reactions[i].go_to_delay = origin.reactions[i].go_to_delay;
            reactions[i].param1 = origin.reactions[i].extra_parameter;
        }
    }

};
// *** NEW AI format *** //


struct st_checkpoint {
    int map; /**< TODO */
    int x; /**< TODO */
    int y; /**< TODO */
    int map_scroll_x; /**< TODO */
            st_checkpoint() {
                reset();
            }
            void reset() {
                map = 0;
                x = RES_W/2-29/2;
                y = -1;
                map_scroll_x = 0;
            }
    };

// CAMPOS NOVOS OU NÃO-USADOS //
struct file_intro {
    int initial_delay;                                  // delay before showing text and effects
    char bg_filename[FS_CHAR_FILENAME_SIZE];            // background image
    int transition_effect;                              // how we transition to next screen
    // text (3 lines x2)
};

struct st_shop_dialog {
    char face_graphics_filename[20];
    char line1[FS_DIALOG_LINES][DIALOG_LINE_LIMIT];
};


struct st_file_trophy {
    Uint8 condition;
    char name[FS_NAME_SIZE];
    char filename[FS_FILENAME_SIZE];
};


struct st_armor_piece {
    int special_ability[FS_MAX_PLAYERS]; // each player can have a different ability
    int got_message[FS_MAX_PLAYERS][FS_DIALOG_LINES];

    st_armor_piece() {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            special_ability[i] = 0;
            for (int j=0; j<FS_DIALOG_LINES; j++) {
                got_message[i][j] = -1;
            }
        }
    }
};


struct file_game {

    // *** WARNINGN *** //
    // any changes made here must be reflected into file_io::write_game() and file_io::read_game() //

    float version;
    char name[FS_CHAR_NAME_SIZE];
    //file_projectile projectiles[FS_MAX_PROJECTILES];
    Sint8 semi_charged_projectile_id;                               // common to all players
    char boss_music_filename[FS_CHAR_NAME_SIZE];
    char final_boss_music_filename[FS_CHAR_NAME_SIZE];
    char got_weapon_music_filename[FS_CHAR_NAME_SIZE];
    char game_over_music_filename[FS_CHAR_NAME_SIZE];
    char stage_select_music_filename[FS_CHAR_NAME_SIZE];
    char game_start_screen_music_filename[FS_CHAR_NAME_SIZE];
    bool use_second_castle;
    Uint8 game_style;                                               // if 1, stages are executed one after another, like castlevania or ghouls & ghosts
    Uint8 final_boss_id;                                            // indicates who is the final boss, the id refers NPC, so it is possible to add the same final boss intro two stages


    // CONSTRUCTOR //
    file_game() {
        /// *** hardcoded parts *** ///
        version = 4.00; // file-format version, not game
        sprintf(name, "%s", "My Game");
        semi_charged_projectile_id = 0;
        boss_music_filename[0] = '\0';
        final_boss_music_filename[0] = '\0';
        got_weapon_music_filename[0] = '\0';
        game_over_music_filename[0] = '\0';
        stage_select_music_filename[0] = '\0';
        game_start_screen_music_filename[0] = '\0';
        game_style = 0;
        use_second_castle = false;
        final_boss_id = -1;

    }

};




// ******************************************* STAGES ********************************************************************** //





#endif // V_4_H
