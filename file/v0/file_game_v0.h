#ifndef V_0_H
#define V_0_H

#include <stdio.h>
#include <iostream>
#include <map>
#include "defines.h"
#include "data/st_common.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"


struct file_projectile_v0 {
    std::string name;
    std::string graphic_filename;
    st_size size;
    bool is_destructible;                                   // indicates if the projectile can be shot down, if true, uses HP
    unsigned int hp;                                               // how much danage needs to be taken to destruct the projectile
    unsigned int trajectory;                                       // RENAMED from projectile_type in 2.1
    unsigned int max_shots;                                        // number of maximum simultaneous projectiles, if 0, is infinite
    unsigned int speed;                                            // now many pixels are moved into a step
    unsigned int damage;                                           // how many hitpoints will be reduced from target
    std::string sfx_filename;                  // allows to use different projectile shot sound
    bool can_be_reflected;                                  // if false, like in a charged-shot, can't be reflected by shield
    int spawn_npc_id;                                     // will spawn NPCs (like bird egg in mm2)
    unsigned int spawn_npc_n;
    bool is_explosive;
    bool vanishes_on_hit;                                   // if false, projectile won't be removed once hitting an enemy

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(name), CEREAL_NVP(graphic_filename), CEREAL_NVP(size), CEREAL_NVP(is_destructible),
              CEREAL_NVP(hp), CEREAL_NVP(trajectory), CEREAL_NVP(max_shots), CEREAL_NVP(speed), CEREAL_NVP(damage),
              CEREAL_NVP(sfx_filename), CEREAL_NVP(can_be_reflected), CEREAL_NVP(spawn_npc_id), CEREAL_NVP(spawn_npc_n),
              CEREAL_NVP(is_explosive), CEREAL_NVP(vanishes_on_hit));
    }

    file_projectile_v0() {
        name =  "Projectile";
        graphic_filename = "projectile_normal.png";
        is_destructible = false;
        hp = 1;
        size.width = 6;
        size.height = 6;
        trajectory = TRAJECTORY_LINEAR;
        max_shots = 3;
        speed = PROJECTILE_DEFAULT_SPEED;
        damage = PROJECTILE_DEFAULT_DAMAGE;
        sfx_filename = "";
        can_be_reflected = true;
        spawn_npc_id = -1;
        spawn_npc_n = 0;
        is_explosive = false;
        vanishes_on_hit = true;
    }
};




struct st_sprite_data_v0 {
    bool used;                                                  // if true, the sprite exists
    unsigned int duration;
    unsigned int sprite_graphic_pos_x;
    st_rectangle collision_rect;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(used), CEREAL_NVP(duration), CEREAL_NVP(sprite_graphic_pos_x), CEREAL_NVP(collision_rect));
    }

    st_sprite_data_v0() {
        used = false;
        duration = 0;
        sprite_graphic_pos_x = 0;
    }

    st_sprite_data_v0 & operator=(st_sprite_data_v0 new_value) {
        used = new_value.used;
        duration = new_value.duration;
        sprite_graphic_pos_x = new_value.sprite_graphic_pos_x;
        collision_rect = new_value.collision_rect;
        return *this;
    }


};


enum PLAYER_ABILITIES_ENUM {
    PLAYER_ABILITIES_DOUBLE_JUMP,
    PLAYER_ABILITIES_CHARGE_SHOT,
    PLAYER_ABILITIES_SLIDE,
    PLAYER_ABILITIES_AIR_DASH,
    PLAYER_ABILITIES_SHIELD,
    PLAYER_ABILITIES_SHOOT_DIAGONALS,
    PLAYER_ABILITIES_DOUBLE_SHOT,
    PLAYER_ABILITIES_COUNT };

struct file_player_v0 {
    std::string name;
    std::string graphic_filename;
    std::string face_filename;
    unsigned int HP;
    st_size sprite_size;                                       // size of sprite graphic
    st_rectangle sprite_hit_area;                                   // the area of the graphic where is used for hit/collision
    unsigned int move_speed;                                               // how many sprites move each step
    std::map<int, std::vector<st_sprite_data_v0>> sprites;                 // index is anim-type
    // habilities part
    bool have_shield;
    unsigned int max_shots;                                                // number of maximum simultaneous projectiles
    unsigned int simultaneous_shots;                                       // number of projectiles shot at one button press
    // TODO: make abilities into a map enum/bool
    bool can_double_jump;
    bool can_slide;                                                 // if false, dashes instead of sliding
    bool can_charge_shot;
    int full_charged_projectile_id;
    bool can_air_dash;
    int damage_modifier;
    bool can_shot_diagonal;
    st_position attack_arm_pos;
    unsigned int attack_frame; // TODO: make into active-frame, and allow to be set for any anim-type
    bool double_shot;
    int normal_shot_projectile_id;
    std::map<PLAYER_ABILITIES_ENUM, bool> abilities;

    file_player_v0(int n) {
        name = "PLAYER" + std::to_string(n);
        graphic_filename = "p" + std::to_string(n+1) + ".png";
        face_filename = "";
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
        have_shield = false;
        can_slide = false;
        can_charge_shot = false;
    }

    file_player_v0() {
        file_player_v0(0);
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(name), CEREAL_NVP(graphic_filename), CEREAL_NVP(face_filename), CEREAL_NVP(HP),
              CEREAL_NVP(sprite_size), CEREAL_NVP(sprite_hit_area), CEREAL_NVP(move_speed), CEREAL_NVP(sprites),
              CEREAL_NVP(have_shield), CEREAL_NVP(max_shots), CEREAL_NVP(simultaneous_shots), CEREAL_NVP(can_double_jump),
              CEREAL_NVP(can_slide), CEREAL_NVP(can_charge_shot), CEREAL_NVP(full_charged_projectile_id),
              CEREAL_NVP(can_air_dash), CEREAL_NVP(damage_modifier), CEREAL_NVP(can_shot_diagonal),
              CEREAL_NVP(attack_arm_pos), CEREAL_NVP(attack_frame), CEREAL_NVP(double_shot),
              CEREAL_NVP(normal_shot_projectile_id));
    }
};




struct file_enemy_v0 {
    int id;                                                  // serial number
    //unsigned int projectile_id;                              // indicates the projectile ID (relation with file_projectile)
    std::string name;
    std::string graphic_filename;
    int hp;
    int direction;                                           // defines how it behavies (kink of AI*)
    int speed;                                               // defines the distances it can see enemies
    int walk_range;                                         // defines how long from the start point it can go
    int facing;                                              // defines the side npc is facing before start moving (also used by LINEWALK behavior)
    struct st_position start_point;
    std::map<int, std::vector<st_sprite_data_v0>> sprites;                 // index is anim-type
    st_size frame_size;
    bool is_ghost;
    int shield_type;
    int fly_flag;                                            // 0 - can't fly, 1 - flyer (...)
    std::string bg_graphic_filename;               // holds a static background
    st_position sprites_pos_bg;                                // holds position of sprites in relation with background
    bool is_boss;                                              // indicates if this NPC is a boss
    bool is_sub_boss;                                          // a middle-stage boss, the doors will only open after it's dead
    int respawn_delay;                                         // if > 0, will respawn even if on-screen
    st_position_int8 attack_arm_pos;
    unsigned int attack_frame;
    st_rectangle vulnerable_area;
    int gfx_effect;                                         // can cause snow, rain, darkned roon, etc
    bool is_npc;
    int npc_dialog_id;
    int npc_requested_item_id;
    int npc_given_item_id;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(id), CEREAL_NVP(name), CEREAL_NVP(graphic_filename), CEREAL_NVP(hp), CEREAL_NVP(direction),
              CEREAL_NVP(speed), CEREAL_NVP(walk_range), CEREAL_NVP(facing), CEREAL_NVP(start_point), CEREAL_NVP(sprites),
              CEREAL_NVP(frame_size), CEREAL_NVP(is_ghost), CEREAL_NVP(shield_type), CEREAL_NVP(fly_flag),
              CEREAL_NVP(bg_graphic_filename), CEREAL_NVP(sprites_pos_bg), CEREAL_NVP(is_boss),
              CEREAL_NVP(is_sub_boss), CEREAL_NVP(respawn_delay), CEREAL_NVP(attack_arm_pos),
              CEREAL_NVP(attack_frame), CEREAL_NVP(vulnerable_area), CEREAL_NVP(gfx_effect),
              CEREAL_NVP(is_npc), CEREAL_NVP(npc_dialog_id), CEREAL_NVP(npc_requested_item_id), CEREAL_NVP(npc_given_item_id));
    }

    file_enemy_v0() {
        id = -1;
        name = "Enemy Name";
        graphic_filename = "";
        direction = ANIM_DIRECTION_LEFT;
        speed = 3;
        walk_range = 160;
        facing = ANIM_DIRECTION_LEFT;
        frame_size.width = TILESIZE;
        frame_size.height = TILESIZE;
        is_ghost = false;
        shield_type = 0;
        fly_flag = 0;
        bg_graphic_filename = "";
        is_boss = false;
        is_sub_boss = false;
        respawn_delay = 0;
        attack_frame = -1;
        gfx_effect = -1;
        is_npc = false;
        npc_dialog_id = -1;
        npc_requested_item_id = -1;
        npc_given_item_id = -1;
    }
};


struct file_npc_v0 {
    int id;                                                  // serial number
    std::string name;
    std::string graphic_filename;
    int direction;                                           // direction it faces by default
    int speed;                                               // defines the distances it can see enemies
    int walk_range;                                         // defines how long from the start point it can go
    int npc_move_behavior;                                  // NPCs do not have programmeable behavior in the editor, but use some hardcoded pre-defined types
    struct st_position start_point;
    int frame_width;
    int frame_duration;
    st_rectangle hit_area;
    int npc_dialog_id;
    int npc_quest_id;                                       // npc has a quest that can be more complex and include dialogs
    int npc_requested_item_id;                              // npcs wants an item to give another
    int npc_given_item_id;
    std::map<int, std::vector<st_sprite_data_v0>> sprites;                 // index is anim-type

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(id), CEREAL_NVP(name), CEREAL_NVP(graphic_filename), CEREAL_NVP(direction),
              CEREAL_NVP(speed), CEREAL_NVP(walk_range), CEREAL_NVP(npc_move_behavior), CEREAL_NVP(start_point),
              CEREAL_NVP(frame_width), CEREAL_NVP(frame_duration), CEREAL_NVP(hit_area), CEREAL_NVP(npc_dialog_id),
              CEREAL_NVP(npc_quest_id), CEREAL_NVP(npc_requested_item_id), CEREAL_NVP(npc_given_item_id),
              CEREAL_NVP(sprites));
    }

    file_npc_v0() {
        id = -1;
        name = "NPC Name";
        graphic_filename = "";
        direction = ANIM_DIRECTION_LEFT;
        speed = 3;
        walk_range = 160;
        frame_width = TILESIZE;
        npc_move_behavior = 0;
        frame_duration = 100;
        npc_dialog_id = -1;
        npc_quest_id = -1;
        npc_requested_item_id = -1;
        npc_given_item_id = -1;
        hit_area = st_rectangle(0, 0, 0, 0);
    }
};



struct file_object_v0 {
    std::string name;
    std::string graphic_filename;
    int type;													///@ TODO: use enum
    int timer;													// used as time for disapearing block
    int speed;												// used as speed for moving platform
    int limit;												// used as range (pixels) for moving platform, visible time for disapearing block
    int direction;											// used to check if moving away from oiginalpoint or moving to it
    int distance;
    st_size size;
    int frame_duration;										// indicates what is the used frame
    bool animation_auto_start;                                  // false means it will not be animated until started
    bool animation_reverse;                                     // start animating
    bool animation_loop;                                        // true = animation will loop, false = animation will run once
    int given_ability;                                       // when object is of type ability, will give this to player


    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(name), CEREAL_NVP(graphic_filename), CEREAL_NVP(timer),
              CEREAL_NVP(timer), CEREAL_NVP(speed), CEREAL_NVP(limit), CEREAL_NVP(direction),
              CEREAL_NVP(distance), CEREAL_NVP(size), CEREAL_NVP(frame_duration), CEREAL_NVP(animation_auto_start),
              CEREAL_NVP(animation_reverse), CEREAL_NVP(animation_loop), CEREAL_NVP(given_ability));
    }


    file_object_v0() {
        name = "Object";
        graphic_filename = "";
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

struct reaction_v0 {
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int extra_parameter;										// left, right, etc

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(action), CEREAL_NVP(go_to), CEREAL_NVP(go_to_delay), CEREAL_NVP(extra_parameter));
    }

    reaction_v0() {
        action = -1;
        go_to = 0;
        go_to_delay = 500;
        extra_parameter = 0;
    }
};

struct file_ai_action_v0 {
    int chance;
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int extra_parameter;										// left, right, etc

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(chance), CEREAL_NVP(action), CEREAL_NVP(go_to), CEREAL_NVP(go_to_delay), CEREAL_NVP(extra_parameter));
    }

    file_ai_action_v0()
    {
        chance = 0;
        action = 0;
        go_to = 0;
        go_to_delay = 500;
        extra_parameter = 0;
    }
};

struct file_artificial_inteligence_v0 {
    std::string name;
    std::vector<file_ai_action_v0> states;
    std::vector<reaction_v0> reactions;                    // reactions near, hit and dead

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(name), CEREAL_NVP(states), CEREAL_NVP(reactions));
    }

    file_artificial_inteligence_v0() {
        name = "A.I.";
    }
};



struct file_ai_reaction_v0 {
    int action;
    int go_to;                                                  // action number to be executed once this one is finished, -1 indicates that must return to CHANCES (random)
    int go_to_delay;                                            // delay before going to the next action
    int param1;                         						// left, right, etc
    st_rectangle param2;                                        // coordinate or more values
    file_ai_reaction_v0() {
        action = -1;
        go_to = 0;
        go_to_delay = 500;
        param1 = 0;
    }
};

struct st_checkpoint_v0 {
    int map;
    int x;
    int y;
    int map_scroll_x;
            st_checkpoint_v0() {
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
struct file_intro_v0 {
    int initial_delay;                                  // delay before showing text and effects
    std::string bg_filename;            // background image
    int transition_effect;                              // how we transition to next screen
    // text (3 lines x2)
};

struct st_shop_dialog_v0 {
    std::string face_graphics_filename;
    std::string line1[FS_DIALOG_LINES];
};


struct st_file_trophy_v0 {
    unsigned int condition;
    std::string name;
    std::string filename;
};


struct st_armor_piece_v0 {
    int special_ability[FS_MAX_PLAYERS]; // each player can have a different ability
    int got_message[FS_MAX_PLAYERS][FS_DIALOG_LINES];

    st_armor_piece_v0() {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            special_ability[i] = 0;
            for (int j=0; j<FS_DIALOG_LINES; j++) {
                got_message[i][j] = -1;
            }
        }
    }
};


struct file_game_v0 {
    float version;
    std::string name;
    //file_projectile projectiles[FS_MAX_PROJECTILES];
    int semi_charged_projectile_id;                               // common to all players
    std::string boss_music_filename;
    std::string final_boss_music_filename;
    std::string got_weapon_music_filename;
    std::string game_over_music_filename;
    std::string stage_select_music_filename;
    std::string game_start_screen_music_filename;
    bool use_second_castle;
    unsigned int game_style;                                               // if 1, stages are executed one after another, like castlevania or ghouls & ghosts
    int final_boss_id;                                            // indicates who is the final boss, the id refers NPC, so it is possible to add the same final boss intro two stages
    int obj_uuid;
    int npc_uuid;


    // CONSTRUCTOR //
    file_game_v0() {
        /// *** hardcoded parts *** ///
        version = 4.00; // file-format version, not game
        name = "My Game";
        semi_charged_projectile_id = 0;
        boss_music_filename = "";
        final_boss_music_filename = "";
        got_weapon_music_filename = "";
        game_over_music_filename = "";
        stage_select_music_filename = "";
        game_start_screen_music_filename = "";
        game_style = 0;
        use_second_castle = false;
        final_boss_id = -1;
        obj_uuid = 0;
        npc_uuid = 0;
    }

};

struct file_map_anim_block_v0 {
    int x;
    int y;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(x), CEREAL_NVP(y));
    }

    file_map_anim_block_v0() {
        x = 0;
        y = 0;
    }
    file_map_anim_block_v0(int set_x, int set_y) {
        x = set_x;
        y = set_y;
    }
};

struct file_anim_block_v0 {
    std::string filename;
    std::vector<int> frame_delay;

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(CEREAL_NVP(filename), CEREAL_NVP(frame_delay));
    }

    file_anim_block_v0() {
        filename = "";
    }
};


// ******************************************* STAGES ********************************************************************** //





#endif // V_0_H
