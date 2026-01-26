#include "dataconverter.h"


DataConverter::DataConverter()
{

}

file_projectile_v0 DataConverter::projectile_v4_to_v0(file_projectilev3 data)
{
    file_projectile_v0 res;
    res.can_be_reflected = data.can_be_reflected;
    res.damage = data.damage;
    res.graphic_filename = data.graphic_filename;
    res.hp = data.hp;
    res.is_destructible = data.is_destructible;
    res.is_explosive = data.is_explosive;
    res.max_shots = data.max_shots;
    res.name = data.name;
    res.sfx_filename = data.sfx_filename;
    res.size = data.size;
    res.spawn_npc_id = data.spawn_npc_id;
    res.spawn_npc_n = data.spawn_npc_n;
    res.speed = data.speed;
    res.trajectory = data.trajectory;
    res.vanishes_on_hit = data.vanishes_on_hit;

    return res;
}

file_player_v0 DataConverter::player_v3_1_to_v0(file_player_v3_1_1 data)
{
    file_player_v0 res;
    res.HP = data.HP;
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_DOUBLE_JUMP, data.can_double_jump));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_CHARGE_SHOT, data.can_charge_shot));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_SLIDE, data.can_slide));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_SHIELD, data.have_shield));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_AIR_DASH, data.can_air_dash));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_SHOOT_DIAGONALS, data.can_shot_diagonal));
    res.abilities.insert(std::pair<PLAYER_ABILITIES_ENUM, bool>(PLAYER_ABILITIES_DOUBLE_SHOT, data.double_shot));

    res.attack_arm_pos = st_position(data.attack_arm_pos.x, data.attack_arm_pos.y);
    res.attack_frame = data.attack_frame;
    res.damage_modifier = data.damage_modifier;
    res.face_filename = data.face_filename;
    res.full_charged_projectile_id = data.full_charged_projectile_id;
    res.graphic_filename = data.graphic_filename;
    res.max_shots = data.max_shots;
    res.move_speed = data.move_speed;
    res.name = data.name;
    res.normal_shot_projectile_id = data.normal_shot_projectile_id;
    res.simultaneous_shots = data.simultaneous_shots;
    res.sprite_hit_area = data.sprite_hit_area;
    res.sprite_size = data.sprite_size;
    //st_sprite_data sprites[ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT];
    for (unsigned int i=0; i<ANIM_TYPE_COUNT; i++) {
        std::vector<st_sprite_data_v0> sprites;
        for (unsigned j=0; j<ANIM_FRAMES_COUNT; j++) {
            if (data.sprites[i][j].used == true) {
                st_sprite_data_v0 sprite;
                sprite.collision_rect = data.sprites[i][j].collision_rect;
                sprite.duration = data.sprites[i][j].duration;
                sprite.sprite_graphic_pos_x = data.sprites[i][j].sprite_graphic_pos_x;
                sprite.used = data.sprites[i][j].used;
                sprites.push_back(sprite);;
            }
        }
        if (sprites.size() > 0) {
            res.sprites.insert(std::pair<int, std::vector<st_sprite_data_v0>>(i, sprites));
        }
    }
    return res;

}
