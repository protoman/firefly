#include <sstream>
#include <list>
#include <math.h>

#ifdef ANDROID
#include <android/log.h>
#endif

#include "classplayer.h"
#include "view/imageview.h"
#include "view/soundview.h"

#include "controller/mapcontroller.h"

#include "game_data.h"
#include "GameManager.h"

//#define PLAYER_MOVE_SPEED 6.00 // higher is faster
#define PLAYER_MOVE_SPEED 4.00 // higher is faster
#include "file/file_io.h"




// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
classPlayer::classPlayer(int playerNumber) : teleporter_n(-1), selected_weapon(-1), l_key_released(true), r_key_released(true)
{

    std::cout << "### PLAYER::CREATE::number[" << _number << "]" << std::endl;

    _number = playerNumber;
    position.y = 0;
    position.x = TILESIZE*AREA_ROOM_TILES_W*1.5;
    hit_duration = 2000;
    hitPoints.total = PLAYER_INITIAL_HP;
	hitPoints.current = hitPoints.total;
	shield_type = SHIELD_FRONT; /// @TODO: from editor
	// load items from save
    selected_weapon = 0;
    move_speed = PLAYER_MOVE_SPEED;

    reset_charging_shot();
}

void classPlayer::set_player_name(std::string set_name)
{
    name = set_name;
}

void classPlayer::initialize()
{
    _number = SharedData::get_instance()->game_save.selected_player;
    char temp_name[30];
    sprintf(temp_name, "PLAYER_%lu", _number);
    name = std::string(temp_name);

    max_projectiles = GameData::get_instance()->player_list_v3_1[_number].max_shots;
    // it is a player, can't have zero projectiles!!
    if (max_projectiles < 1) {
        max_projectiles = 1;
    }

    //std::cout << "player.number[" << _number << "]" << std::endl;
    _charged_shot_projectile_id = GameData::get_instance()->player_list_v3_1[_number].full_charged_projectile_id;
    _normal_shot_projectile_id = GameData::get_instance()->player_list_v3_1[_number].normal_shot_projectile_id;

    //std::cout << "classPlayer::initialize - player[" << _number << "][" << name << "], _normal_shot_projectile_id[" << _normal_shot_projectile_id << "]" << std::endl;

    _simultaneous_shots = GameData::get_instance()->player_list_v3_1[_number].simultaneous_shots;
    //std::cout << "classjump::set_acceleration - player[" << name << "], accel[" << GameMediator::get_instance()->player_list[_number].jump_gravity << "]" << std::endl;
    if (can_double_jump() == true) {
        _jumps_number = 2;
    } else {
        _jumps_number = 1;
    }
    _damage_modifier = GameData::get_instance()->player_list_v3_1[_number].damage_modifier;
}


bool classPlayer::get_item(object_collision &obj_info)
{

    //std::cout << "classPlayer::get_item[" << (int)obj_info._object->get_type() << "]" << std::endl;

    if (state.animation_type == ANIM_TYPE_TELEPORT) {
        return false;
    }
	if (character::get_item(obj_info)) {
		return true;
	}

	bool res = false;
	// deal with non-blocking items
	if (obj_info._object != nullptr && obj_info._object->finished() == false) {
        //if (obj_info._object->get_name() != "Treasure Chest") std::cout << "character::classPlayer::START [" << obj_info._object->get_name() << "]" << std::endl;
        //std::cout << "classPlayer::get_item" << std::endl;
		switch (obj_info._object->get_type()) {
		case OBJ_ENERGY_TANK:
            if (SharedData::get_instance()->game_save.items.energy_tanks < 9) { // max 9
                SharedData::get_instance()->game_save.items.energy_tanks++;
            }
			obj_info._object->set_finished(true);

            SoundView::get_instance()->play_sfx(SFX_GOT_ITEM);
            res = true;
			break;
		case OBJ_WEAPON_PILL_BIG:
			obj_info._object->set_finished(true);
			recharge(ENERGY_TYPE_WEAPON, ENERGY_ITEM_BIG);
            res = true;
			break;
		case OBJ_WEAPON_PILL_SMALL:
			obj_info._object->set_finished(true);
			recharge(ENERGY_TYPE_WEAPON, ENERGY_ITEM_SMALL);
            res = true;
			break;
        case OBJ_ABILITY_ITEM:
        {
            SoundView::get_instance()->play_sfx(SFX_GOT_ITEM);
            obj_info._object->set_finished(true);
            int given_ability = obj_info._object->get_ability();
            if (given_ability == -1 || given_ability > ABILITIES_SLOTS) {
                int obj_id = obj_info._object->get_id();
                std::cout << "ERROR: given_ability[" << given_ability << "] is invalid for object ["  << obj_id << "]" << std::endl;
                exit(-1);
            }
            SharedData::get_instance()->game_save.abilities[given_ability] = true;
            GameManager::get_instance()->show_ability_item_dialog(given_ability);

            v6_file_game_object_state obj_state;
            obj_state.uuid = obj_info._object->get_uuid();
            obj_state.obj_id = obj_info._object->get_id();
            obj_state.area_n = SharedData::get_instance()->v6_selected_stage;
            obj_state.x = -1;
            obj_state.y = -1;
            obj_state.finished = true;
            SharedData::get_instance()->game_object_state_map.insert(std::pair<int, v6_file_game_object_state>(obj_state.uuid, obj_state));

            set_animation_type(ANIM_TYPE_GOT_ITEM);
            cancel_slide();
            _obj_jump.interrupt();
            set_animation_type(ANIM_TYPE_GOT_ITEM);

            res = true;
            break;
        }
        case OBJ_DOOR_KEY:
        {
            SoundView::get_instance()->play_sfx(SFX_GOT_ITEM);
            int key_n = obj_info._object->get_key_n();
            SharedData::get_instance()->game_save.keys[key_n] = true;
            res = true;
            obj_info._object->set_finished(true);
            break;
        }
        default:
            //std::cout << "classPlayer::get_item - unknown item type: " << (int)obj_info._object->get_type() << std::endl;
			break;
		}
	}
    return res;
}



void classPlayer::recharge(e_energy_types _en_type, int value)
{
	if (_en_type == ENERGY_TYPE_HP) {
        // TODO::IURI - MP //
		character::recharge(_en_type, value);
    }
}


bool classPlayer::shoryuken()
{
    // trying to start command (can only start if standing)

    //std::cout << ">> SHOURYUKEN - up: " << moveCommands.up << ", dash: " << moveCommands.dash << ", jump: " << moveCommands.jump << std::endl;

    if (moveCommands.up != 0 && moveCommands.dash != 0 && state.animation_type == ANIM_TYPE_STAND)  {
        state.animation_type = ANIM_TYPE_SPECIAL_ATTACK;
        std::cout << ">>>>>>>>>>>>>>>>>>>> SHORYUKEN::START" << std::endl;
        SoundView::get_instance()->play_sfx(SFX_SHORYUKEN_GIRL);
        _obj_jump.start(true, TERRAIN_UNBLOCKED);
        _can_execute_airdash = false;
        return true;
    // is executing
    } else if (state.animation_type == ANIM_TYPE_SPECIAL_ATTACK) {
        std::cout << ">>>>>>>>>>>>>>>>>>>> SHORYUKEN::EXECUTE" << std::endl;
        _obj_jump.execute(TERRAIN_UNBLOCKED);
        int jump_speed = _obj_jump.get_speed();
        bool jump_moved = false;

        // check collision
        for (int i=abs((float)jump_speed); i>0; i--) {
            int speed_y = 0;
            if (jump_speed > 0) {
                speed_y = i;
            } else {
                speed_y = i*-1;
            }
            st_map_collision map_col = map_collision(0, speed_y, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
            int map_lock = map_col.block;
            //std::cout << "jump::check_collision - i[" << i << "], map_lock["  << map_lock << "]" << std::endl;

            if (map_lock == BLOCK_UNBLOCKED || map_lock == BLOCK_WATER) {
                std::cout << ">>>>>>>>>>>>>>>>>>>> PLAYER::INC-Y #1" << std::endl;
                position.y += speed_y;
                jump_moved = true;
                break;
            }
        }
        if (jump_speed != 0 && jump_moved == false) {
            if (jump_speed < 0) {
                _obj_jump.interrupt();
            } else {
                _obj_jump.finish();
                state.animation_type = ANIM_TYPE_STAND;
            }
        }



        if (_obj_jump.is_started() == false) {
            return false;
        }
        return true;
    }
    return false;
}

Uint8 classPlayer::get_max_hp()
{
    return fio.get_heart_pieces_number(SharedData::get_instance()->game_save);
}


void classPlayer::attack(bool dont_update_colors)
{
    st_position proj_pos;


    if (state.animation_type == ANIM_TYPE_HIT) { // can't fire when hit
        //std::cout << "classPlayer::attack::LEAVE #1" << std::endl;
        return;
    }

    if (state.animation_type == ANIM_TYPE_SHIELD) { // can't attack when using shield
        //std::cout << "classPlayer::attack::LEAVE #2" << std::endl;
        return;
    }

    bool always_charged = false;
    // player with armor-special-type changes to auto-carged instead of charging shot

    if (selected_weapon == -1) {
        /// @NOTE: desabilitei o tiro em diagonal pois vai precisar mudanças no sistema de arquivos para comportar as poses/frames de ataque para cima e para baixo

        if (SharedData::get_instance()->game_config.auto_charge_mode) {
            if (moveCommands.attack == 1) {
                std::cout << "auto-charge-attack" << std::endl;
                moveCommands.attack = 0;
                attack_button_released = true;
            } else {
                moveCommands.attack = 1;
                attack_button_released = false;
            }
        }

        bool is_on_ground = hit_ground();
        if (can_shoot_diagonal() == true && moveCommands.up != 0 && is_on_ground == true) {
            character::attack(false, 1, always_charged);
        } else if (can_shoot_diagonal() == true && moveCommands.down != 0 && is_on_ground == true) {
            character::attack(false, -1, always_charged);
        } else {
            character::attack(false, 0, always_charged);
        }
        if (_player_must_reset_colors == true) {
            _player_must_reset_colors = false;
        }
        //std::cout << "classPlayer::attack::LEAVE #3" << std::endl;
        return;
    }


    if (moveCommands.attack == 0 && attack_button_released == false) {
        //std::cout << ">>>>>>>>> attack_button_released[TRUE] #2 <<<<<<<<<<<<<" << std::endl;
        attack_button_released = true;
        return;
    }

    int effect_type = is_executing_effect_weapon();
    int used_weapon = selected_weapon;
    if (effect_type == TRAJECTORY_FREEZE) { // freeze can shoot normal projectiles
        if (max_projectiles > get_projectile_count()) {
            character::attack(true, 0, always_charged);
        }
        return;
    } else if (effect_type != -1) {
        if (moveCommands.attack != 0 && (TimerView::get_instance()->getTimer()-state.attack_timer) > 100 && attack_button_released == true) {
            inc_effect_weapon_status(); // this method have a filter to inc only the types that are effect (centered, bomb, etc)
        }
        return;
    }
    if (max_projectiles <= get_projectile_count()) {
        return;
    }

    if (moveCommands.attack != 0 && (TimerView::get_instance()->getTimer()-state.attack_timer) > 100 && attack_button_released == true) {
        //std::cout << "########## attack_button_released[FALSE] #1 ##########" << std::endl;
        attack_button_released = false;


        if (state.direction == ANIM_DIRECTION_LEFT) {
            proj_pos = st_position(position.x, position.y+frameSize.height/2);
        } else {
            proj_pos = st_position(position.x+frameSize.width-TILESIZE*2, position.y+frameSize.height/2);
        }


        int proj_direction = state.direction;
        if (_number == 0) { // frankie can shoot 8 directions
            if (InputController::get_instance()->p1_input[BTN_R] == 1) {
                if (InputController::get_instance()->p1_input[BTN_DOWN] == 1) {
                    if (state.direction == ANIM_DIRECTION_LEFT) {
                        proj_direction = ANIM_DIRECTION_DOWN_LEFT;
                    } else {
                        proj_direction = ANIM_DIRECTION_DOWN_RIGHT;
                    }
                } else {
                    if (state.direction == ANIM_DIRECTION_LEFT) {
                        proj_direction = ANIM_DIRECTION_UP_LEFT;
                    } else {
                        proj_direction = ANIM_DIRECTION_UP_RIGHT;
                    }
                }
            } else {
                if (InputController::get_instance()->p1_input[BTN_UP] == 1) {
                    proj_direction = ANIM_DIRECTION_UP;
                } else if (InputController::get_instance()->p1_input[BTN_DOWN] == 1) {
                    proj_direction = ANIM_DIRECTION_DOWN;
                }
            }
        }

        short int weapon_id = 0;

        std::cout << "PLAYER::ATTACK - used_weapon[" << used_weapon << "]" << std::endl;

        weapon_id = used_weapon;

        if (weapon_id == -1) {
            weapon_id = 0;
        }

        std::cout << ">>>>>>>>>>>>>>>>>> proj_direction[" << proj_direction << "] <<<<<<<<<<<<<<<<<<" << std::endl;
        projectile_list.push_back(projectile(0, proj_direction, get_attack_position(), is_player()));
        projectile &temp_proj = projectile_list.back();
        temp_proj.play_sfx(false);
        temp_proj.set_is_permanent();
        temp_proj.set_weapon_id(weapon_id);
        temp_proj.set_owner(this);
        temp_proj.set_max_dist(TILESIZE*4);


        std::cout << "classPlayer::attack::DEBUG #1" << std::endl;

        int weapon_trajectory = GameData::get_instance()->get_projectile(0).trajectory;
        if (weapon_trajectory == TRAJECTORY_CENTERED || weapon_trajectory == TRAJECTORY_SLASH) {
            temp_proj.set_owner_direction(&state.direction);
            temp_proj.set_owner_position(&position);
        } else if (weapon_trajectory == TRAJECTORY_CHAIN) {
            temp_proj.set_owner_position(&position);
            temp_proj.set_owner_direction(&state.direction);

        } else if (weapon_trajectory == TRAJECTORY_FOLLOW) {
            st_rectangle hitbox = get_hitbox();
            GameEnemy* temp = GameManager::get_instance()->get_current_map_obj()->find_nearest_enemy(st_position(hitbox.x+hitbox.w/2, hitbox.y+hitbox.h/2));
            if (temp != nullptr) {
                std::cout << "PLAYER::attack - could not find target" << std::endl;
                temp_proj.set_target_position(temp->get_position_ref());
            }
        } else if (weapon_trajectory == TRAJECTORY_TARGET_DIRECTION || weapon_trajectory == TRAJECTORY_TARGET_EXACT || weapon_trajectory == TRAJECTORY_ARC_TO_TARGET) {
            st_rectangle hitbox = get_hitbox();
            st_position player_pos(hitbox.x+hitbox.w/2, hitbox.y+hitbox.h/2);
            GameEnemy* temp = GameManager::get_instance()->get_current_map_obj()->find_nearest_enemy_on_direction(player_pos, state.direction);
            if (temp != nullptr) {
                std::cout << "PLAYER::attack - could not find target" << std::endl;
                temp_proj.set_target_position(temp->get_position_ref());
            }
            if (weapon_trajectory == TRAJECTORY_ARC_TO_TARGET) {
                set_animation_type(ANIM_TYPE_ATTACK_DIAGONAL_UP);
            } else if (temp != nullptr && (weapon_trajectory == TRAJECTORY_TARGET_DIRECTION || weapon_trajectory == TRAJECTORY_TARGET_EXACT)) {
                st_rectangle npc_hitbox = temp->get_hitbox();
                st_position npc_pos(npc_hitbox.x + (npc_hitbox.w/2), npc_hitbox.y + (npc_hitbox.h/2));
                // check if fire if diagonal (distY > distX)
                int distY = abs(npc_pos.y-(player_pos.y-TILESIZE*2));
                if (distY > 0) {
                    if (npc_pos.y > player_pos.y) {
                        set_animation_type(ANIM_TYPE_ATTACK_DIAGONAL_DOWN);
                    } else {
                        set_animation_type(ANIM_TYPE_ATTACK_DIAGONAL_UP);
                    }
                }
            }
        }

        std::cout << "classPlayer::attack::DEBUG #2" << std::endl;

        attack_state = ATTACK_START;
        state.attack_timer = TimerView::get_instance()->getTimer();
        if (state.animation_type == ANIM_TYPE_STAND) {
            set_animation_type(ANIM_TYPE_ATTACK);
        } else if (state.animation_type == ANIM_TYPE_JUMP) {
            set_animation_type(ANIM_TYPE_JUMP_ATTACK);
        } else if (state.animation_type == ANIM_TYPE_STAIRS || state.animation_type == ANIM_TYPE_STAIRS_SEMI || state.animation_type == ANIM_TYPE_STAIRS_MOVE) {
            set_animation_type(ANIM_TYPE_STAIRS_ATTACK);
        } else if (state.animation_type == ANIM_TYPE_WALK) {
            set_animation_type(ANIM_TYPE_WALK_ATTACK);
        }

        std::cout << "classPlayer::attack::DEBUG #3" << std::endl;
    }
}

void classPlayer::damage_ground_npcs()
{
	/// @TODO - this part must be done only ONCE
	// find quake in projectiles list
	int projectile_n = -1;
    for (int i =0; i<GameData::get_instance()->get_projectile_list_size(); i++) {
        if (GameData::get_instance()->get_projectile(i).trajectory == TRAJECTORY_QUAKE) {
			projectile_n = i;
			break;
		}
	}
	if (projectile_n == -1) {
		std::cout << "damage_ground_npcs - could not find projectile of QUAKE trajectory type" << std::endl;
		return;
	}
	// find quake weapon in weapons list
	// could not find the weapon

    for (int i=0; i<GameManager::get_instance()->get_current_map_obj()->map_enemy_list.size(); i++) {
        if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_on_visible_screen() == false) {
			continue;
		}

		// check if NPC is on ground
        st_position npc_pos(GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().x, GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().y);
        npc_pos.x = (npc_pos.x + GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).get_size().width/2)/TILESIZE;
        npc_pos.y = (npc_pos.y + GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).get_size().height)/TILESIZE;
        int lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(npc_pos);
		if (lock == TERRAIN_UNBLOCKED || lock == TERRAIN_STAIR || lock == TERRAIN_WATER) {
			continue;
		} else {
            GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).damage(TOUCH_DAMAGE_BIG, false);
		}
	}
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void classPlayer::initFrames()
{

    frameSize.width = PLAYER_SPRITE_SIZE;
    frameSize.height = PLAYER_SPRITE_SIZE;


    add_graphic();

	st_imageData playerSpriteSurface;
    char filename[512];
    sprintf(filename, "%s/images/sprites/p%lu.png", SharedData::get_instance()->FILEPATH.c_str(), _number+1);

    //std::cout << "FILEPATH[" << SharedData::get_instance()->FILEPATH << "]" << std::endl;
    //std::cout << "p1.filename[" << filename << "]" << std::endl;

    playerSpriteSurface = ImageView::get_instance()->imageFromFile(filename);
    if (playerSpriteSurface.surface == nullptr) {
		std::cout << "initFrames - Error loading player surface from file\n";
		return;
	}

    //animation_obj.init(name, filename, frameSize, GameMediator::get_instance()->player_list_v3_1.at(0).sprites);


    // TODO - add collision for player

    // @TODO - automatically add inverse direction (right) sprites

	// STAND
    addSpriteFrame(ANIM_TYPE_STAND, 3, playerSpriteSurface, 170);
    addSpriteFrame(ANIM_TYPE_STAND, 4, playerSpriteSurface, 170);
    addSpriteFrame(ANIM_TYPE_STAND, 5, playerSpriteSurface, 170);
    addSpriteFrame(ANIM_TYPE_STAND, 4, playerSpriteSurface, 170);



	// WALK
    addSpriteFrame(ANIM_TYPE_WALK, 8, playerSpriteSurface, WALK_FRAME_DELAY);
    addSpriteFrame(ANIM_TYPE_WALK, 9, playerSpriteSurface, WALK_FRAME_DELAY);
    addSpriteFrame(ANIM_TYPE_WALK, 8, playerSpriteSurface, WALK_FRAME_DELAY);
    addSpriteFrame(ANIM_TYPE_WALK, 7, playerSpriteSurface, WALK_FRAME_DELAY);
    addSpriteFrame(ANIM_TYPE_WALK, 6, playerSpriteSurface, WALK_FRAME_DELAY);
    addSpriteFrame(ANIM_TYPE_WALK, 7, playerSpriteSurface, WALK_FRAME_DELAY);

	// JUMP
    addSpriteFrame(ANIM_TYPE_JUMP, 10, playerSpriteSurface, 150);
	// ATTACK
    addSpriteFrame(ANIM_TYPE_ATTACK, 12, playerSpriteSurface, 150);
	// ATTACK + JUMP
    addSpriteFrame(ANIM_TYPE_JUMP_ATTACK, 11, playerSpriteSurface, 80);
	// ATTACK + WALK
    addSpriteFrame(ANIM_TYPE_WALK_ATTACK, 13, playerSpriteSurface, 150);
    addSpriteFrame(ANIM_TYPE_WALK_ATTACK, 14, playerSpriteSurface, 150);
    addSpriteFrame(ANIM_TYPE_WALK_ATTACK, 15, playerSpriteSurface, 150);
	// HIT
    addSpriteFrame(ANIM_TYPE_HIT, 16, playerSpriteSurface, 100);
    addSpriteFrame(ANIM_TYPE_HIT, 17, playerSpriteSurface, 100);
	// TELEPORT
    addSpriteFrame(ANIM_TYPE_TELEPORT, 0, playerSpriteSurface, 200);
    addSpriteFrame(ANIM_TYPE_TELEPORT, 1, playerSpriteSurface, 100);
    addSpriteFrame(ANIM_TYPE_TELEPORT, 2, playerSpriteSurface, 100);
	// STAIRS
    addSpriteFrame(ANIM_TYPE_STAIRS, 18, playerSpriteSurface, 5000);
	// stairs semi
    addSpriteFrame(ANIM_TYPE_STAIRS_SEMI, 19, playerSpriteSurface, 5000);
    addSpriteFrame(ANIM_TYPE_STAIRS_SEMI, 19, playerSpriteSurface, 5000);
    addSpriteFrame(ANIM_TYPE_STAIRS, 18, playerSpriteSurface, 5000);
	// stairs + move
    addSpriteFrame(ANIM_TYPE_STAIRS_MOVE, 18, playerSpriteSurface, 200);
    // stairs + attack
    addSpriteFrame(ANIM_TYPE_STAIRS_ATTACK, 20, playerSpriteSurface, 500);

    // slide
    addSpriteFrame(ANIM_TYPE_SLIDE, 21, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_SLIDE, 22, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_SLIDE, 23, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_SLIDE, 24, playerSpriteSurface, 1000000);

    // throw
    //addSpriteFrame(ANIM_TYPE_THROW, 21, 0, playerSpriteSurface, 1000);


	// shield
    addSpriteFrame(ANIM_TYPE_SHIELD, 22, playerSpriteSurface, 100);

    // shoot-diagonal-up
    addSpriteFrame(ANIM_TYPE_ATTACK_DIAGONAL_UP, 23, playerSpriteSurface, 100);

    // shoot-diagonal-down
    addSpriteFrame(ANIM_TYPE_ATTACK_DIAGONAL_DOWN, 24, playerSpriteSurface, 100);

    // comemoration/got-weapon
    addSpriteFrame(ANIM_TYPE_GOT_WEAPON, 26, playerSpriteSurface, 200);
    addSpriteFrame(ANIM_TYPE_GOT_WEAPON, 27, playerSpriteSurface, 200);

    addSpriteFrame(ANIM_TYPE_SPECIAL_ATTACK, 29, playerSpriteSurface, 200);

    addSpriteFrame(ANIM_TYPE_GOT_ITEM, 33, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_GOT_ITEM, 34, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_GOT_ITEM, 35, playerSpriteSurface, 60);
    addSpriteFrame(ANIM_TYPE_GOT_ITEM, 36, playerSpriteSurface, 8000);


    playerSpriteSurface.freeGraphic();

}


void classPlayer::execute()
{
    move();
    if (have_shoryuken() == true && shoryuken() == true) { // while doing shoryuken won't move
        return;
    }

    charMove();
    attack();

}

void classPlayer::execute_projectiles()
{
    // animate projectiles
    std::vector<projectile>::iterator it;
    bool ignore_hit_timer = false;
    if (_simultaneous_shots > 1) {
        ignore_hit_timer = true;
    }

    for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        if ((*it).is_finished == true) {
            projectile_list.erase(it);
            break;
        }
        st_size moved = (*it).move();

        //std::cout << "projectile.move_type: " << (*it)->get_move_type() << std::endl;

        /// @TODO projectiles that are tele-guided
        if ((*it).get_move_type() == TRAJECTORY_QUAKE) {
            damage_ground_npcs();
            continue;
        }
        (*it).draw();
        if ((*it).is_reflected == true) {
            continue;
        }

        // check collision against enemies
        for (int i=0; i<GameManager::get_instance()->get_current_map_obj()->map_enemy_list.size(); i++) {
            if ((*it).is_finished == true) {
                projectile_list.erase(it);
                break;
            }
            if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_on_visible_screen() == false) {
                continue;
            }
            if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_dead() == true) {
                continue;
            }


            // collision against whole body
            st_rectangle npc_hitbox = GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).get_hitbox();
            //std::cout << "### #1 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].hitbox[" << npc_hitbox.x << "," << npc_hitbox.y << "," << npc_hitbox.w << "," << npc_hitbox.h << "]" << std::endl;

            //classnpc* enemy = (*enemy_it);
            if ((*it).check_collision(npc_hitbox, st_position(moved.width, moved.height)) == true) {

                //std::cout << "### #2 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].hit[TRUE]" << std::endl;

                // shielded NPC: reflects/finishes shot
                if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_intangible() == true) {
                    //std::cout << "### #3 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].intangible[TRUE]" << std::endl;
                    continue;
                } else if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_shielded((*it).get_direction()) == true && (*it).get_trajectory() != TRAJECTORY_BOMB && (*it).get_trajectory() != TRAJECTORY_LIGHTING && (*it).get_trajectory() != TRAJECTORY_SLASH && (*it).get_vanishes_on_hit() == true) {
                    //std::cout << "### #4 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].shielded[TRUE]" << std::endl;
                    if ((*it).get_trajectory() == TRAJECTORY_CHAIN) {
                        (*it).consume_projectile();
                    } else {
                        (*it).reflect();    // SHIELD reflect
                    }
                    continue;
                }
                if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_invisible() == true) { // invisible NPC -> ignore shot
                    //std::cout << "### #5 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].invisible[TRUE]" << std::endl;
                    continue;
                }
                if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_teleporting() == true) { // executing AI-action TELEPORT
                    //std::cout << "### #6 - enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].teleporting[TRUE]" << std::endl;
                    continue;
                }

                // check if have hit area, and if hit it
                st_rectangle npc_vulnerable_area = GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).get_vulnerable_area();

                //std::cout << "### enemy[" << gameManager::get_instance()->get_current_map_obj()->_npc_list.at(i).get_name() << "].vulnerable_area[" << npc_vulnerable_area.x << "," << npc_vulnerable_area.y << "," << npc_vulnerable_area.w << "," << npc_vulnerable_area.h << "]" << std::endl;

                int temp_x = Sint16(npc_vulnerable_area.x-GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x);

                if (npc_vulnerable_area.is_empty() == false && npc_vulnerable_area != npc_hitbox && (*it).check_collision(npc_vulnerable_area, st_position(moved.width, moved.height)) == false) { // hit body, but not the hit area -> reflect

                    std::cout << "### MISS-ENEMY VULNERABLE-AREA - projectile.x[" << (*it).get_position().x << "], enemy.pos.x[" << GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().x << "], enemy.pos.y[" << GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().y << "]"  << std::endl;
                    std::cout << "### npc_vulnerable_area x[" << npc_vulnerable_area.x << "], y[" << npc_vulnerable_area.y << "], w[" << npc_vulnerable_area.w << "], h[" << npc_vulnerable_area.h << "]" << std::endl;
                    std::cout << "### npc_hitbox x[" << npc_hitbox.x << "], y[" << npc_hitbox.y << "], w[" << npc_hitbox.w << "], h[" << npc_hitbox.h << "]" << std::endl;

                    (*it).reflect();        // HITAREA reflect
                    continue;
                } else {
                    std::cout << "### HIT-ENEMY VULNERABLE-AREA - enemy.pos.x[" << GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().x << "], enemy.pos.y[" << GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).getPosition().y << "]";
                }

                short wpn_id = (*it).get_weapon_id();

                if (wpn_id < 0) {
                    wpn_id = 0;
                }

                // NPC using cicrcle weapon, is only be destroyed by CHAIN, but NPC won't take damage
                if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_using_circle_weapon() == true) {
                    if ((*it).get_trajectory() == TRAJECTORY_CHAIN) {
                        std::cout << "PROJ::END #3" << std::endl;
                        GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).consume_projectile();
                    }
                    std::cout << "PROJ::END #4" << std::endl;
                    (*it).consume_projectile();
                    return;
                }

                if ((*it).get_damage() > 0) {
                    int multiplier = 1;
                    GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).damage((*it).get_damage() * multiplier, ignore_hit_timer);
                } else {
                    std::cout << "PLAYER::EXECUTE_PROJ - projectile damage is zero" << std::endl;
                }
                if ((*it).get_damage() > 0) {
                    if ((*it).get_vanishes_on_hit() == true) {
                        (*it).consume_projectile();
                    }
                    SoundView::get_instance()->play_sfx(SFX_NPC_HIT);
                }
            }
        }


        // if projectile is a bomb, check collision against objects
        if ((*it).get_effect_n() == 1 && ((*it).get_move_type() == TRAJECTORY_BOMB || (*it).get_move_type() == TRAJECTORY_FALL_BOMB) || (*it).is_explosive() == true) {
            //std::cout << "PLAYER::execute_projectiles - Have exploding bomb, checking objects that collide..." << std::endl;
            std::vector<GameObject*> res_obj = GameManager::get_instance()->get_current_map_obj()->check_collision_with_objects((*it).get_area());
            if (res_obj.size() > 0) {
                //std::cout << "PLAYER::execute_projectiles - Found objects (" << res_obj.size() << ") that collides with bomb!" << std::endl;
                for (unsigned int i=0; i<res_obj.size(); i++) {
                    GameObject* temp_obj = res_obj.at(i);
                    //std::cout << "PLAYER::execute_projectiles - OBJ[" << temp_obj->get_name() << "].type: " << temp_obj->get_type() << ", OBJ_DESTRUCTIBLE_WALL: " << OBJ_DESTRUCTIBLE_WALL << std::endl;
                    if (temp_obj->get_type() == OBJ_DESTRUCTIBLE_WALL) {
                        //std::cout << "PLAYER::execute_projectiles - Found destructible block!!!!" << std::endl;
                        temp_obj->set_finished(true);
                        if ((*it).is_explosive() == true) {
                            (*it).transform_into_explosion();
                        }
                    }
                }
            }
        }
    }
    projectile_list.insert(projectile_list.end(), projectile_to_be_added_list.begin(), projectile_to_be_added_list.end());
    projectile_to_be_added_list.clear();
}

void classPlayer::move()
{
    if (InputController::get_instance()->p1_input[BTN_DOWN] == 1) {
		moveCommands.down = 1;
	} else {
		moveCommands.down = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_UP] == 1) {
		moveCommands.up = 1;
	} else {
		moveCommands.up = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_LEFT] == 1) {
		moveCommands.left = 1;
	} else {
		moveCommands.left = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_RIGHT] == 1) {
		moveCommands.right = 1;
	} else {
		moveCommands.right = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_JUMP] == 1) {
		moveCommands.jump = 1;
	} else {
		moveCommands.jump = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_ATTACK] == 1) {
		moveCommands.attack = 1;
	} else {
        //std::cout << ">>> moveCommands.attack::RESET #1" << std::endl;
		moveCommands.attack = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_ITEM] == 1) {
		moveCommands.use_item = 1;
        moveCommands.left = 0;
        moveCommands.right = 0;
        moveCommands.up = 0;
        moveCommands.down = 0;
	} else {
		moveCommands.use_item = 0;
	}
    if (InputController::get_instance()->p1_input[BTN_DASH] == 1) {
		moveCommands.dash = 1;
	} else {
		moveCommands.dash = 0;
	}

    // players that shoot on diagonal can't move shile attacking
    if (can_shoot_diagonal()) {
        if (is_on_attack_frame()) {
            if (state.animation_type != ANIM_TYPE_JUMP_ATTACK) {
                if (state.animation_type == ANIM_TYPE_WALK_ATTACK) {
                    set_animation_type(ANIM_TYPE_ATTACK);
                }
                moveCommands.left = 0;
                moveCommands.right = 0;
            }
        }
    }

    if (InputController::get_instance()->p1_input[BTN_L] != 1 && l_key_released == false) {
		l_key_released = true;
	}
    //wpn_max--;
    if (InputController::get_instance()->p1_input[BTN_L] == 1 && l_key_released == true) {
		l_key_released = false;
        //std::cout << ">>> LBUTTON - selected_weapon: " << selected_weapon << ", selected_weapon_c: " << selected_weapon_c << ", WEAPON_COUNT: " << WEAPON_COUNT << std::endl;
	}

    if (InputController::get_instance()->p1_input[BTN_R] != 1 && r_key_released == false) {
		r_key_released = true;
	}
    if (InputController::get_instance()->p1_input[BTN_R] == 1 && r_key_released == true) {
		r_key_released = false;
        //std::cout << ">>> RBUTTON - selected_weapon: " << selected_weapon << ", selected_weapon_c: " << selected_weapon_c << ", WEAPON_COUNT: " << WEAPON_COUNT << std::endl;
	}


	// send commands to the platform in special cases
	if (_platform != nullptr) {
        if (_platform->get_type() == OBJ_ITEM_FLY && TimerView::get_instance()->getTimer() > _platform->get_timer()) {
            _platform->set_timer(TimerView::get_instance()->getTimer()+240);
		}
		//std::cout << ">>> PLAYER SEND COMMAND FOR " << _platform->get_name() << ", type: " << _platform->get_type() << std::endl;
		if (moveCommands.up == 1) {
			_platform->command_up();
		}
		if (moveCommands.down == 1) {
			_platform->command_down();
		}
	}

    if (state.animation_type == ANIM_TYPE_STAND && moveCommands.jump == 0 && moveCommands.use_item == 1) {
        use_game_item();
		return;
	}
    execute_projectiles();
}


void classPlayer::set_teleporter(int n)
{
	//std::cout << "******** classPlayer::set_teleporter - current: " << teleporter_n << ", new: " << n << std::endl;
	teleporter_n = n;
}

int classPlayer::get_teleporter()
{
	return teleporter_n;
}



void classPlayer::death()
{
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->play_sfx(SFX_PLAYER_DEATH);
    GameManager::get_instance()->draw_player_death(relativePosition);

    GameManager::get_instance()->get_current_map_obj()->print_objects_number();
    reset_charging_shot();
    GameManager::get_instance()->get_current_map_obj()->clear_animations();
    GameManager::get_instance()->get_current_map_obj()->print_objects_number();
    GameManager::get_instance()->get_current_map_obj()->reset_objects();
    GameManager::get_instance()->get_current_map_obj()->print_objects_number();
	dead = true;
    _obj_jump.interrupt();
    _obj_jump.finish();

    last_hit_time = 0;

    selected_weapon = 0;
    clear_move_commands();
    InputController::get_instance()->clean();
    set_direction(ANIM_DIRECTION_RIGHT);
    GameManager::get_instance()->remove_current_teleporter_from_list();

    GameManager::get_instance()->draw_explosion(relativePosition, false);

}

void classPlayer::reset_hp()
{
	hitPoints.current = hitPoints.total;
}



void classPlayer::save_input()
{
    saved_move_commands = moveCommands;
    //std::cout << "PLAYER::save_input::ATTACK: " << moveCommands.attack << ", BTN-ATTACK: " << (int)saved_input[BTN_ATTACK] << std::endl;
}

void classPlayer::restore_input()
{
    //std::cout << "PLAYER::restore_input::OLD-ATTACK: " << saved_move_commands.attack << std::endl;
    moveCommands = saved_move_commands;
    //std::cout << "PLAYER::restore_input::ATTACK: " << moveCommands.attack << ", BTN-ATTACK: " << (int)InputController::get_instance()->p1_input[BTN_ATTACK] << std::endl;
}


void classPlayer::set_teleport_minimal_y(int y)
{
    //std::cout << "PLAYER::set_teleport_minimal_y[" << y << "]" << std::endl;
    _teleport_minimal_y = y-2;
}

bool classPlayer::can_fly()
{
    return false;
}





void classPlayer::clean_move_commands()
{
    std::cout << ">>> moveCommands.attack::RESET #2" << std::endl;
    moveCommands.attack = 0;
    moveCommands.dash = 0;
    moveCommands.down = 0;
    moveCommands.jump = 0;
    moveCommands.left = 0;
    moveCommands.right = 0;
    moveCommands.use_item = 0;
    moveCommands.start = 0;
    moveCommands.up = 0;
}

bool classPlayer::can_shoot_diagonal()
{
    if (GameData::get_instance()->player_list_v3_1[_number].can_shot_diagonal) {
        return true;
    }
    // armor-pieces checking


    return false;
}

bool classPlayer::can_double_jump()
{
    if (GameData::get_instance()->player_list_v3_1[_number].can_double_jump) {
        return true;
    }
    // -------------------- armor-pieces checking -------------------- //
    return false;
}

bool classPlayer::can_air_dash()
{
    if (GameData::get_instance()->player_list_v3_1[_number].can_air_dash == true) {
        return true;
    }

    return false;
}

void classPlayer::damage(unsigned int damage_points, bool ignore_hit_timer)
{
    int new_damage_points = damage_points;
    character::damage(damage_points, ignore_hit_timer);
}

void classPlayer::damage_spikes(bool ignore_hit_timer)
{
    classPlayer::damage(SPIKES_DAMAGE, ignore_hit_timer);
}






void classPlayer::reset_charging_shot()
{
    if (selected_weapon != -1) { // only do this, if using normal weapon
        return;
    }
    state.attack_timer = 0;
    attack_button_released = true;
    SoundView::get_instance()->stop_repeated_sfx();

    // also reset slide/dash
    if (state.animation_type == ANIM_TYPE_SLIDE) {
        set_animation_type(ANIM_TYPE_WALK);
        state.slide_distance = 0;
    }
}

bool classPlayer::is_teleporting()
{
    if (state.animation_type == ANIM_TYPE_TELEPORT) {
        return true;
    }
    return false;
}


