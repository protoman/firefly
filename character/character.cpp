#include "character.h"

#include <vector>
#include <math.h>

#include "GameManager.h"
#include "game_data.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#define STAIR_ANIMATION_WAIT_FRAMES 10
#define STAIRS_GRAB_TIMEOUT 200
#define JUMP_X_SPEED_MULTIPLIER 1.2

#include "data/shareddata.h"
#include "data/SharedPlayerData.hpp"

#include "view/imageview.h"
#include "view/soundview.h"
#include "view/timerview.h"
#include "view/draw.h"

#include "GameManager.h"

#include "objects/GameObject.h"

// initialize static member
static std::map<std::string, st_imageData> _character_frames_surface;

// init character with default values
// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
character::character() : hitPoints(1, 1), last_hit_time(0), is_player_type(false), _platform(nullptr), hit_animation_timer(0), hit_moved_back_n(0), jump_button_released(true), attack_button_released(true), dead(false), charging_color_n(0), charging_color_timer(0), shield_type(0), _moving_platform_timer(0), position(), _number(0), _super_jump(false), _force_jump(false), _teleport_minimal_y(0), _is_falling(false), _dead_state(0), _water_splash(false), _has_background(false), hit_duration(300), _is_boss(false), _is_stage_boss(false), is_ghost(false)
{
    _was_animation_reset = false;
    move_speed = 2.0;

    accel_speed_y = 1;
    gravity_y = 0.25;
	position.y = 0;
	position.x = 0;
    can_fly = false;
	attack_state = ATTACK_NOT;
	max_projectiles = 1;
    _debug_char_name = "MUMMY BOT";
    _stairs_stopped_count = 0;
    _charged_shot_projectile_id = -1;
    _normal_shot_projectile_id = 0;
    _is_last_frame = false;
    _simultaneous_shots = 1;
    _ignore_gravity = false;
    _always_move_ahead = false;
    _was_hit = false;
    _progressive_appear_pos = 0;
    last_execute_time = 0;
    _check_always_move_ahead = false;
    _dropped_from_stairs = false;
    _jumps_number = 1;
    _dash_button_released = true;
    _damage_modifier = 0;
    _hit_ground = false;
    _dashed_jump = false;
    _can_execute_airdash = true;
    _player_must_reset_colors = false;
    hit_animation_count = 0;
    _attack_frame_n = -1;
    _is_attack_frame = false;
    _stairs_falling_timer = 0;
    attack_button_pressed_timer = 0;
    attack_button_last_state = 0;
    must_show_dash_effect = false;
}


// init character with default values
// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
character::~character()
{
	clean_projectiles();
	/// @TODO free map _character_frames_surface
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::char_update_real_position() {
    if (GameManager::get_instance()->get_current_map_obj() != nullptr) {
        relativePosition.x = position.x - (int)GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
        relativePosition.y = position.y - (int)GameManager::get_instance()->get_current_map_obj()->getMapScrolling().y;
        //std::cout << ">>>> show::char_update_real_position - realPosition.y: " << realPosition.y << ", pos.y: " << position.y << ", gameManager::get_instance()->get_current_map_obj()->getMapScrolling().y: " << GameManager::get_instance()->get_current_map_obj()->getMapScrolling().y << std::endl;
    } else {
        relativePosition.x = position.x;
        relativePosition.y = position.y;
    }
}

st_float_position character::get_screen_position_from_point(st_float_position pos)
{
    st_float_position res_pos;
    if (GameManager::get_instance()->get_current_map_obj() != nullptr) {
        res_pos.x = pos.x - (int)GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
        res_pos.y = pos.y - (int)GameManager::get_instance()->get_current_map_obj()->getMapScrolling().y;
        //std::cout << ">>>> show::char_update_real_position - realPosition.y: " << realPosition.y << ", pos.y: " << position.y << ", gameManager::get_instance()->get_current_map_obj()->getMapScrolling().y: " << gameManager::get_instance()->get_current_map_obj()->getMapScrolling().y << std::endl;
    } else {
        res_pos.x = pos.x;
        res_pos.y = pos.y;
    }
    return res_pos;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::charMove() {

    int mapLock = 0;
	bool moved = false;
    float temp_move_speed = move_speed;

    // store previous position
    store_previous_position();

    if (TimerView::get_instance()->is_paused() == true) {
        return;
    }

    check_y_scroll();

    //std::cout << "CHAR::MOVE - py[" << position.y << "], real.y[" << realPosition.y << "]" << std::endl;

    bool did_hit_ground = hit_ground();

    if (_dashed_jump == true) {
        if (state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) {
            temp_move_speed = move_speed * 2;
            if (did_hit_ground == true) {
                _dashed_jump = false;
            }
        }
    }
    if (did_hit_ground == true) {
        _can_execute_airdash = true;
    }



    //if (is_player()) std::cout << "CHAR::CHARMOVE - _fractional_move_speed: " << _fractional_move_speed << std::endl;


    if (GameManager::get_instance()->get_current_map_obj() == nullptr) {
        std::cout << "# CHAR::MOVE::NO-MAP" << std::endl;
		return; // error - can't execute this action without an associated map
	}

    int water_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position((position.x+frameSize.width/2)/TILESIZE, (position.y+6)/TILESIZE));
    if (is_player() == true && water_lock == TERRAIN_WATER) {
        GameManager::get_instance()->get_current_map_obj()->add_bubble_animation(st_position(relativePosition.x+frameSize.width/2, position.y+6));
        if (TimerView::get_instance()->getTimer() > last_water_damage_warning_sound_timer) {
            SoundView::get_instance()->play_sfx(SFX_BEAM);
            last_water_damage_warning_sound_timer = TimerView::get_instance()->getTimer() + 1200;
        }
        damage(2, false);
    }

    int map_point_x = (position.x+frameSize.width/2)/TILESIZE;
    int map_point_y = (position.y+frameSize.height)/TILESIZE;
    int bottom_point_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(map_point_x, map_point_y));
    //std::cout << "map.x: " << map_point_x << ", map.y: " << map_point_y << ", bottom_point_lock: " << bottom_point_lock << std::endl;

	if (state.animation_type == ANIM_TYPE_TELEPORT) {
        std::cout << "OUT.TELEPORT" << std::endl;
		gravity(false);
		return;
	}


    if (state.animation_type == ANIM_TYPE_HIT) {
        //std::cout << "OUT.HIT" << std::endl;

        //if (is_player()) std::cout << "hit_moved_back_n: " << hit_moved_back_n << ", get_hit_push_back_n(): " << get_hit_push_back_n() << std::endl;

        if (hit_moved_back_n < get_hit_push_back_n()) {
            //std::cout << ">>>>>>>>>>>>> ANIM_TYPE_HIT::PUSHBACK #2" << std::endl;

            if (state.direction == ANIM_DIRECTION_LEFT) {
                moveCommands.left = 0;
                moveCommands.right = 1;
            } else {
                moveCommands.left = 1;
                moveCommands.right = 0;
            }
            moveCommands.up = 0;
            moveCommands.down = 0;
            moveCommands.jump = 0;
            if (did_hit_ground == true) {
                temp_move_speed = 0.5;
            } else {
                temp_move_speed = 0.8;
            }

        } else {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #Z" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
			hit_moved_back_n = 0;
		}
	}

    if (is_player() && moveCommands.jump == 1 && moveCommands.down == 1 && bottom_point_lock == TERRAIN_PLATFORM) {
        int player_bottom_y = get_hitbox().y + get_hitbox().h;
        int exact_player_y = map_point_y * TILESIZE;
        int player_y_diff = player_bottom_y - exact_player_y;
        if (player_y_diff < TILESIZE/4) {
            position.y+= TILESIZE/4;
            _obj_jump.interrupt();
            return;
        }
    }

    if (moveCommands.left == 1 && position.x > -TILESIZE && state.animation_type != ANIM_TYPE_SLIDE && is_in_stairs_frame() == false) {
        // check inverting direction
        if (state.animation_type != ANIM_TYPE_HIT && state.direction != ANIM_DIRECTION_LEFT) {
            set_direction(ANIM_DIRECTION_LEFT);
            return;
        }

        if (state.animation_type == ANIM_TYPE_JUMP) {
            temp_move_speed = temp_move_speed*JUMP_X_SPEED_MULTIPLIER;
        }

        if (is_on_quicksand()) {
            temp_move_speed = QUICKSAND_JUMP_LIMIT/2;
        }

        float minimal_speed = 0.1f;
        {
            float test_slope_angle = slopesManager.get_current_slope_angle();
            if (test_slope_angle > SLOPE_ANGLE_THRESHOLD) {
                minimal_speed = slopesManager.calculate_minimal_speed(test_slope_angle, temp_move_speed);
            }
        }

        for (float i=temp_move_speed; i>=minimal_speed; i--) {
            st_map_collision map_col = map_collision(-i, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
            mapLock = map_col.block;
            if (state.animation_type == ANIM_TYPE_HIT) {
                hit_moved_back_n += temp_move_speed;
            }
            //if (is_player()) std::cout << "### MOVE::LEFT - mapLock[" << mapLock << "] ###" << std::endl;
            bool is_on_slope = executeCheckSlope(i, 0);
            if (is_on_slope == true || mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                if (is_on_slope == true || mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_Y) {
                    position.x -= i + GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                    moved_dist.x -= i + GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                } else if (mapLock == BLOCK_WATER) {
                    position.x -= i*WATER_SPEED_MULT + GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                    moved_dist.x -= i*WATER_SPEED_MULT + GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
				}
				if (state.animation_type != ANIM_TYPE_HIT) {
                    if (is_player() && state.direction == ANIM_DIRECTION_LEFT && is_in_stairs_frame() != true) {
                        //if (is_player()) std::cout << "### MOVE::LEFT - MOVED[" << PLAYER_LEFT_TO_RIGHT_DIFF << "] ###" << std::endl;
                        position.x -= PLAYER_LEFT_TO_RIGHT_DIFF;
                        moved_dist.x -= PLAYER_LEFT_TO_RIGHT_DIFF;
                    }
                    set_direction(ANIM_DIRECTION_LEFT);
				} else {
                    gravity(false);
                    return;
				}
                if (state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_WALK_ATTACK) {
					state.animation_timer = 0;
                }
                if (state.animation_type != ANIM_TYPE_GOT_ITEM && state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_JUMP && state.animation_type != ANIM_TYPE_SLIDE && state.animation_type != ANIM_TYPE_JUMP_ATTACK && state.animation_type != ANIM_TYPE_HIT && (state.animation_type != ANIM_TYPE_WALK_ATTACK || (state.animation_type == ANIM_TYPE_WALK_ATTACK && state.attack_timer+ATTACK_DELAY < TimerView::get_instance()->getTimer()))) {
                    set_animation_type(ANIM_TYPE_WALK);
				}
				moved = true;
				break;
			}
        }
	}


    if (moveCommands.right != 1 && moveCommands.left == 1 && state.direction != ANIM_DIRECTION_LEFT && (state.animation_type == ANIM_TYPE_SLIDE || is_in_stairs_frame() == true)) {
        if (is_player() && state.direction == ANIM_DIRECTION_RIGHT && is_in_stairs_frame() != true) {
            position.x -= PLAYER_RIGHT_TO_LEFT_DIFF;
            moved_dist.x -= PLAYER_RIGHT_TO_LEFT_DIFF;
        }
        set_direction(ANIM_DIRECTION_LEFT);
    }


    if (moveCommands.left != 1 && moveCommands.right == 1 && state.animation_type != ANIM_TYPE_SLIDE && is_in_stairs_frame() == false) {

        if (state.animation_type != ANIM_TYPE_HIT && state.direction != ANIM_DIRECTION_RIGHT) {
            set_direction(ANIM_DIRECTION_RIGHT);
            return;
        }
        if (state.animation_type == ANIM_TYPE_HIT) {
            hit_moved_back_n += temp_move_speed;
        }

        if (state.animation_type == ANIM_TYPE_JUMP) {
            temp_move_speed = temp_move_speed*JUMP_X_SPEED_MULTIPLIER;
        }

        if (state.animation_type == ANIM_TYPE_HIT) {
            hit_moved_back_n += temp_move_speed;
        }

        if (is_on_quicksand()) {
            temp_move_speed = temp_move_speed/4;
        }

        //std::cout << "### MOVE::RIGHT#1 temp_move_speed[" << temp_move_speed << "] ###" << std::endl;
        float minimal_speed = 0.1f;
        {
            float test_slope_angle = slopesManager.get_current_slope_angle();
            if (test_slope_angle > SLOPE_ANGLE_THRESHOLD) {
                minimal_speed = slopesManager.calculate_minimal_speed(test_slope_angle, temp_move_speed);
            }
        }
        for (float i=temp_move_speed; i>=minimal_speed; i--) {
            // movement is too small to change a pixel in player movement, ignore it
            int adjusted_real_pos = (int)(relativePosition.x + i);
            int real_pos = (int)relativePosition.x;
            //std::cout << "adjusted_real_pos[" << adjusted_real_pos << "], real_pos[" << real_pos << "]" << std::endl;

            if (adjusted_real_pos == real_pos) {
                break;
            }
            bool is_on_slope = executeCheckSlope(i, 0);

            if (is_player() == false || (relativePosition.x + i + frameSize.width/2) < RES_W) {
                st_map_collision map_col = map_collision(i, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                mapLock = map_col.block;
                //std::cout << "### MOVE::RIGHT#2 - is_on_slope[" << is_on_slope << "] ###" << std::endl;
                if (is_on_slope == true || mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                    //std::cout << "CHAR::MOVE-RIGHT #3 mapLock[" << mapLock << "], pos.x[" << position.x << "]" << std::endl;

                    if (is_on_slope == true || mapLock == TERRAIN_UNBLOCKED || mapLock == BLOCK_Y) {
                        position.x += i - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                        moved_dist.x += i - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                    } else if (mapLock == BLOCK_WATER) {
                        position.x += i*WATER_SPEED_MULT - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                        moved_dist.x += i*WATER_SPEED_MULT - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                    }
                    if (state.animation_type != ANIM_TYPE_HIT) {
                        if (is_player() && state.direction == ANIM_DIRECTION_RIGHT && is_in_stairs_frame() != true) {
                            //if (is_player()) std::cout << "### MOVE::RIGHT - MOVED[" << PLAYER_RIGHT_TO_LEFT_DIFF << "] ###" << std::endl;
                            position.x += PLAYER_RIGHT_TO_LEFT_DIFF;
                            moved_dist.x += PLAYER_RIGHT_TO_LEFT_DIFF;
                        }
                        set_direction(ANIM_DIRECTION_RIGHT);
                    } else {
                        gravity(false);
                        return;
                    }
                    if (state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_WALK_ATTACK) {
                        state.animation_timer = 0;
                    }
                    if (state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_JUMP && state.animation_type != ANIM_TYPE_SLIDE && state.animation_type != ANIM_TYPE_JUMP_ATTACK && state.animation_type != ANIM_TYPE_HIT && (state.animation_type != ANIM_TYPE_WALK_ATTACK || (state.animation_type == ANIM_TYPE_WALK_ATTACK && state.attack_timer+ATTACK_DELAY < TimerView::get_instance()->getTimer()))) {
                        set_animation_type(ANIM_TYPE_WALK);
                    }
                    moved = true;
                    break;
                } else {
                    moved = false;
                }
            }
		}
	}
	if (moveCommands.right == 1 && state.direction != ANIM_DIRECTION_RIGHT && (state.animation_type == ANIM_TYPE_SLIDE || is_in_stairs_frame() == true)) {
        if (is_player() && state.direction == ANIM_DIRECTION_LEFT && is_in_stairs_frame() != true) {
            position.x += PLAYER_RIGHT_TO_LEFT_DIFF;
            moved_dist.x += PLAYER_RIGHT_TO_LEFT_DIFF;
        }
        set_direction(ANIM_DIRECTION_RIGHT);
    }


    // Ice inertia
    if (bottom_point_lock == TERRAIN_ICE || (_inertia_obj.is_started() && (bottom_point_lock == TERRAIN_UNBLOCKED|| bottom_point_lock == TERRAIN_WATER))) {

        if (moved == true) {
            if (moveCommands.right == 1) {
                st_map_collision map_col = map_collision(1, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                mapLock = map_col.block;
                if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                    position.x++;
                }
            } else if (moveCommands.left == 1) {
                st_map_collision map_col = map_collision(-1, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                mapLock = map_col.block;
                if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                    position.x--;
                }
            }
            _inertia_obj.start();
        } else if (moveCommands.left == 0 && moveCommands.right == 0) {
            int inertia_xinc = _inertia_obj.execute();
            if (inertia_xinc != 0) {
                if (state.direction == ANIM_DIRECTION_LEFT) {
                    if (position.x - inertia_xinc < 0) {
                        _inertia_obj.stop();
                    } else {
                        st_map_collision map_col = map_collision(-inertia_xinc, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                        mapLock = map_col.block;
                        if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                            //std::cout << "inertia CONTINUE, pos.x: " << position.x << ", mapLock: " << mapLock << std::endl;
                            position.x -= inertia_xinc;
                            moved_dist.x -= inertia_xinc;
                        //} else {
                            //std::cout << "inertia DONT EXECUTE, pos.x: " << position.x << ", mapLock: " << mapLock << std::endl;
                        }
                    }
                } else {
                    if (relativePosition.x+inertia_xinc > RES_W) {
                        //std::cout << "INERTIA::STOP #2" << std::endl;
                        _inertia_obj.stop();
                    } else {
                        st_map_collision map_col = map_collision(inertia_xinc, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                        mapLock = map_col.block;
                        if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
                            //std::cout << "inertia CONTINUE, pos.x: " << position.x << ", mapLock: " << mapLock << std::endl;
                            position.x += inertia_xinc;
                            moved_dist.x += inertia_xinc;
                        //} else {
                            //std::cout << "inertia DONT EXECUTE, pos.x: " << position.x << ", mapLock: " << mapLock << std::endl;
                        }
                    }
                }
            }
        }
        if (GameManager::get_instance()->get_current_map_obj() != nullptr) {
            GameManager::get_instance()->update_stage_scrolling();
        }
        char_update_real_position();
    } else if (bottom_point_lock != TERRAIN_WATER) {
        _inertia_obj.stop();
    }

    if (_obj_jump.is_started() == true) {
        _inertia_obj.stop();
    }


	// check if character is on stairs
    if (moveCommands.up == 1 && state.animation_type != ANIM_TYPE_STAIRS_ATTACK) { // check stairs on middle
        st_position stairs_pos = is_on_stairs(st_rectangle(position.x, position.y+(frameSize.height/2)-2, frameSize.width, frameSize.height/2-2));
        int top_terrain = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(((stairs_pos.x * TILESIZE - 6)+frameSize.width/2)/TILESIZE, position.y/TILESIZE));
        if (stairs_pos.x != -1) {
            if (state.animation_type != ANIM_TYPE_STAIRS_MOVE && _stairs_falling_timer < TimerView::get_instance()->getTimer()) {
                set_animation_type(ANIM_TYPE_STAIRS_MOVE);
			}
            if (is_in_stairs_frame() && (top_terrain == TERRAIN_UNBLOCKED || top_terrain == TERRAIN_WATER || top_terrain == TERRAIN_STAIR)) {
                position.y -= temp_move_speed * STAIRS_MOVE_MULTIPLIER;
                //std::cout << "<<<<<<<<<<< POS.X.SET #1 >>>>>>>>>>>>>" << std::endl;
                position.x = stairs_pos.x * TILESIZE - 6;
            }
		// out of stairs
		} else {
            int map_terrain = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(((position.x+frameSize.width/2)/TILESIZE), ((position.y+frameSize.height-4)/TILESIZE)));
            //std::cout << ">> map_terrain: " << map_terrain << ", _dropped_from_stairs: " << _dropped_from_stairs << std::endl;
            if (_dropped_from_stairs == false && map_terrain == TERRAIN_STAIR) { // check stairs bottom (leaving)
                //if (is_player()) std::cout << "STAIRS SEMI - SET #1" << std::endl;
                set_animation_type(ANIM_TYPE_STAIRS_SEMI);
                position.y -= temp_move_speed * STAIRS_MOVE_MULTIPLIER;
			} else if (state.animation_type == ANIM_TYPE_STAIRS_SEMI) {
                //if (is_player()) std::cout << "CHAR::RESET_TO_STAND #A" << std::endl;
                set_animation_type(ANIM_TYPE_STAND);
                //std::cout << "LEAVE STAIRS (BOTTOM->UP)" << std::endl;
                position.y -= 2;
			}
		}
    }




    if (moveCommands.down == 1 && state.animation_type != ANIM_TYPE_SLIDE && state.animation_type != ANIM_TYPE_STAIRS_ATTACK) {
        st_position stairs_pos_center = is_on_stairs(st_rectangle(position.x, position.y+frameSize.height/2, frameSize.width, frameSize.height/2));
        bool is_already_on_stairs = is_in_stairs_frame();
        /// @TODO - check that move-speed/2 is not zero

        // is on stairs
        if (is_already_on_stairs == true) {
            // if frame is semi, but already entered whole body, change to full-stairs frame
            if (state.animation_type == ANIM_TYPE_STAIRS_SEMI && stairs_pos_center.x != -1 && _stairs_falling_timer < TimerView::get_instance()->getTimer()) {
                //std::cout << "STAIRS *MOVE* - SET #2" << std::endl;
                set_animation_type(ANIM_TYPE_STAIRS_MOVE);
            }

            // check that path is clear to move
            if (is_in_stairs_frame() && (bottom_point_lock == TERRAIN_WATER || bottom_point_lock == TERRAIN_UNBLOCKED || bottom_point_lock == TERRAIN_STAIR)) {
                position.y += temp_move_speed * STAIRS_MOVE_MULTIPLIER;
            }

            // if bottom point is not stairs, leave it
            if (bottom_point_lock != TERRAIN_STAIR) {
                if (stairs_pos_center.x == -1 && (bottom_point_lock == TERRAIN_UNBLOCKED || bottom_point_lock == TERRAIN_WATER)) {
                    if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A" << std::endl;
                    set_animation_type(ANIM_TYPE_JUMP);
                }
                if (bottom_point_lock != TERRAIN_UNBLOCKED && bottom_point_lock != TERRAIN_WATER) {
                    if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #B, bottom_point_lock[" << bottom_point_lock << "]" << std::endl;
                    set_animation_type(ANIM_TYPE_STAND);
                }
            }
        // not in stairs, but over it
        } else {
            if (stairs_pos_center.x == -1 && bottom_point_lock == TERRAIN_STAIR) {
                // over stairs, enter it
                st_position stairs_pos_bottom = is_on_stairs(st_rectangle(position.x, position.y+frameSize.height, frameSize.width, frameSize.height/2));
                if (stairs_pos_bottom.x != -1) {
                    //std::cout << "STAIRS SEMI - SET #2" << std::endl;
                    set_animation_type(ANIM_TYPE_STAIRS_SEMI);

                    //std::cout << "### STAIRS-DOWN #2 ###" << std::endl;
                    position.y += temp_move_speed * STAIRS_MOVE_MULTIPLIER;
                    //std::cout << "<<<<<<<<<<< POS.X.SET #2 >>>>>>>>>>>>>" << std::endl;
                    position.x = stairs_pos_bottom.x * TILESIZE - 6;
                }
            }
        }

    }


	// is on stairs without moving
	if (moveCommands.down == 0 && moveCommands.up == 0 && state.animation_type == ANIM_TYPE_STAIRS_MOVE) {
        _stairs_stopped_count++;
        if (_stairs_stopped_count > STAIR_ANIMATION_WAIT_FRAMES) {
            set_animation_type(ANIM_TYPE_STAIRS);
        }
    } else if ((moveCommands.down != 0 || moveCommands.up != 0) && _stairs_falling_timer < TimerView::get_instance()->getTimer()) {
        _stairs_stopped_count = 0;
        if (state.animation_type == ANIM_TYPE_STAIRS) {
            //std::cout << "STAIRS *MOVE* - SET #3" << std::endl;
            set_animation_type(ANIM_TYPE_STAIRS_MOVE);
        }
    }

    check_reset_stand();

    if (is_player() == false) {
        character::attack(false, 0, false);
    }


    bool res_slide = slide(GameManager::get_instance()->get_current_map_obj()->getMapScrolling());

    bool resJump = false;
    resJump = jump(moveCommands.jump, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    if (resJump == true || res_slide == true) {
        if (state.animation_type == ANIM_TYPE_HIT) {
            gravity(false);
        }
    /// @TODO: removed a gravity from here in an ELSE. Hope it was not necessary
	}

    if (SharedData::get_instance()->must_interrupt_character_execution == true) {
        std::cout << "CHARMOVE::INTERRUPT-EXECUTION" << std::endl;
        return;
    }

    if (is_player_type && moved == false && resJump == false && res_slide == false) {
		//if (state.animation_type != ANIM_TYPE_WALK) {
         if (is_in_stairs_frame() == false && state.animation_type != ANIM_TYPE_STAND && state.animation_type != ANIM_TYPE_JUMP && state.animation_type != ANIM_TYPE_JUMP_ATTACK && state.animation_type != ANIM_TYPE_TELEPORT && state.animation_type != ANIM_TYPE_SHIELD && state.animation_type != ANIM_TYPE_TELEPORT && state.animation_type != ANIM_TYPE_HIT && state.animation_type != ANIM_TYPE_SLIDE && (is_on_attack_frame() == false || (is_on_attack_frame() == true && state.attack_timer+ATTACK_DELAY < TimerView::get_instance()->getTimer()))) {
			//if (is_player()) std::cout << "********* reset to stand - on_stairs_frame: " << is_in_stairs_frame() << ", state.animation_type: " << state.animation_type << std::endl;
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #D" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
		}
		//std::cout << "charMove - ANIM_TYPE_HIT: " << ANIM_TYPE_HIT << ", animation_type: " << state.animation_type << ", TimerView::get_instance()->getTimer(): " << TimerView::get_instance()->getTimer() << ", hit_duration+last_hit_time: " << hit_duration+last_hit_time << std::endl;
		if (state.animation_type == ANIM_TYPE_HIT && TimerView::get_instance()->getTimer() > hit_duration/2+last_hit_time) { // finished hit time
			//if (is_player()) std::cout << "state.animation_type SET to STAND " << std::endl;
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #E" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
		}
	}

    //if (is_player()) std::cout << "CHAR::MOVE - pos.x: " << position.x << ", pos.y: " << position.y << std::endl;

    if (_dropped_from_stairs == true) {
        if (TimerView::get_instance()->getTimer() > hit_duration+last_hit_time) {
            std::cout << "RESET _dropped_from_stairs flag #1" << std::endl;
            _dropped_from_stairs = false;
        } else if (hit_ground() == true) {
            std::cout << "RESET _dropped_from_stairs flag #2" << std::endl;
            _dropped_from_stairs = false;
        }
    }


    GameManager::get_instance()->get_current_map_obj()->reset_scrolled();
    _previous_position = position;


}


void character::check_y_scroll()
{

    if (is_player()) {
        //std::cout << "relativePosition.y[" << relativePosition.y << "], pos.y[" << position.y << "]" << std::endl;
        // BOTTOM //
        if (relativePosition.y+frameSize.height > AREA_H*0.8) {
            int diff_y = relativePosition.y+frameSize.height - AREA_H*0.9;
            if (diff_y > 0) {
                GameManager::get_instance()->get_current_map_obj()->changeScrolling(st_float_position(0, diff_y), true);
            }
        // TOP //
        } else if (relativePosition.y < AREA_H*0.3) {
            int diff_y = AREA_H*0.3 - relativePosition.y;
            if (relativePosition.y > 0 && relativePosition.y < TILESIZE) {
                diff_y = relativePosition.y;
            }
            if (diff_y > 0) {
                //std::cout << "character::check_y_scroll - diffY[" << diffY << "], realPosition.y[" << realPosition.y << "], AREA_H*0.3[" << (AREA_H*0.3) << "]" << std::endl;
                GameManager::get_instance()->get_current_map_obj()->changeScrolling(st_float_position(0, -diff_y), true);
            }
        }
    }
}

void character::store_previous_position()
{
    previous_position_list.push_back(position);
    if (previous_position_list.size() > PREVIOUS_FRAMES_MAX) {
        previous_position_list.erase(previous_position_list.begin());
    }
}



void character::clear_move_commands()
{
	moveCommands.up = 0;
	moveCommands.down = 0;
	moveCommands.left = 0;
	moveCommands.right = 0;
    //std::cout << ">>> moveCommands.attack::RESET #3" << std::endl;
	moveCommands.attack = 0;
	moveCommands.jump = 0;
    moveCommands.start = 0;
}



// return 0 if must not attack, 1 for normal attack, 2 for semi-charged and 3 for fully charged
ATTACK_TYPES character::check_must_attack(bool always_charged)
{

    // capture button timer even if can't shoot, so we avoid always charging
    if (moveCommands.attack != 0 && attack_button_last_state == 0) {
        attack_button_pressed_timer = TimerView::get_instance()->getTimer();
    }

    if (TimerView::get_instance()->is_paused()) {
        return ATTACK_TYPE_NOATTACK;
    }

    if (state.animation_type == ANIM_TYPE_TELEPORT) {
        return ATTACK_TYPE_NOATTACK;
    }
    if (ImageView::get_instance()->character_graphics_list.find(name) == ImageView::get_instance()->character_graphics_list.end()) {
        std::cout << "CHAR::ATTACK::ERROR: could not find character graphics!" << std::endl;
        return ATTACK_TYPE_NOATTACK;
    }

    if (state.animation_type == ANIM_TYPE_SLIDE) {
        //std::cout << "character::attack - LEAVE #2" << std::endl;
        return ATTACK_TYPE_NOATTACK;
    }

    if (max_projectiles <= get_projectile_count()) {
        return ATTACK_TYPE_NOATTACK;
    }

    if (is_player() == true && get_projectile_max_shots(always_charged) <= projectile_list.size()) {
        return ATTACK_TYPE_NOATTACK;
    }

    int now_timer = TimerView::get_instance()->getTimer();
    int time_diff = now_timer - attack_button_pressed_timer;

    if (SharedData::get_instance()->game_config.turbo_mode == true && moveCommands.attack != 0) {
        if (now_timer < state.attack_timer + TURBO_ATTACK_INTERVAL) {
            return ATTACK_TYPE_NOATTACK;
        } else {
            return ATTACK_TYPE_NORMAL;
        }
    }

    // button changed from released to pressed
    if (moveCommands.attack != 0 && attack_button_last_state == 0) {
        //std::cout << "CHAR::check_must_attack - ATTACK NORMAL" << std::endl;
        return ATTACK_TYPE_NORMAL;
    // button changed from pressed to released and char can use charged attacks
    } else if (SharedData::get_instance()->game_config.turbo_mode == false && _charged_shot_projectile_id > 0 && moveCommands.attack == 0 && attack_button_last_state == 1) {
        // @TODO use super charged time also
        if (time_diff >= CHARGED_SHOT_TIME) {
            return ATTACK_TYPE_FULLYCHARGED;
        } else if (time_diff >= CHARGED_SHOT_INITIAL_TIME) {
            std::cout << "time_diff: " << time_diff << std::endl;
            return ATTACK_TYPE_SEMICHARGED;
        }
    }
    return ATTACK_TYPE_NOATTACK;
}

void character::check_charging_colors(bool always_charged)
{
    // change player colors if charging attack
    int now_timer = TimerView::get_instance()->getTimer();
    int attack_diff_timer = now_timer-attack_button_pressed_timer;

    if (SharedData::get_instance()->game_config.turbo_mode == true) {
        return;
    }

    // don't charge if can't shot
    if (max_projectiles <= get_projectile_count()) {
        // reset time, so we start counting only when all projectiles are gone
        return;
    }

    if (is_player() == true && get_projectile_max_shots(always_charged) <= projectile_list.size()) {
        // reset time, so we start counting only when all projectiles are gone
        return;
    }


    //std::cout << "_charged_shot_projectile_id[" << _charged_shot_projectile_id << "]" << std::endl;

    if (_charged_shot_projectile_id > 0 && attack_diff_timer > CHARGED_SHOT_INITIAL_TIME && attack_diff_timer < CHARGED_SHOT_TIME && attack_button_last_state == 1 && moveCommands.attack == 1 && _simultaneous_shots < 2) {
        if (is_player() && SoundView::get_instance()->is_playing_repeated_sfx() == false) {
            SoundView::get_instance()->play_repeated_sfx(SFX_CHARGING1, 0);
        }
        if (color_keys[0].r != -1) {
            if (charging_color_timer < TimerView::get_instance()->getTimer()) {
                charging_color_n++;
                if (charging_color_n > 2) {
                    charging_color_n = 0;
                }
                charging_color_timer = TimerView::get_instance()->getTimer()+200;
            }
        }
    }
    if (_charged_shot_projectile_id > 0 && is_player() && attack_diff_timer >= CHARGED_SHOT_TIME && attack_button_last_state == 1 && moveCommands.attack == 1) {
        if (SoundView::get_instance()->is_playing_repeated_sfx() == true && SoundView::get_instance()->get_repeated_sfx_n() == SFX_CHARGING1) {
            SoundView::get_instance()->stop_repeated_sfx();
            SoundView::get_instance()->play_repeated_sfx(SFX_CHARGING2, 255);
        }
        if (color_keys[0].r != -1) {
            if (charging_color_timer < TimerView::get_instance()->getTimer()) {
                charging_color_n++;
                if (charging_color_n > 2) {
                    charging_color_n = 0;
                }
                charging_color_timer = TimerView::get_instance()->getTimer()+100;
            }
        }
    }
}




st_position character::get_attack_position()
{
    return get_attack_position(state.direction);
}


st_position character::get_attack_position(short direction)
{
    st_position proj_pos;
    if (direction == ANIM_DIRECTION_LEFT) {
        proj_pos = st_position(position.x+TILESIZE/3, position.y+frameSize.height/2);
    } else {
        proj_pos = st_position(position.x+frameSize.width-TILESIZE/2, position.y+frameSize.height/2);
    }
    if (is_player() == false) {
        st_position_int8 attack_arm_pos = GameData::get_instance()->get_enemy(_number)->attack_arm_pos;
        if (attack_arm_pos.x >= 1 || attack_arm_pos.y >= 1) {
            if (direction == ANIM_DIRECTION_LEFT) {
                proj_pos = st_position(position.x + attack_arm_pos.x, position.y + attack_arm_pos.y);
            } else {
                proj_pos = st_position(position.x + frameSize.width - attack_arm_pos.x, position.y + attack_arm_pos.y);
            }
        }
    }
    return proj_pos;

}

st_float_position character::get_last_moved()
{
    return moved_dist;
}

void character::pick_game_item(GameObject &obj_info)
{
    if (is_player() == false) {
        return;
    }
    std::cout << "character::pick_item::START" << std::endl;
    std::cout << "character::pick_item[" << (int)obj_info.get_id() << ", '" << obj_info.get_name() << "']" << std::endl;
    bool picked_item = false;
    for (int i=0; i<GAME_ITEM_SLOTS; i++) {
        if (SharedData::get_instance()->game_save.game_item_list[i].uuid == -1) {
            std::cout << "picked item in slot[" << i << "]" << std::endl;
            SharedData::get_instance()->game_save.game_item_list[i].obj_id = obj_info.get_id();
            SharedData::get_instance()->game_save.game_item_list[i].uuid = obj_info.get_uuid();
            obj_info.set_finished(true);
            picked_item = true;
            InputController::get_instance()->p1_input[BTN_ITEM] = 0;
            moveCommands.use_item = 0;
            break;
        }
    }
    std::cout << "character::pick_item::END" << std::endl;

}

void character::use_game_item()
{
    // if still have an empty slot, the player
    // can't use an item on another one, will try to pick instead
    if (is_on_game_item_area == true) {
        bool has_slot = false;
        for (int i=0; i<GAME_ITEM_SLOTS; i++) {
            if (SharedData::get_instance()->game_save.game_item_list[i].uuid == -1) {
                has_slot = true;
            }
        }
        if (has_slot == true) {
            return;
        }
    }

    std::cout << "character::use_game_item::START" << std::endl;
    if (SharedData::get_instance()->game_save.game_item_list[0].uuid != -1) {
        std::cout << "picked_item[FALSE]" << std::endl;
        GameManager::get_instance()->get_current_map_obj()->drop_game_item(SharedData::get_instance()->game_save.game_item_list[0].obj_id, SharedData::get_instance()->game_save.game_item_list[0].uuid, position.x, position.y);
        SharedData::get_instance()->game_save.game_item_list[0] = SharedData::get_instance()->game_save.game_item_list[1];
        SharedData::get_instance()->game_save.game_item_list[1] = SharedData::get_instance()->game_save.game_item_list[2];
        SharedData::get_instance()->game_save.game_item_list[2].uuid = -1;
        SharedData::get_instance()->game_save.game_item_list[2].obj_id = -1;
        moveCommands.use_item = 0;
        InputController::get_instance()->p1_input[BTN_ITEM] = 0;
    }
}

int character::get_current_item_id_from_slot()
{
    return SharedData::get_instance()->game_save.game_item_list[0].obj_id;
}

void character::morph_item(int new_obj_id)
{
    SharedData::get_instance()->game_save.game_item_list[0].obj_id = new_obj_id;
}

void character::remove_game_item_from_slot()
{
    SharedData::get_instance()->game_save.game_item_list[0].obj_id = -1;
    SharedData::get_instance()->game_save.game_item_list[0].uuid = -1;
}

void character::set_is_on_game_item_area(bool state)
{
    is_on_game_item_area = state;
}

/// @TODO: this must be moved to player, as character attack must be very simple
void character::attack(bool dont_update_colors, short updown_trajectory, bool always_charged)
{
    if (attack_state != ATTACK_NOT && (TimerView::get_instance()->getTimer()-state.attack_timer) >= (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[state.direction][state.animation_type][state.animation_state].delay) {
		//std::cout << "character::attack - shoot projectile END" << std::endl;
		attack_state = ATTACK_NOT;
	}

    ATTACK_TYPES must_attack = check_must_attack(always_charged);
    check_charging_colors(always_charged);
    attack_button_last_state = moveCommands.attack;


    int attack_id = -1;


    if (must_attack == ATTACK_TYPE_NOATTACK) {
        return;
    } else if (must_attack == ATTACK_TYPE_NORMAL) {
        if (always_charged == true) {
            attack_id = GameData::get_instance()->game_data.semi_charged_projectile_id;
        } else {
            if (_normal_shot_projectile_id > 0) {
                attack_id = _normal_shot_projectile_id;
            } else {
                attack_id = 0;
            }
        }
    } else if (must_attack == ATTACK_TYPE_SEMICHARGED) {
        attack_id = GameData::get_instance()->game_data.semi_charged_projectile_id;
    } else if (must_attack == ATTACK_TYPE_FULLYCHARGED) {
        attack_id = _charged_shot_projectile_id;
    }


    if (attack_id != -1) {
        std::cout << "character::attack - attack_id: " << attack_id << std::endl;

        //if (!is_player()) { std::cout << "CHAR::attack::attack_id: " << attack_id << std::endl; }
        if (attack_id == _charged_shot_projectile_id || attack_id == GameData::get_instance()->game_data.semi_charged_projectile_id) {
			if (is_player() && SoundView::get_instance()->is_playing_repeated_sfx() == true) {
				SoundView::get_instance()->stop_repeated_sfx();
			}
		}


        //std::cout << "character::attack - shoot projectile" << std::endl;
        st_position proj_pos = get_attack_position();

        projectile_list.push_back(projectile(attack_id, state.direction, proj_pos, is_player()));
        projectile &temp_proj = projectile_list.back();
        temp_proj.set_is_permanent();
        temp_proj.play_sfx(!is_player());
        temp_proj.set_owner(this);

        _player_must_reset_colors = true;


        // second projectile for player that fires multiple ones
        if ((attack_id == 0 || attack_id == _normal_shot_projectile_id || (attack_id == GameData::get_instance()->game_data.semi_charged_projectile_id && always_charged == true)) && is_player() && _simultaneous_shots > 1) { /// @TODO - move number of simultaneous shots to character/data-file
            int pos_x_second = proj_pos.x+TILESIZE;
            if (state.direction == ANIM_DIRECTION_RIGHT) {
                pos_x_second = proj_pos.x-TILESIZE;
            }
            projectile_list.push_back(projectile(attack_id, state.direction, st_position(pos_x_second, proj_pos.y+5), is_player()));
            projectile &temp_proj2 = projectile_list.back();
            temp_proj2.set_is_permanent();
            temp_proj2.set_owner(this);
        }
        if (attack_id == 0 || attack_id == _normal_shot_projectile_id) { // handle normal attack differences depending on player
            if (updown_trajectory == 1) {
                temp_proj.set_trajectory(TRAJECTORY_DIAGONAL_UP);
                set_animation_type(ANIM_TYPE_ATTACK_DIAGONAL_UP);
            } else if (updown_trajectory == -1) {
                temp_proj.set_trajectory(TRAJECTORY_DIAGONAL_DOWN);
                set_animation_type(ANIM_TYPE_ATTACK_DIAGONAL_DOWN);
            } else if (is_on_attack_frame() == true && updown_trajectory == 0 && (state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_UP || state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_DOWN)) { // not shooting diagonal, but animation is on diagonal -> reset to normal attack
                set_animation_type(ANIM_TYPE_ATTACK);
            }

        }

        int proj_trajectory = GameData::get_instance()->get_projectile(attack_id).trajectory;
        temp_proj.set_owner(this);
        if (proj_trajectory == TRAJECTORY_CENTERED || proj_trajectory == TRAJECTORY_SLASH) {
            temp_proj.set_owner_direction(&state.direction);
            temp_proj.set_owner_position(&position);
		}
        if (proj_trajectory == TRAJECTORY_TARGET_DIRECTION || proj_trajectory == TRAJECTORY_TARGET_EXACT || proj_trajectory == TRAJECTORY_ARC_TO_TARGET || proj_trajectory == TRAJECTORY_FOLLOW) {
            // NPC
            if (!is_player() && GameManager::get_instance()->get_player() != nullptr) {
                temp_proj.set_target_position(GameManager::get_instance()->get_player()->get_position_ref());
            // PLAYER
            } else {
                GameEnemy* temp_npc = nullptr;
                if (proj_trajectory == TRAJECTORY_TARGET_DIRECTION || proj_trajectory == TRAJECTORY_TARGET_EXACT || proj_trajectory == TRAJECTORY_ARC_TO_TARGET) {
                    temp_npc = GameManager::get_instance()->get_current_map_obj()->find_nearest_enemy(st_position(position.x, position.y));
                } else {
                    temp_npc = GameManager::get_instance()->get_current_map_obj()->find_nearest_enemy_on_direction(st_position(position.x, position.y), state.direction);
                }
                if (temp_npc != nullptr) {
                    temp_proj.set_target_position(temp_npc->get_position_ref());
                }
            }
		}

		attack_state = ATTACK_START;
		state.attack_timer = TimerView::get_instance()->getTimer();
		if (state.animation_type == ANIM_TYPE_STAND) {
            set_animation_type(ANIM_TYPE_ATTACK);
		} else if (state.animation_type == ANIM_TYPE_JUMP) {
            set_animation_type(ANIM_TYPE_JUMP_ATTACK);
		} else if (is_in_stairs_frame()) {
            set_animation_type(ANIM_TYPE_STAIRS_ATTACK);
		} else if (state.animation_type == ANIM_TYPE_WALK) {
			//std::cout << "+++++++++++ CHARACTER - set animation to ANIM_TYPE_WALK_ATTACK" << std::endl;
            set_animation_type(ANIM_TYPE_WALK_ATTACK);
		}
    }
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::advance_frameset()
{

    //if (is_player()) std::cout << "character::show - direction: " << state.direction << ", type: " << state.animation_type << ", state: " << state.animation_state << "\n";
    //[CHAR_ANIM_DIRECTION_COUNT][ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT]
    if (state.direction > CHAR_ANIM_DIRECTION_COUNT) {
        //if (is_player()) std::cout << "WARNING - character::show - (" << name << ") error, direction value " << state.direction << " is invalid" << std::endl;
        set_direction(ANIM_DIRECTION_LEFT);
		return;
	}
    if (state.animation_type > ANIM_TYPE_COUNT) {
        //if (is_player()) std::cout << "character::show - error, type value " << state.direction << " is invalid" << std::endl;
		return;
	}
    if ((is_player() && state.animation_state > MAX_PLAYER_SPRITES) || (!is_player() && state.animation_state > MAX_NPC_SPRITES)) {
        //if (is_player()) std::cout << "character::show - error, animation_state value " << state.animation_state << " is invalid. state.animation_type: " << state.animation_type << std::endl;

        //std::cout << "### RESET-FRAME-N #1 ###" << std::endl;
		state.animation_state = 0;
		return;
	}
    if (have_frame_graphic(state.direction, state.animation_type, state.animation_state) == false) {
        //if (!is_player()) std::cout << "CHAR::advance_frameset - reset frameset animation" << std::endl;
        _was_animation_reset = true;

        //std::cout << "### RESET-FRAME-N #2 ###" << std::endl;
        state.animation_state = 0;
        _is_last_frame = true;
    } else {
        if (have_frame_graphic(state.direction, state.animation_type, state.animation_state+1) == false) {
            //if (!is_player()) std::cout << "CHAR::advance_frameset - _is_last_frame TRUE" << std::endl;
            _is_last_frame = true;
        } else {
            //if (is_player()) std::cout << "CHAR::advance_frameset - _is_last_frame FALSE" << std::endl;
            _is_last_frame = false;
        }
    }
}



void character::reset_jump()
{
    _obj_jump.finish();
}

void character::consume_projectile()
{
    if (projectile_list.size() > 0) {
        projectile_list.at(0).consume_projectile();
    }

}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::show() {
    if (is_dead() == true) {
        return;
    }


	// update real position
	char_update_real_position();
    if (!is_player() && is_on_visible_screen() == false) {
		return;
	}

    /*
    if (is_player() == false) {
        animation_obj.show_sprite(realPosition);
    } else {
        show_at(realPosition);
    }
    */

    show_previous_sprites();

    if (is_player()) {
        st_float_position player_pos = player_data::SharedPlayerData::get_instance()->getPosition();
        show_at(st_position(player_pos.x - 54/3 - GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x, player_pos.y + (54/3) - GameManager::get_instance()->get_current_map_obj()->getMapScrolling().y));
    } else {
        show_at(relativePosition);
    }


    // TODO: move zoom in/out logic to gameManager
    if (is_player() && state.animation_type == ANIM_TYPE_GOT_ITEM) {
        std::cout << ">>>>>>>>>>>>>>>>>>>> ANIM_TYPE_GOT_ITEM - interrupt-timer[" << SharedData::get_instance()->get_item_timer << "], timer[" << TimerView::get_instance()->getTimer() << "]" << std::endl;
        // revert animation
        if (SharedData::get_instance()->get_item_timer <= TimerView::get_instance()->getTimer()) {
            if (_is_last_frame == true && state.animation_inverse == false) {
                std::cout << ">>>>>>>> ANIM_TYPE_GOT_ITEM - invert frames" << std::endl;
                state.animation_inverse = true;
            // reached first frame
            } else if (_is_last_frame == false && state.animation_inverse == true && state.animation_state != 0) {
                std::cout << "GOT-ITEM - DEC-ZOOM" << std::endl;
                if (ImageView::get_instance()->get_scale() > 1.0) {
                    ImageView::get_instance()->inc_scale(-0.02);
                }
            } else if (state.animation_state == 0 && state.animation_inverse == true) {
                std::cout << ">>>>>>>> ANIM_TYPE_GOT_ITEM - FINISH" << std::endl;
                state.animation_inverse = false;
                SharedData::get_instance()->must_interrupt_character_execution = false;
                SharedData::get_instance()->get_item_timer = 0;
                ImageView::get_instance()->reset_scale();
                set_animation_type(ANIM_TYPE_STAND);
                GameManager::get_instance()->start_stage_music();
            }
        // show item over player (on his hand)
        } else {
            std::cout << "GOT-ITEM - INC-ZOOM" << std::endl;
            if (ImageView::get_instance()->get_scale() < 1.2) {
                ImageView::get_instance()->inc_scale(0.01);
            }
            Draw::get_instance()->show_object_graphic(relativePosition.x, relativePosition.y+40, got_item_id);
        }
    }
}

void character::reset_dash_effect()
{
    dash_effect_shadow_surface_frame.freeGraphic();
}

void character::show_previous_sprites()
{
    if (must_show_dash_effect == false && state.animation_type != ANIM_TYPE_SLIDE) {
        dash_effect_shadow_surface_frame.freeGraphic();
        reset_dash_effect();
        return;
    }

    //if (dash_effect_shadow_surface_frame.is_null()) {
        st_imageData* surface_frame_original = get_current_frame_surface(state.direction, state.animation_type, state.animation_state);
        // make a copy of the frame
        dash_effect_shadow_surface_frame = ImageView::get_instance()->initSurface(st_size(surface_frame_original->surface->w, surface_frame_original->surface->h));
        ImageView::get_instance()->copyArea(st_position(0, 0), *surface_frame_original, dash_effect_shadow_surface_frame);

    //}

    // show previous frames
    std::map<std::string, st_char_sprite_data>::iterator it_graphic = ImageView::get_instance()->character_graphics_list.find(name);
    for (int i=0; i<previous_position_list.size(); i++) {
        // only show each two frames
        if (i%2 == 0) {
            continue;
        }
        st_float_position screen_pos = get_screen_position_from_point(previous_position_list.at(i));
        ImageView::get_instance()->set_surface_alpha(40*i/2, dash_effect_shadow_surface_frame);
        ImageView::get_instance()->renderTexturePortionAt(0, 0, dash_effect_shadow_surface_frame.surface->w, dash_effect_shadow_surface_frame.surface->h, screen_pos.x, screen_pos.y, dash_effect_shadow_surface_frame.texture);
    }
    ImageView::get_instance()->set_surface_alpha(255, dash_effect_shadow_surface_frame);
}

void character::show_at(st_position pos)
{
    // check attack frame

    if (_attack_frame_n != -1 && is_on_attack_frame() && state.animation_state == _attack_frame_n) {
        _is_attack_frame = true;
    } else {
        _is_attack_frame = false;
    }

    // show background, if any
    if (have_background_graphics() == true) {
        ImageView::get_instance()->renderImageAt(pos.x, pos.y, ImageView::get_instance()->character_graphics_background_list.find(name)->second);
    }

    // only advance if time for the current frame has finished
    advance_frameset();

    // turn is a special case, if it does not exist, we must show stand instead
    if ((state.animation_type == ANIM_TYPE_TURN || state.animation_type == ANIM_TYPE_VERTICAL_TURN) && have_frame_graphic(state.direction, state.animation_type, state.animation_state) == false) {
        //if (is_player() == false) std::cout << "show() - TURN graphic FINISHED, y[" << position.y << "]" << std::endl;
        if (have_frame_graphic(state.direction, ANIM_TYPE_WALK, state.animation_state) == true) {
            show_sprite_graphic(state.direction, ANIM_TYPE_WALK, state.animation_state, pos);
        } else {
            show_sprite_graphic(state.direction, ANIM_TYPE_STAND, state.animation_state, pos);
        }
    // npc teleport use shows stand for now (will have a common graphic to show in the future)
    } else {
        show_sprite_graphic(state.direction, state.animation_type, state.animation_state, pos);
    }
    st_rectangle hitbox = get_hitbox();
    if (GameManager::get_instance()->get_current_map_obj() != nullptr) {
        hitbox.x -= GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
    }
    show_sprite();
#ifdef SHOW_HITBOXES
    //std::cout << "[" << name << "] - pos.x: " << position.x << ", hitbox - x: " << hitbox.x << ", hitbox.y: " << hitbox.y << std::endl;
    ImageView::get_instance()->draw_rectangle(hitbox, 255, 0, 255, 100);
#endif
#ifdef SHOW_VULNERABLE_AREAS
    st_rectangle vulnerable_area = get_vulnerable_area();
    ImageView::get_instance()->draw_rectangle(vulnerable_area, 255, 0, 0, 100);
#endif
}





void character::show_sprite()
{
    //if (is_player()) std::cout << "######### timer[" << TimerView::get_instance()->getTimer() << "], state.animation_timer[" << state.animation_timer << "]" << std::endl;
    unsigned int now_timer = TimerView::get_instance()->getTimer();
    if (state.animation_timer < now_timer) { // time passed the value to advance frame

		// change animation state to next frame
		int frame_inc = 1;
		if (state.animation_inverse == true) {
            frame_inc = frame_inc * -1;
            std::cout << "show_sprite, INVERSE, inc: " << frame_inc << std::endl;
		}
        int new_frame = (state.animation_state + frame_inc);
        if (have_frame_graphic(state.direction, state.animation_type, new_frame)) {
			state.animation_state += frame_inc;
            if (state.animation_state < 0) {
                if (state.animation_inverse == true) {
                    advance_to_last_frame();
                } else {
                    state.animation_state = 0;
                }
            }
        } else {
            if (state.animation_type == ANIM_TYPE_VERTICAL_TURN) {
				if (state.direction == ANIM_DIRECTION_LEFT) {
                    set_direction(ANIM_DIRECTION_RIGHT);
				} else {
                    set_direction(ANIM_DIRECTION_LEFT);
				}
                if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #F" << std::endl;
                set_animation_type(ANIM_TYPE_STAND);
			}
			if (state.animation_inverse == false) {
				if (state.animation_state > 0) {
                    //std::cout << "### RESET-FRAME-N #3 ###" << std::endl;
                    state.animation_state = 0;
				}
			} else {
				advance_to_last_frame();
            }

            _was_animation_reset = true;
            // some animation types reset to stand/other
            if (state.animation_type == ANIM_TYPE_STAIRS_ATTACK) {
                set_animation_type(ANIM_TYPE_STAIRS);
            }
        }
		if (state.animation_type == ANIM_TYPE_WALK_ATTACK) {
            state.animation_timer = TimerView::get_instance()->getTimer() + 180;
		} else {
            short direction = ANIM_DIRECTION_RIGHT;
            int delay = (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[direction][state.animation_type][state.animation_state].delay;
            state.animation_timer = TimerView::get_instance()->getTimer() + delay;
        }
    }
}

// we need to reset the time of the animation to discount pause
// because otherwise, we can't animate player/enemies during a pause like transition
void character::reset_sprite_animation_timer()
{
    if (state.animation_type == ANIM_TYPE_WALK_ATTACK) {
        state.animation_timer = TimerView::get_instance()->getTimer() + 180;
    } else {
        short direction = ANIM_DIRECTION_RIGHT;

        int delay = 100;
        if (ImageView::get_instance()->character_graphics_list.find(name) != ImageView::get_instance()->character_graphics_list.end()) {
            if (direction < CHAR_ANIM_DIRECTION_COUNT) {
                if (state.animation_type < ANIM_TYPE_COUNT) {
                    if (state.animation_state < ANIM_FRAMES_COUNT) {
                        delay = (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[direction][state.animation_type][state.animation_state].delay;
                    }
                }
            }
        }

        state.animation_timer = TimerView::get_instance()->getTimer() + delay;
    }
}

void character::show_sprite_graphic(short direction, short type, short frame_n, st_position frame_pos)
{

    if (state.invisible == true) {
        return;
    }

    st_imageData *frame_surface = get_current_frame_surface(direction, type, frame_n);

    if (frame_surface == nullptr) {
        return;
    }

    /// blinking when hit
    unsigned int now_timer = TimerView::get_instance()->getTimer();
    if (now_timer < hit_duration+last_hit_time) {

        if (hit_animation_timer > now_timer) {
            // TODO::IURI - usar frame específico //
            //ImageView::get_instance()->show_white_surface_at(frame_surface, frame_pos);
            hit_animation_count = 0;
            return;
        } else if ((hit_animation_timer+HIT_BLINK_ANIMATION_LAPSE) < now_timer) {
            hit_animation_count++;
            if (hit_animation_count > 2) {
                hit_animation_timer = now_timer+HIT_BLINK_ANIMATION_LAPSE;
            }
            return;
        }
    }
    if (_progressive_appear_pos == 0) {
        if (rotated_graphic_frame.texture != nullptr) {
            //if (!is_player()) std::cout << "CHAR::SHOW[" << name << "] - rotated_graphic_frame.w[" << rotated_graphic_frame.surface->w << "]" << std::endl;
            ImageView::get_instance()->renderImageAt(frame_pos.x, frame_pos.y, rotated_graphic_frame);
        } else {
            //if (!is_player()) std::cout << "CHAR::SHOW[" << name << "] - NORMAL" << std::endl;
            ImageView::get_instance()->renderImageAt(frame_pos.x, frame_pos.y, *frame_surface);
        }
    } else {
        int diff_y = frameSize.height-_progressive_appear_pos;

        ImageView::get_instance()->renderTexturePortionAt(0, 0, frameSize.width, (frameSize.height-_progressive_appear_pos), frame_pos.x, frame_pos.y-diff_y, frame_surface->texture);
        _progressive_appear_pos--;
        if (_progressive_appear_pos == 0) {
            position.y -= frameSize.height;
        }
    }
}

st_imageData *character::get_current_frame_surface(short direction, short type, short frame_n)
{

    if (frame_n < 0) {
        std::cout << "ERROR::character::get_current_frame_surface - negative frame-n" << std::endl;
        frame_n = 0;
    }

    std::map<std::string, st_char_sprite_data>::iterator it_graphic;
    it_graphic = ImageView::get_instance()->character_graphics_list.find(name);
    if (it_graphic == ImageView::get_instance()->character_graphics_list.end()) {
        std::cout << "ERROR: #1 character::show_sprite_graphic - Could not find graphic for NPC [" << name << "]" << std::endl;
        return nullptr;
    }
    // for non left-right directions, use the original facing direction for NPCs
    if (is_player() == false && direction != ANIM_DIRECTION_LEFT && direction != ANIM_DIRECTION_RIGHT) {
        std::cout << "%%%% character::show_sprite_graphic(" << name << ") invalid sprite direction[" << direction << "], use[" << facing << "] instead" << std::endl;
        direction = facing;
    }
    if (have_frame_graphic(direction, type, frame_n) == false) { // check if we can find the graphic with the given N position
        if (frame_n == 0) {
            //std::cout << ">> character::show_sprite_graphic(" << name << ") #1 - no graphic for type (" << type << "):frame_n(" << frame_n << "), set to STAND" << std::endl;
            if (type == ANIM_TYPE_TELEPORT) {
                type = ANIM_TYPE_JUMP;
            } else {
                type = ANIM_TYPE_STAND;
            }
        } else {
            //std::cout << ">> character::show_sprite_graphic(" << name << ") #1 - no graphic for type (" << type << "):frame_n(" << frame_n << "), set to ZERO pos" << std::endl;
            frame_n = 0;
            state.animation_state = 0;
            _was_animation_reset = true;
            return &it_graphic->second.frames[direction][type][frame_n].frameSurface;
        }
        state.animation_state = 0;
        _was_animation_reset = true;


        //std::cout << "### RESET-FRAME-N #4 ###" << std::endl;
        if (have_frame_graphic(direction, type, frame_n) == false) { // check if we can find the graphic with the given type
            //if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #G" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
            type = ANIM_TYPE_STAND;
            if (have_frame_graphic(direction, type, frame_n) == false) { // check if we can find the graphic at all
                std::cout << "ERROR: #2 character::show_sprite_graphic - Could not find graphic for NPC [" << name << "] at pos[0][0][0]" << std::endl;
                return nullptr;
            }
        }
    }

    return &it_graphic->second.frames[direction][type][frame_n].frameSurface;
}

void character::reset_gravity_speed()
{
    accel_speed_y = 0.25;
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool character::gravity(bool boss_demo_mode=false)
{

    /// @TODO: gravity speed is starting at 1.25, it should start at 0.25

    if (_progressive_appear_pos != 0) {
        reset_gravity_speed();
        return false;
    }

    if (_obj_jump.is_started() && _obj_jump.get_state() == JUMPUP) {
        return false;
    }

    if (!GameManager::get_instance()->get_current_map_obj()) {
        std::cout << "ERROR: can't execute gravity without a map" << std::endl;
        reset_gravity_speed();
        return false; // error - can't execute this action without an associated map
	}

    bool can_use_air_dash = false;
    if (is_player() == true) {
        can_use_air_dash = can_air_dash();
    }

    if (can_use_air_dash == true && state.animation_type == ANIM_TYPE_SLIDE) {
        reset_gravity_speed();
        return false;
    }

    if ((_is_boss || _is_stage_boss) && get_anim_type() == ANIM_TYPE_INTRO) {
        return false;
    }
    int gravity_max_speed = GRAVITY_MAX_SPEED;
    if (state.animation_type == ANIM_TYPE_TELEPORT) {
        gravity_max_speed = GRAVITY_TELEPORT_MAX_SPEED;
    } else if (state.animation_type == ANIM_TYPE_HIT) {
        gravity_max_speed = 2;
    }

	// ------------- NPC gravity ------------------ //
	if (!is_player()) {
        if (_ignore_gravity == true) {
            return false;
        }
        if (can_fly == false || SharedData::get_instance()->is_showing_boss_intro == true) {
            bool is_moved = false;
            short int limit_speed = move_speed;
			if (boss_demo_mode == true) {
                limit_speed = gravity_max_speed;
			}
            if (limit_speed < 1) {
                limit_speed = 1;
            }

			for (int i=limit_speed; i>0; i--) {
                bool res_test_move = test_change_position(0, i);
                if ((boss_demo_mode == true && position.y <= TILESIZE*2) || res_test_move == true) {
                    position.y += i;
					is_moved = true;
					break;
				}
            }
			return is_moved;
        }
        reset_gravity_speed();
        return false; // not moved because of IA type
	}

	// ------------ PLAYER gravity --------------------- //
    if (is_in_stairs_frame() && (get_anim_type() == ANIM_TYPE_HIT || get_anim_type() == ANIM_TYPE_HIT_SPECIAL)) {
        reset_gravity_speed();
        return false;
	}

    if (_obj_jump.is_started() == false && can_fly == false && position.y < GameManager::get_instance()->get_current_map_obj()->get_size().height*TILESIZE+1 + frameSize.height) {
        // tem que inicializar essa variável sempre que for false
        accel_speed_y = accel_speed_y + accel_speed_y*gravity_y;

        if (accel_speed_y < 0.25) {
            accel_speed_y = 0.25;
        } else if (accel_speed_y > gravity_max_speed) {
            accel_speed_y = gravity_max_speed;
        }

        int adjusted_speed = accel_speed_y;
        if (adjusted_speed < 1) {
            adjusted_speed = 1;
        }

		if (state.animation_type == ANIM_TYPE_TELEPORT) {

            if (_teleport_minimal_y - position.y > TILESIZE) {
                adjusted_speed = gravity_max_speed;
            } else {
                adjusted_speed = gravity_max_speed/2;
            }
		}

        st_map_collision map_col;
        bool was_moved = false;
        bool hit_slope = executeCheckSlope(0, adjusted_speed);
        if (!hit_slope) {
            for (int i=adjusted_speed; i>0; i--) {
                map_col = map_collision(0, i+1, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
                int mapLock = map_col.block;

                if (state.animation_type == ANIM_TYPE_TELEPORT && position.y < _teleport_minimal_y-TILESIZE) {
                    mapLock = BLOCK_UNBLOCKED;
                } else if (!is_player() && state.animation_type == ANIM_TYPE_TELEPORT && position.y >= _teleport_minimal_y-TILESIZE) {
                    _teleport_minimal_y = frameSize.height+TILESIZE*2; // RESET MIN_Y -> remove limit for next telepor
                } else if (position.y+frameSize.height >= GameManager::get_instance()->get_current_map_obj()->get_size().height*TILESIZE) { // out of screen
                    mapLock = BLOCK_UNBLOCKED;
                }

                float movedY = 0;
                if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_STAIR_X || mapLock == BLOCK_STAIR_Y) {
                    //if (is_player()) std::cout << "character::gravity - FALL, mapLock[" << mapLock << "]" << std::endl;
                    if (mapLock != BLOCK_WATER || (mapLock == BLOCK_WATER && abs((float)i*WATER_SPEED_MULT) < 1)) {
                        //std::cout << "QUICKSAND.GRAVIOTY #1" << std::endl;
                        position.y += i;
                        movedY = i;
                    } else {
                        //std::cout << "QUICKSAND.GRAVIOTY #2" << std::endl;
                        position.y += i*WATER_SPEED_MULT;
                        movedY = i*WATER_SPEED_MULT;
                    }
                    if (state.animation_type != ANIM_TYPE_JUMP && state.animation_type != ANIM_TYPE_JUMP_ATTACK && state.animation_type != ANIM_TYPE_TELEPORT && state.animation_type != ANIM_TYPE_SLIDE && state.animation_type != ANIM_TYPE_HIT && (state.animation_type != ANIM_TYPE_JUMP_ATTACK || (state.animation_type == ANIM_TYPE_JUMP_ATTACK && state.attack_timer+ATTACK_DELAY < TimerView::get_instance()->getTimer()))) {
                        //std::cout << "LEAVE STAIRS - GRAVITY #1, current-anim-type[" << state.animation_type << "]" << std::endl;
                        //if (is_player()) std::cout << "CHAR::GRAVITY - p.x[" << position.x << "] CHAR::RESET_TO_JUMP #A.1" << std::endl;
                        set_animation_type(ANIM_TYPE_JUMP);
                    }
                    was_moved = true;

                    // TODO::IURI - adicionar checagem de morte, etc //

                    if (state.animation_type != ANIM_TYPE_TELEPORT) {
                        _is_falling = true;
                    }
                    break;
                } else if (map_col.terrain_type == TERRAIN_QUICKSAND) {
                    //std::cout << "CHAR::GRAVITY - over QUICKSAND" << std::endl;
                    position.y += QUICKSAND_GRAVITY;
                    movedY = QUICKSAND_GRAVITY;
                }
                if (i == 1) {
                    reset_gravity_speed();
                }
            }
        }

		if (was_moved == false && (state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) && state.animation_type != ANIM_TYPE_SLIDE) {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #H" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
			return true;
        } else if (was_moved == false && state.animation_type == ANIM_TYPE_TELEPORT && position.y >= RES_H/3) {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #I" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
			return true;
		}
        //std::cout << "CHECK-FALLING - was_moved[" << was_moved << "], _is_falling[" << _is_falling << "]" << std::endl;
        if (was_moved == false && _is_falling == true) {
            _is_falling = false;
            if (is_player()) {
                if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #Y.1" << std::endl;
                set_animation_type(ANIM_TYPE_STAND);
                //std::cout << "SFX_PLAYER_JUMP #2" << std::endl;
                SoundView::get_instance()->play_sfx(SFX_PLAYER_JUMP);
            }
        }

        check_platform_move(map_col.terrain_type);

		// teleport finished
        //std::cout << "NOT FALLING, RESET ACCEL_SPEED_Y" << std::endl;
    }

    return false;
}

bool character::hit_ground() // indicates if character is standing above ground
{
    // if position did not changed since last execution, return previous hit_ground value
    /*
    if (position == _previous_position) {
        return _hit_ground;
    }
    */
    short map_tile_x = (position.x + frameSize.width/2)/TILESIZE;
    short map_tile_y1 = (position.y + frameSize.height)/TILESIZE;
    short map_tile_y2 = (position.y + frameSize.height/2)/TILESIZE;
    int pointLock1 = GameManager::get_instance()->getMapPointLock(st_position(map_tile_x, map_tile_y1));
    _hit_ground = false;
    // TODO: TERRAIN_SLOPE //
    if (pointLock1 != TERRAIN_UNBLOCKED && pointLock1 != TERRAIN_WATER && pointLock1 != TERRAIN_STAIR) {
        //if (is_player()) std::cout << "hit_ground #1" << std::endl;
        _hit_ground = true;
    } else if (!is_player() && pointLock1 == TERRAIN_STAIR) {
        _hit_ground = true;
    } else {
        int pointLock2 = GameManager::get_instance()->getMapPointLock(st_position(map_tile_x, map_tile_y2));
        if (pointLock1 != pointLock2) {
            //if (is_player()) std::cout << "hit_ground #2" << std::endl;
            _hit_ground = true;
        }

    }
    return _hit_ground;
}

bool character::is_on_last_animation_frame()
{
    return (_is_last_frame && state.animation_timer < TimerView::get_instance()->getTimer()+2000); // plus 2 seconds before the fight starts
}

bool character::is_on_quicksand()
{
    int px = (get_hitbox().x + get_hitbox().w/2)/TILESIZE;
    int py = (get_hitbox().y + get_hitbox().h - QUICKSAND_JUMP_LIMIT)/TILESIZE;
    Uint8 terrain_type = GameManager::get_instance()->getMapPointLock(st_position(px, py));
    bool result = (terrain_type == TERRAIN_QUICKSAND);
    //std::cout << "CHAR::is_on_quicksand - [" << result << "], terrain[" << (int)terrain_type << "]" << std::endl;
    return result;
}

bool character::will_hit_ground(int y_change) const
{
    short map_tile_x = (position.x + frameSize.width/2)/TILESIZE;
    short map_tile_y = (position.y + y_change + frameSize.height)/TILESIZE;
    int pointLock = GameManager::get_instance()->getMapPointLock(st_position(map_tile_x, map_tile_y));
    // TODO: TERRAIN_SLOPE //
    if (pointLock != TERRAIN_UNBLOCKED && pointLock != TERRAIN_WATER) {
        return true;
    }
    return false;
}

bool character::is_on_screen()
{
    st_float_position scroll(0, 0);
    if (GameManager::get_instance()->get_current_map_obj() == nullptr) {
        return false;
    }

    scroll = GameManager::get_instance()->get_current_map_obj()->getMapScrolling();

    // is on screen plus a bit more on both sides
    if (abs((float)position.x+frameSize.width*2) >= scroll.x && abs((float)position.x-frameSize.width*2) <= scroll.x+RES_W) {
        return true;
    }


    // regular enemies work only on a limited screen
    if (is_stage_boss() == false) {
        return false;
    }

    // is on left of the screen
    if (abs((float)position.x) > scroll.x-RES_W/2 && abs((float)position.x) < scroll.x) {
        // check wall-lock on the range
        int map_point_start = (scroll.x-RES_W/2)/TILESIZE;
        int map_point_end = scroll.x/TILESIZE;
        bool found_lock = false;
        for (int i=map_point_start; i<=map_point_end; i++) {
            if (GameManager::get_instance()->get_current_map_obj()->get_map_point_wall_lock(i) == true) {
                found_lock = true;
            }
        }
        if (found_lock == false) {
            return true;
        }
    }

    // is on right to the screen
    if (abs((float)position.x) > scroll.x+RES_W && abs((float)position.x) < scroll.x+RES_W*1.5) {
        int map_point_start = (scroll.x+RES_W)/TILESIZE;
        int map_point_end = (scroll.x*1.5)/TILESIZE;
        bool found_lock = false;
        for (int i=map_point_start; i<=map_point_end; i++) {
            if (GameManager::get_instance()->get_current_map_obj()->get_map_point_wall_lock(i) == true) {
                found_lock = true;
            }
        }
        if (found_lock == false) {
            std::cout << "CHAR::is_on_screen[" << name << "], x[" << position.x << "], map_point_start[" << map_point_start << "], map_point_end[" << map_point_end << "]" << std::endl;
            return true;
        }
        if (name == "Dynamite Bot") std::cout << ">>>> character::is_on_screen - right <<<<" << std::endl;
    }
    return false;
}

bool character::is_on_visible_screen()
{
    if (GameManager::get_instance()->get_current_map_obj() == nullptr) { // used ins scenes
        return true;
    }
    st_float_position scroll = GameManager::get_instance()->get_current_map_obj()->getMapScrolling();
    // entre scroll.x e scroll.x+RES_W



    if (abs((float)position.x + frameSize.width) >= scroll.x && abs((float)position.x) < scroll.x+RES_W) {
        if (abs((float)position.y + frameSize.height) >= scroll.y && abs((float)position.y) < scroll.y+AREA_H) {
            if (!is_player()) {
                //std::cout << "CHAR::is_on_visible_screen - pos.x[" << position.x << "], w[" << frameSize.width << "], scroll.x[" << scroll.x << "]" << std::endl;
            }
        }
        return true;
    }
    return false;
}

bool character::is_entirely_on_screen()
{
    if (GameManager::get_instance()->get_current_map_obj() == nullptr) { // used ins scenes
        return true;
    }
    st_float_position scroll = GameManager::get_instance()->get_current_map_obj()->getMapScrolling();

    if (abs((float)position.x + frameSize.width) >= scroll.x+TILESIZE && abs((float)position.x+(float)frameSize.width) < scroll.x+RES_W-TILESIZE) {
        if (abs((float)position.y + frameSize.height) >= scroll.y+TILESIZE && abs((float)position.y+(float)frameSize.height) < scroll.y+AREA_H-TILESIZE) {
            if (!is_player()) {
                //std::cout << "CHAR::is_on_visible_screen - pos.x[" << position.x << "], w[" << frameSize.width << "], scroll.x[" << scroll.x << "]" << std::endl;
            }
        }
        return true;
    }
    return false;
}

bool character::is_invisible() const
{
	return state.invisible;
}


void character::activate_super_jump()
{
    _super_jump = true;
}

void character::activate_force_jump()
{
	_force_jump = true;
}

st_float_position *character::get_position_ref()
{
	return &position;
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
st_float_position character::getPosition() const
{
	/// @TODO - this is crashing sometimes
	return position;
}

void character::set_position(struct st_position new_pos)
{
    //std::cout << "### character::set_position, x[" << new_pos.x << "], y[" << new_pos.y << "] ###" << std::endl;
    //std::cout << "<<<<<<<<<<< POS.X.SET #3 >>>>>>>>>>>>>" << std::endl;
    position.x = new_pos.x;
	position.y = new_pos.y;
    char_update_real_position();
}

void character::inc_position(float inc_x, float inc_y)
{
    //std::cout << "CHAR::inc_position::inc_x[" << inc_x << "]" << std::endl;
    position.x += inc_x;
    position.y += inc_y;
    moved_dist.x += inc_x;
    moved_dist.y += inc_y;
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool character::slide(st_float_position mapScrolling)
{
    if (is_player() == false) {
        return false;
    }

    // change jump button released state, if needed
    if (_dash_button_released == false && moveCommands.dash == 0) {
        _dash_button_released = true;
    }


    if (state.animation_type == ANIM_TYPE_TELEPORT || state.animation_type == ANIM_TYPE_SHIELD) {
        return false;
    }

    if ((state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) && can_air_dash() == false) {
        return false;
    }


	if (is_in_stairs_frame()) {
		return false;
	}

    //std::cout << "CHAR::SLIDE - ability.slide[" << SharedData::get_instance()->game_save.abilities[PROPERTY_NAME_SLIDE] << "]" << std::endl;
    if (SharedData::get_instance()->game_save.abilities[PROPERTY_NAME_SLIDE] == false) {
        //std::cout << "SLIDE::OUT #5" << std::endl;
        return false;
    }


    // no need to slide
    if (state.animation_type != ANIM_TYPE_SLIDE && moveCommands.dash != 1) {
        //std::cout << "SLIDE::OUT #6" << std::endl;
        return false;
    }

    if (position.x <= 0 && state.direction == ANIM_DIRECTION_LEFT) {
        if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A.2" << std::endl;
        set_animation_type(ANIM_TYPE_JUMP);
        state.slide_distance = 0;
        return false;
    }

    bool did_hit_ground = hit_ground();

    int adjust = -1;
    st_map_collision map_col = map_collision(0, adjust, GameManager::get_instance()->get_current_map_obj()->getMapScrolling(), ANIM_TYPE_SLIDE); // slide_adjust is used because of adjustments in slide collision
    int map_lock =  map_col.block;

    st_map_collision map_col_above = map_collision(0, adjust-20, GameManager::get_instance()->get_current_map_obj()->getMapScrolling(), ANIM_TYPE_SLIDE); // slide_adjust is used because of adjustments in slide collision
    int map_lock_above =  map_col_above.block;

    //std::cout << "SLIDE - map_lock[" << map_lock << "], map_lock_above[" << map_lock_above << "]" << std::endl;

    // releasing down (or dash button) interrupts the slide
    if (moveCommands.dash != 1 && state.animation_type == ANIM_TYPE_SLIDE && (map_lock == BLOCK_UNBLOCKED || map_lock == BLOCK_WATER) && (map_lock_above == BLOCK_UNBLOCKED || map_lock_above == BLOCK_WATER)) {
        if (did_hit_ground) {
            //if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #Y.2" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
        } else {
            set_animation_type(ANIM_TYPE_JUMP);
        }
        //if (is_player()) std::cout << "CHAR::SLIDE LEAVE #1.2" << std::endl;
        return false;
    }


    if (state.slide_distance > TILESIZE*SLIDE_TILES_MAX_DISTANCE && (map_lock == BLOCK_UNBLOCKED || map_lock == BLOCK_WATER) && (map_lock_above == BLOCK_UNBLOCKED || map_lock_above == BLOCK_WATER)) {
        if (did_hit_ground == true) {
            //if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #Y.3" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
        } else {
            set_animation_type(ANIM_TYPE_JUMP);
        }
        state.slide_distance = 0;
        //if (is_player()) std::cout << "CHAR::SLIDE LEAVE #1.3" << std::endl;
        return false;
    }

    //std::cout << "_dash_button_released: " << _dash_button_released << std::endl;

    // start slide
    if (state.animation_type != ANIM_TYPE_SLIDE && _dash_button_released == true) {
        if (moveCommands.dash == 1) {
            if (did_hit_ground == true || (did_hit_ground == false && _can_execute_airdash == true)) {
                _can_execute_airdash = false;
                set_animation_type(ANIM_TYPE_SLIDE);
                state.slide_distance = 0;
                _dash_button_released = false;
                int adjust_x = -3;
                if (state.direction == ANIM_DIRECTION_LEFT) {
                    adjust_x = frameSize.width+3;
                }
                previous_position_list.clear();
                GameManager::get_instance()->get_current_map_obj()->add_animation(ANIMATION_STATIC, &ImageView::get_instance()->dash_dust, position, st_position(adjust_x, frameSize.height-8), 160, 0, state.direction, st_size(8, 8));
            }
		}
    }

    if (state.animation_type != ANIM_TYPE_SLIDE) {
        return false;
    }

    // if there is no ground, interrupts slide
    //std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    st_rectangle hitbox_temp = get_hitbox();
    //std::cout << "hitbox x[" << hitbox_temp.x << "], y[" << hitbox_temp.y << "], w[" << hitbox_temp.w << "], h[" << hitbox_temp.h << "], py[" << position.y << "], frame.h[" << frameSize.height << "]" << std::endl;
    st_map_collision map_col_fall = map_collision(0, 4, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    int fall_map_lock = map_col_fall.block;
    //std::cout << "character::slide - fall_map_lock: " << fall_map_lock << std::endl;
    //std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

    if (can_air_dash() == false && (fall_map_lock == BLOCK_UNBLOCKED || fall_map_lock == BLOCK_WATER)) {
        //if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A.3" << std::endl;
        set_animation_type(ANIM_TYPE_JUMP);
        state.slide_distance = 0;
        return false;
    }

    // check if trying to leave screen LEFT
    if (state.direction == ANIM_DIRECTION_LEFT && position.x <= 0) {
        state.slide_distance = 0;
        //if (is_player()) std::cout << "CHAR::SLIDE LEAVE #1.4" << std::endl;
        return false;
    }

    // check if trying to leave screen RIGHT
    if (is_player() == true && (relativePosition.x + frameSize.width/2) > RES_W) {
        state.slide_distance = 0;
        //if (is_player()) std::cout << "CHAR::SLIDE LEAVE #1.5" << std::endl;
        return false;
    }

    // end of map
    if (state.direction == ANIM_DIRECTION_RIGHT && position.x + frameSize.width > GameManager::get_instance()->get_map_size().width * TILESIZE) {
        state.slide_distance = 0;
        //if (is_player()) std::cout << "CHAR::SLIDE LEAVE #1.6" << std::endl;
        return false;
    }


    float res_move_x = 0;
    int mapLockAfter = BLOCK_UNBLOCKED;
    _obj_jump.finish();

    // reduce progressively the jump-move value in oder to deal with collision
    float max_speed = move_speed * 2.5;
    for (float i=max_speed; i>0.0; i--) {


        int temp_i;
        if (state.direction == ANIM_DIRECTION_LEFT) {
            temp_i = -i;
        } else {
            temp_i = i;
        }
        st_map_collision map_col = map_collision(temp_i, -3, mapScrolling);
        mapLockAfter = map_col.block;

        //std::cout << "SLIDE - DEBUG i[" << i << "], map_col.terrain_type[" << map_col.terrain_type << "], map_col.block[" << map_col.block << "]" << std::endl;

        if (mapLockAfter == BLOCK_UNBLOCKED) {
            res_move_x = temp_i;
            break;
        } else if (mapLockAfter == BLOCK_WATER) {
            res_move_x = temp_i*0.8;
            break;
        }
    }


    if (res_move_x != 0 && (mapLockAfter == BLOCK_UNBLOCKED || mapLockAfter == BLOCK_WATER)) {
        position.x += res_move_x;
        moved_dist.x += res_move_x;
        state.slide_distance += abs((float)res_move_x);
    } else {
        if (state.animation_type == ANIM_TYPE_SLIDE) {
            set_animation_type(ANIM_TYPE_JUMP);
        }
        state.slide_distance = 0;
        return false;
    }

    return true;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool character::jump(int jumpCommandStage, st_float_position mapScrolling)
{
    // @TODO - can only jump again once set foot on land
    if (jumpCommandStage == 0 && jump_button_released == false) {
        jump_button_released = true;
    }

    if (state.animation_type == ANIM_TYPE_HIT) {
        //std::cout << "JUMP LEAVE #1" << std::endl;
        return false;
    }

    // can't jump while on air dash
    if (state.animation_type == ANIM_TYPE_SLIDE && hit_ground() == false) {
        //std::cout << "JUMP LEAVE #2" << std::endl;
        return false;
    }

    int water_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position((position.x+frameSize.width/2)/TILESIZE, (position.y+6)/TILESIZE));

    if (_force_jump == true || (jumpCommandStage == 1 && jump_button_released == true)) {
        //std::cout << "char::jump - button pressed" << std::endl;

        if (is_in_stairs_frame()) {
            if (_obj_jump.is_started() == false) {
                if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A.5" << std::endl;
                set_animation_type(ANIM_TYPE_JUMP);
                _is_falling = true;
                _stairs_falling_timer = TimerView::get_instance()->getTimer() + STAIRS_GRAB_TIMEOUT; // avoid player entering stairs immediatlly after jumping from it
                //std::cout << "JUMP OUT OF STAIRS #1" << std::endl;
                return false;
            } else {
                //std::cout << "JUMP OUT OF STAIRS #2" << std::endl;
                _obj_jump.interrupt();
                if (_force_jump == true) {
                    _force_jump = false;
                }
            }
        } else {
            //std::cout << "char::jump - _is_falling[" << _is_falling << "], _jumps_number: " << _jumps_number << ", obj::_jumps_number: " << _obj_jump.get_jumps_number() << std::endl;
            if (_is_falling == false && (_obj_jump.is_started() == false || (_jumps_number > _obj_jump.get_jumps_number()))) {
                int map_tile_x = (position.x+frameSize.width/2) / TILESIZE;
                int map_tile_y = (get_hitbox().y+get_hitbox().h) / TILESIZE;
                int platform_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(map_tile_x, map_tile_y));
                if (_super_jump == true) {
                    _super_jump = false;
                    //std::cout << "JUMP START #1" << std::endl;
                    _obj_jump.start(true, water_lock);
                } else if (!(moveCommands.down == 1 && platform_lock == TERRAIN_PLATFORM)) {
                    //std::cout << "JUMP START #2 - platform_lock[" << platform_lock << "]" << std::endl;
                    _obj_jump.start(false, water_lock);
                }
                if (state.animation_type == ANIM_TYPE_SLIDE) {
                    _dashed_jump = true;
                }
                //if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A.6" << std::endl;
                set_animation_type(ANIM_TYPE_JUMP);
                jump_button_released = false;
            }
        }
    }

    bool is_onquicksand = false;
    if (is_on_quicksand()) {
        is_onquicksand = true;
    }

    if (_obj_jump.is_started() == true) {
        int jump_speed = _obj_jump.get_speed();
        bool jump_moved = false;

        // if got into stairs, finish jumping
        if ((is_in_stairs_frame())) {
            _obj_jump.finish();
            return false;
        }

        if (jump_speed < 0 && jumpCommandStage == 0 && _force_jump == false) {
            _obj_jump.interrupt();
        }

        if (jump_speed > 0) {
            bool hit_slope = executeCheckSlope(0, jump_speed);
            if (hit_slope) {
                reset_animation_type();
                std::cout << "################ hit_slope #2, p.bottom[" << get_hitbox().y + get_hitbox().h << "]" << std::endl;
                _obj_jump.finish();
                return false;
            }
        }

        // check collision
        for (int i=abs((float)jump_speed); i>0; i--) {
            int speed_y = 0;
            if (jump_speed > 0) {
                speed_y = i;
            } else {
                speed_y = i*-1;
            }
            st_map_collision map_col = map_collision(0, speed_y, mapScrolling);
            int map_lock = map_col.block;
            if (is_player() && map_lock != BLOCK_UNBLOCKED) {
                //std::cout << "jump::check_collision - i[" << i << "], map_lock["  << map_lock << "]" << std::endl;
            }

            if (map_lock == BLOCK_UNBLOCKED || map_lock == BLOCK_WATER) {
                //std::cout << "jump.speed[" << speed_y << "]" << std::endl;
                if (is_onquicksand) {
                    position.y -= QUICKSAND_JUMP_LIMIT*3;
                    jump_button_released = false;
                    _obj_jump.finish();
                } else {
                    position.y += speed_y;
                }
                jump_moved = true;
                break;
            }
        }
        if (jump_speed != 0 && jump_moved == false) {
            InputController::get_instance()->test_erumble();
            if (jump_speed < 0) {
                _obj_jump.interrupt();
            } else {
                _obj_jump.finish();
            }
        }

        _obj_jump.execute(water_lock);
        if (_obj_jump.is_started() == false) {
            //std::cout << "SFX_PLAYER_JUMP #1" << std::endl;
            SoundView::get_instance()->play_sfx(SFX_PLAYER_JUMP);
            if (_force_jump == true) {
                _force_jump = false;
            }
        } else {
            if (is_player() && position.y > GameManager::get_instance()->get_current_map_obj()->get_size().height*TILESIZE+1) {
                std::cout << "**** JUMP::LEAVE (death)" << std::endl;
                _obj_jump.finish();
            }
        }
    } else {
        if (_force_jump == true) {
            _force_jump = false;
        }
        //accel_speed_y = GRAVITY_MAX_SPEED;
        gravity();
        return false;
    }


    return true;
}




void character::check_map_collision_point(int &map_block, int &new_map_lock, int &old_map_lock, int mode_xy) // mode_xy 0 is x, 1 is y
{
    if (map_block == BLOCK_UNBLOCKED && new_map_lock == TERRAIN_WATER) {
        map_block = BLOCK_WATER;
    }

    bool must_block = false;

    if (old_map_lock != new_map_lock) {
        if (is_player() == false && new_map_lock == TERRAIN_UNBLOCKED && old_map_lock == TERRAIN_WATER) { // NPCs must not leave water
            must_block = true;
        } else if (is_player() == false && _is_boss == false && old_map_lock == TERRAIN_UNBLOCKED && new_map_lock == TERRAIN_WATER) { // non-boss NPCs must not enter water
            must_block = true;
        } else if (is_player() == false && new_map_lock == TERRAIN_HSCROLL_LOCK) {
            must_block = true;
        } else if (is_player() == false && new_map_lock == TERRAIN_VSCROLL_LOCK) {
            must_block = true;
        } else if (new_map_lock == TERRAIN_SLOPE) {
            must_block = true;
        } else if (new_map_lock != TERRAIN_UNBLOCKED && new_map_lock != TERRAIN_WATER && new_map_lock != TERRAIN_STAIR) {
            must_block = true;
        } else if (map_block == BLOCK_UNBLOCKED && (new_map_lock != BLOCK_UNBLOCKED && new_map_lock != TERRAIN_STAIR && new_map_lock != TERRAIN_WATER)) {
            must_block = true;
        }

    } else if (map_block == BLOCK_UNBLOCKED && new_map_lock == TERRAIN_SOLID) {
        must_block = true;
    } else if (map_block == BLOCK_WATER && new_map_lock == BLOCK_X) {
        must_block = true;
    }

    //std::cout << "check_map_collision_point.must_block[" << must_block << "]" << std::endl;
    if (must_block == true) {
        if (mode_xy == 0) {
            if (map_block != BLOCK_XY) {
                map_block = BLOCK_X;
            }
        } else {
            if (map_block == BLOCK_X) {
                map_block = BLOCK_XY;
            } else if (map_block != BLOCK_XY) {
                map_block = BLOCK_Y;
            }
        }
    }
    //std::cout << "check_map_collision_point.map_block[" << map_block << "]" << std::endl;
}

bool character::process_special_map_points(int map_lock, int incx, int incy, st_position map_pos)
{
    int direction = ANIM_DIRECTION_LEFT;
    if (incx > 0) {
        direction = ANIM_DIRECTION_RIGHT;
    }

    // TODO: crerate a method for transition, as it can be as a teleporter
    if (incx != 0 && map_lock == TERRAIN_AREA_HSCROLL_LOCK) {
        int temp_xinc = TILESIZE+6;
        if (state.direction == ANIM_DIRECTION_LEFT) {
            temp_xinc = -(TILESIZE+6);
        }
        GameManager::get_instance()->transition_area_horizontal(state.direction, map_pos.x, map_pos.y);
        return true;
    } else if (incy != 0 && map_lock == TERRAIN_AREA_VSCROLL_LOCK) {
        std::cout << "##### DEBUG TERRAIN_AREA_VSCROLL_LOCK #####" << std::endl;
        // check that player is fully into the scroll-lock area //
        st_rectangle hitbox = get_hitbox();
        int py = hitbox.y + hitbox.h + 3;
        if (incy < 0) {
            py = hitbox.y - 3;
        }
        std::cout << "TERRAIN_AREA_VSCROLL_LOCK - incx[" << incx << "], incy[" << incy << "], map_pos.x[" << map_pos.x << "], map_pos.y[" << map_pos.y << "], px[" << hitbox.x/TILESIZE << "], py[" << py/TILESIZE << "]" << std::endl;

        // both left and right must be in vlock tiles
        int left_map_lock = GameManager::get_instance()->getMapPointLock(st_position(hitbox.x/TILESIZE, py/TILESIZE));
        int right_map_lock = GameManager::get_instance()->getMapPointLock(st_position((hitbox.x+hitbox.w)/TILESIZE, py/TILESIZE));

        if (left_map_lock != TERRAIN_AREA_VSCROLL_LOCK || right_map_lock != TERRAIN_AREA_VSCROLL_LOCK) {
            std::cout << "TERRAIN_AREA_VSCROLL_LOCK::FALSE px_left[" << hitbox.x/TILESIZE << "], px_right[" << (hitbox.x+hitbox.w)/TILESIZE << "], left_map_lock[" << left_map_lock << "], right_map_lock[" << right_map_lock << "]" << std::endl;
            return false;
        }

        int temp_yinc = TILESIZE+6;
        int directionY = ANIM_DIRECTION_DOWN;
        if (incy < 0) {
            temp_yinc = -(TILESIZE+6);
            directionY = ANIM_DIRECTION_UP;
        }


        // TODO::IURI //
        GameManager::get_instance()->transition_area_vertical(directionY, map_pos.y);
        return true;
    } else if (incx != 0 && map_lock == TERRAIN_HSCROLL_LOCK) {
        std::cout << "##### DEBUG #931 - incx[" << incx << "] #####" << std::endl;
        int temp_xinc = TILESIZE+6;
        if (state.direction == ANIM_DIRECTION_LEFT) {
            temp_xinc = -(TILESIZE+6);
        }
        GameManager::get_instance()->horizontal_screen_move(state.direction, true, map_pos.x, map_pos.y);
		return true;
    } else if (incy != 0 && map_lock == TERRAIN_VSCROLL_LOCK) {
        //std::cout << "##### DEBUG #932 #####" << std::endl;
        // check that player is fully into the scroll-lock area //
        st_rectangle hitbox = get_hitbox();
        int py = hitbox.y + hitbox.h + 3;
        if (incy < 0) {
            py = hitbox.y - 3;
        }
        //std::cout << ">>>>>>> incx[" << incx << "], incy[" << incy << "], map_pos.x[" << map_pos.x << "], map_pos.y[" << map_pos.y << "], px[" << hitbox.x/TILESIZE << "], py[" << py/TILESIZE << "]" << std::endl;

        // both left and right must be in vlock tiles
        int left_map_lock = GameManager::get_instance()->getMapPointLock(st_position(hitbox.x/TILESIZE, py/TILESIZE));
        int right_map_lock = GameManager::get_instance()->getMapPointLock(st_position((hitbox.x+hitbox.w)/TILESIZE, py/TILESIZE));

        if (left_map_lock != TERRAIN_VSCROLL_LOCK || right_map_lock != TERRAIN_VSCROLL_LOCK) {
            //std::cout << "px_left[" << hitbox.x/TILESIZE << "], px_right[" << (hitbox.x+hitbox.w)/TILESIZE << "], left_map_lock[" << left_map_lock << "], right_map_lock[" << right_map_lock << "]" << std::endl;
            return false;
        }

        int temp_yinc = TILESIZE+6;
        int directionY = ANIM_DIRECTION_DOWN;
        if (incy < 0) {
            temp_yinc = -(TILESIZE+6);
            directionY = ANIM_DIRECTION_UP;
        }


        // TODO::IURI //
        GameManager::get_instance()->vertical_screen_move(directionY, true, map_pos.y);
        return true;

    }
    if (state.animation_type != ANIM_TYPE_TELEPORT && (map_lock == TERRAIN_SPIKE)) {
        damage_spikes(true);
        return true;
    }

    return false;
}

void character::check_platform_move(short map_lock)
{
    float move = 0.0;
    bool can_move = true;

    if (_moving_platform_timer < TimerView::get_instance()->getTimer()) {
        int pos_y = (position.y + frameSize.height/2) / TILESIZE;
        if (map_lock == TERRAIN_MOVE_LEFT) {
            move = (move_speed-0.5)*-1;
            int pos_x = (position.x + move) / TILESIZE;
            if (is_player() && state.direction == ANIM_DIRECTION_RIGHT) { // add a few pixels because of graphic when turned right
                pos_x = (position.x + 5 + move) / TILESIZE;
            }
            int point_terrain = GameManager::get_instance()->getMapPointLock(st_position(pos_x, pos_y));
            if (point_terrain != TERRAIN_UNBLOCKED && point_terrain != TERRAIN_WATER) {
                can_move = false;
            }
        } else if (map_lock == TERRAIN_MOVE_RIGHT) {
            move = move_speed-0.5;
            int pos_x = (position.x + frameSize.width - 10 + move) / TILESIZE;
            int point_terrain = GameManager::get_instance()->getMapPointLock(st_position(pos_x, pos_y));
            if (point_terrain != TERRAIN_UNBLOCKED && point_terrain != TERRAIN_WATER) {
                can_move = false;
            }
        } else {
            return;
        }
        if (can_move) {
            position.x += move;
            moved_dist.x += move;
            _moving_platform_timer = TimerView::get_instance()->getTimer()+MOVING_GROUND;
        }
    }
}

st_map_collision character::map_collision(const float incx, const short incy, st_float_position mapScrolling, int hitbox_anim_type)
{
    int py_adjust = 0;
    if (is_player() == true) {
        py_adjust = 8;
    }
    int terrain_type = TERRAIN_UNBLOCKED;

    /// @TODO: move to char hitbox
    if (state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) {
        py_adjust = 1;
    }

    int map_block = BLOCK_UNBLOCKED;

    if (GameManager::get_instance()->get_current_map_obj() == nullptr) {
        return st_map_collision(BLOCK_XY, TERRAIN_SOLID);
    }

    GameManager::get_instance()->get_current_map_obj()->collision_char_object(this, incx, incy);
    object_collision res_collision_object = GameManager::get_instance()->get_current_map_obj()->get_obj_collision();

    //if (is_player() && incx < 0) std::cout << "CHAR::map_collision, y_inc[" << incy << "], obj_collision.block[" << res_collision_object._block << "]" << std::endl;

    if (is_player() == true && res_collision_object._block != 0) {
        // deal with teleporter object that have special block-area and effect (9)teleporting)
        if (state.animation_type != ANIM_TYPE_TELEPORT && res_collision_object._object != nullptr) {

            //std::cout << "CHAR::PLAYER::check-obj-collision #1, p.x[" << position.x << "], block["  << res_collision_object._block << "], type[" << res_collision_object._object->get_type() << "]" << std::endl;

            if (res_collision_object._object->get_type() == OBJ_BOSS_TELEPORTER || (res_collision_object._object->get_type() == OBJ_FINAL_BOSS_TELEPORTER && res_collision_object._object->is_started() == true)) {
                if (is_on_teleporter_capsulse(res_collision_object._object) == true) {
                    set_direction(ANIM_DIRECTION_RIGHT);
                    GameManager::get_instance()->object_teleport_boss(res_collision_object._object->get_boss_teleporter_dest(), res_collision_object._object->get_boss_teleport_map_dest(), res_collision_object._object->get_obj_map_id(), true);
                }
            } else if (res_collision_object._object->get_type() == OBJ_STAGE_BOSS_TELEPORTER) {
                //std::cout << "character::map_collision - OBJ_STAGE_BOSS_TELEPORTER" << std::endl;
                if (is_on_teleporter_capsulse(res_collision_object._object) == true) {
                    set_direction(ANIM_DIRECTION_RIGHT);
                    GameManager::get_instance()->object_teleport_boss(res_collision_object._object->get_boss_teleporter_dest(), res_collision_object._object->get_boss_teleport_map_dest(), res_collision_object._object->get_obj_map_id(), false);
                }
            // platform teleporter is just a base where player can step in to teleport
            } else if (res_collision_object._object->get_type() == OBJ_PLATFORM_TELEPORTER && is_on_teleport_platform(res_collision_object._object) == true) {
                set_direction(ANIM_DIRECTION_RIGHT);
                SoundView::get_instance()->play_sfx(SFX_TELEPORT);
                GameManager::get_instance()->object_teleport_boss(res_collision_object._object->get_boss_teleporter_dest(), res_collision_object._object->get_boss_teleport_map_dest(), res_collision_object._object->get_obj_map_id(), false);
            // ignore block
            } else if (res_collision_object._object->get_type() == OBJ_FINAL_BOSS_TELEPORTER && res_collision_object._object->is_started() == false) {
                // do nothing
            } else if (res_collision_object._object->get_type() == OBJ_FRONT_DOOR_TELEPORTER && InputController::get_instance()->p1_input[BTN_UP] == 1) {
                SoundView::get_instance()->play_sfx(SFX_TELEPORT);
                GameManager::get_instance()->object_teleport_boss(res_collision_object._object->get_boss_teleporter_dest(), res_collision_object._object->get_boss_teleport_map_dest(), res_collision_object._object->get_obj_map_id(), false);
            } else if (res_collision_object._object->get_type() == OBJ_TYPE_PUSH_BOX && incx != 0) {
                st_position obj_pos = res_collision_object._object->get_position();
                int obj_move_speed  = res_collision_object._object->get_move_speed();
                if (incx > 0) {
                    res_collision_object._object->inc_position(obj_move_speed, 0);
                } else {
                    res_collision_object._object->inc_position(-obj_move_speed, 0);
                }
                // still blocks player
                map_block = res_collision_object._block;
            } else if (!get_item(res_collision_object)) {
                if (res_collision_object._object->get_type() == OBJ_TREASURE_CHEST) {
                    if (res_collision_object._object->is_started() == false) {
                        Draw::get_instance()->draw_game_button(relativePosition.x+frameSize.width/2, relativePosition.y-20, INPUT_IMAGES_DPAD_DOWN);
                        if (InputController::get_instance()->p1_input[BTN_DOWN] == 1) {
                            got_item_id = res_collision_object._object->get_ability();
                            std::cout << "character::map_collision - OPEN TREASURE CHEST, id_item[" << got_item_id << "]" << std::endl;
                            // @TODO: get the object, put abover player, wait a bit, then get it //
                            res_collision_object._object->start();
                            if (got_item_id != -1) {
                                std::cout << ">>>>>>>>>>>>> GOT ITEM <<<<<<<<<<<<<<" << std::endl;
                                SharedData::get_instance()->get_item_timer = TimerView::get_instance()->getTimer() + 7000;
                                SharedData::get_instance()->must_interrupt_character_execution = true;

                                set_animation_type(ANIM_TYPE_GOT_ITEM);
                                cancel_slide();
                                _obj_jump.interrupt();
                                set_animation_type(ANIM_TYPE_GOT_ITEM);

                                std::cout << ">>>>>>>>>>>>>>>> state.anim_type[" << state.animation_type << "], got_item_value[" << ANIM_TYPE_GOT_ITEM << "]" << std::endl;

                                MapController *map = GameManager::get_instance()->get_current_map_obj();
                                GameObject temp_obj = GameObject(got_item_id, map, st_position(position.x, position.y), st_position(-1, -1), 0);
                                temp_obj.set_position(st_position(static_cast<int>(position.x), static_cast<int>(position.y)));
                                // TODO: Change get_item do handle regular object or just object id instead of having to create those //
                                object_collision col_obj;
                                col_obj._object = &temp_obj;
                                get_item(col_obj);
                                temp_obj.set_finished(true);

                            }
                        }
                    }
                } else {
                    map_block = res_collision_object._block;

                    if (map_block == BLOCK_Y || map_block == BLOCK_XY) {
                        _can_execute_airdash = true;
                    }
                    // INSIDE PLATFORM OBJECT, MUST DIE
                    if (map_block == BLOCK_INSIDE_OBJ) {
                        //std::cout << "DEBUG-OBJ-COLlISION #5" << std::endl;
                        damage(999, true);
                        return st_map_collision(BLOCK_UNBLOCKED, TERRAIN_SOLID);
                    } else {
                        //std::cout << "player-over-obj-platform, map_block[" << map_block << "], py[" << position.y << "]" << std::endl;
                    }
                }
            }
        }
    } else if (is_player() == false && res_collision_object._block != 0) {
        map_block = res_collision_object._block;
        if (map_block == BLOCK_Y || map_block == BLOCK_XY) {
            _can_execute_airdash = true;
        }
    }


    if (is_player()) {
        if (have_shoryuken() == true && state.animation_type == ANIM_TYPE_SPECIAL_ATTACK) {
            GameManager::get_instance()->get_current_map_obj()->collision_player_special_attack(this, incx, incy, 9, py_adjust);
        } else {
            GameEnemy* enemy_touch = GameManager::get_instance()->get_current_map_obj()->collision_player_enemies(this, 0, 0);
            if (enemy_touch != nullptr) {
                if (enemy_touch->get_size().height > this->get_size().height) {
                    damage(TOUCH_DAMAGE_SMALL, false);
                } else {
                    damage(TOUCH_DAMAGE_BIG, false);
                }
                if (_was_hit == true) {
                    enemy_touch->hit_player();
                }
            }
            GameManager::get_instance()->get_current_map_obj()->collision_player_npcs(this);
        }
	}

    // no need to test map collision if object collision is already X+Y
    if (map_block == BLOCK_XY && incx != 0) {
        //std::cout << "st_map_collision character::map_collision LEAVE #1" << std::endl;
        return st_map_collision(BLOCK_XY, TERRAIN_SOLID);
    }


    if (incx == 0 && incy == 0) {
        //std::cout << "st_map_collision character::map_collision LEAVE #2" << std::endl;
        return st_map_collision(BLOCK_UNBLOCKED, TERRAIN_UNBLOCKED);
    }

    if (_always_move_ahead == false && ((incx < 0 && position.x + incx < 0) || (incx > 0 && position.x+incx > GameManager::get_instance()->get_map_size().width*TILESIZE))) {
        if (map_block == BLOCK_UNBLOCKED) {
            map_block = BLOCK_X;
        } else {
            map_block = BLOCK_XY;
        }
        //if (incx > 0) std::cout << ">>>>>>> #3 - map_block[" << map_block << "]" << std::endl;
    }
    if ((incy < 0 && ((position.y+incy+frameSize.height < 0) || (incx > 0 && position.y+incx+TILESIZE > GameManager::get_instance()->get_map_size().height*TILESIZE)))) {
        if (map_block == BLOCK_UNBLOCKED) {
            map_block = BLOCK_Y;
        } else {
            map_block = BLOCK_XY;
        }
        if (incx > 0) std::cout << ">>>>>>> #4 - map_block[" << map_block << "]" << std::endl;
    }

    // if we are out of map, return always true
    if (_always_move_ahead == true) {
        if ((incx < 0 && (position.x+incx < 0)) || (incx > 0 && position.x+incx > GameManager::get_instance()->get_map_size().width*TILESIZE)) {
            return st_map_collision(BLOCK_UNBLOCKED, TERRAIN_UNBLOCKED);
        }
    }

    //if (incx > 0) std::cout << ">>>>>>> #9 - map_block[" << map_block << "]" << std::endl;


    /// @TODO - use collision rect for the current frame. Until there, use 3 points check
    int py_top, py_middle, py_bottom;
    int px_left, px_center, px_right;
    int old_px_left, old_px_center, old_px_right;
    st_rectangle rect_hitbox = get_hitbox(hitbox_anim_type);

    py_top = rect_hitbox.y + incy + py_adjust;

    py_middle = rect_hitbox.y + incy + rect_hitbox.h/2;
    py_bottom = rect_hitbox.y + incy + rect_hitbox.h - 2;

    px_center = rect_hitbox.x + incx + rect_hitbox.w/2;
    px_left = rect_hitbox.x + incx;
    px_right = rect_hitbox.x + incx + rect_hitbox.w;

    old_px_left = rect_hitbox.x;
    old_px_right = rect_hitbox.x + rect_hitbox.w;

    if (incx == 0 && incy != 0) {
        px_right--;
    }


    //std::cout << "st_map_collision character::map_collision::py_bottom: " << py_bottom << std::endl;

    st_position map_point;
    st_position old_map_point;
    map_point.x = px_left/TILESIZE;
    old_map_point.x = old_px_left/TILESIZE;
    int new_map_lock = TERRAIN_UNBLOCKED;
    int old_map_lock = TERRAIN_UNBLOCKED;
    if (incx > 0) {
        map_point.x = px_right/TILESIZE;
        old_map_point.x = old_px_right/TILESIZE;
    }

    /// @TODO - use a array-of-array for poijts in order to having a cleaner code

    int map_x_points[3];
    if (incx == 0 && incy != 0) {
        px_left++;
        px_right--;
    }
    map_x_points[0] = px_left/TILESIZE;
    map_x_points[1] = px_center/TILESIZE;
    map_x_points[2] = px_right/TILESIZE;

    int map_y_points[3];
    map_y_points[0] = py_top/TILESIZE;
    map_y_points[1] = py_middle/TILESIZE;
    map_y_points[2] = py_bottom/TILESIZE;

    // TEST X POINTS
    if (incx != 0) {
        for (int i=0; i<3; i++) {
            if (is_player() && (state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) && i == 0) {
                map_point.y = (py_top+1)/TILESIZE;
            } else {
                map_point.y = map_y_points[i];
            }
            old_map_point.y = map_point.y;
            old_map_lock = GameManager::get_instance()->getMapPointLock(old_map_point);
            new_map_lock = GameManager::get_instance()->getMapPointLock(map_point);
            check_map_collision_point(map_block, new_map_lock, old_map_lock, 0);

            if (is_player() && process_special_map_points(new_map_lock, incx, incy, map_point) == true) {
                return st_map_collision(map_block, new_map_lock);
            }
        }
    }

    // TEST Y POINTS
    if (incy < 0) {
    map_point.y = py_top/TILESIZE;
    } else if (incy > 0) {
        map_point.y = py_bottom/TILESIZE;
    }

    // Search fro slope in the middle X point first
    bool hit_slope = false;
    if ((state.animation_type == ANIM_TYPE_JUMP || state.animation_type == ANIM_TYPE_JUMP_ATTACK) && incy != 0) {
        int slope_map_x_points[3];
        slope_map_x_points[0] = (px_center-4)/TILESIZE;
        slope_map_x_points[1] = px_center/TILESIZE;
        slope_map_x_points[2] = (px_center+4)/TILESIZE;
        for (int i=0; i<3; i++) {
            int test_x = slope_map_x_points[i];
            int test_y = (py_bottom)/TILESIZE;

            file_v6_room_tile tile_center = GameManager::get_instance()->get_current_map_obj()->getTileFromPosition(test_x, test_y);

            // this exists to prevent irregular movement when we reach the single-pixel space between slope tiles

            if (tile_center.tile_underlay.type == TILE_TYPE_SLOPE) {
                hit_slope = true;
                // TODO
                return st_map_collision(map_block, TERRAIN_SOLID);
            }

            //std::cout << "check_map_collision_point #2, map_block[" << map_block << "], pos.y[" << position.y << "], py_bottom[" << py_bottom << "], rect_hitbox.h[" << rect_hitbox.h << "], py_top[" << py_top << "], rect_hitbox.y[" << rect_hitbox.y << "], py_adjust[" << py_adjust << "], incy[" << incy << "]" << std::endl;
            //std::cout << "CHAR::GRAVITY - CHECK-SLOPE - tile.type[" << tile_center.tile_underlay.type << "], map.x[" << test_x << "], map.y[" << test_y << "], py_bottom[" << py_bottom << "]" << std::endl;
        }

    }

    //if (is_player() == false) std::cout << "CHAR::MAP_COLLISION[" << name << "], map_point.y: " << map_point.y << std::endl;

    if (incy != 0 && hit_slope == false) {
        for (int i=0; i<3; i++) {
            map_point.x = map_x_points[i];
            old_map_point.x = map_x_points[i];

            old_map_lock = GameManager::get_instance()->getMapPointLock(old_map_point);
            new_map_lock = GameManager::get_instance()->getMapPointLock(map_point);
            //py_top = rect_hitbox.y + incy + py_adjust;
            check_map_collision_point(map_block, new_map_lock, old_map_lock, 1);

            //std::cout << "check_map_collision_point #2, map_block[" << map_block << "], pos.y[" << position.y << "], py_bottom[" << py_bottom << "], rect_hitbox.h[" << rect_hitbox.h << "], py_top[" << py_top << "], rect_hitbox.y[" << rect_hitbox.y << "], py_adjust[" << py_adjust << "], incy[" << incy << "]" << std::endl;
            //if (is_player() && incy < 0) std::cout << "new_map_lock[" << new_map_lock << "], TERRAIN_PLATFORM[" << TERRAIN_PLATFORM << "]" << std::endl;

            if (new_map_lock != TERRAIN_UNBLOCKED) {
                //std::cout << "CHAR::GRAVITY - new_map_lock[" << new_map_lock << "], map.x[" << map_point.x << "], map.y[" << map_point.y << "], py_bottom[" << py_bottom << "]" << std::endl;
                terrain_type = new_map_lock;
            }

            if (is_player() && process_special_map_points(new_map_lock, incx, incy, map_point) == true) {
                std::cout << "SPECIAL-POINT-LEAVE" << std::endl;
                return st_map_collision(map_block, new_map_lock);
            }


            // SLOPE //
            if (map_block != BLOCK_UNBLOCKED && new_map_lock == TERRAIN_SLOPE) {
                std::cout << ">>>>>>>>>>>>>>>> ADJUST TO SLOPE" << std::endl;
                //map_block = slopesManager.checkSlope(0, incy, get_hitbox(state.animation_state));
            }
            // STAIRS //
            if ((map_block == BLOCK_UNBLOCKED || map_block == BLOCK_X || map_block == BLOCK_WATER) && incy > 0 && new_map_lock == TERRAIN_STAIR) { // stairs special case
                int middle_y_point_lock = TERRAIN_UNBLOCKED;
                if (incy == 1) { // gravity
                    middle_y_point_lock = GameManager::get_instance()->getMapPointLock(st_position(map_x_points[i], (py_bottom-1)/TILESIZE));
                } else { // other cases as falling or jump
                    middle_y_point_lock = GameManager::get_instance()->getMapPointLock(st_position(map_x_points[i], map_y_points[1]));
                }

                if (middle_y_point_lock != TERRAIN_STAIR) {
                    if (map_block == BLOCK_X) {
                        map_block = BLOCK_XY;
                    } else {
                        map_block = BLOCK_Y;
                    }
                }
            }
            // PLATFORM
            if (new_map_lock == TERRAIN_PLATFORM) {
                if (incy < 0) {
                    map_block = BLOCK_UNBLOCKED;
                } else if (incy > 0) {
                    int player_bottom_y = get_hitbox().y + get_hitbox().h;
                    int exact_player_y = map_point.y * TILESIZE;
                    if (player_bottom_y-exact_player_y >= TILESIZE/4) {
                        map_block = BLOCK_UNBLOCKED;
                        break;
                    }
                }
            }
        }
    }

    if (is_player()) {
        // check water splash
        int point_top = GameManager::get_instance()->getMapPointLock(st_position(map_x_points[1], map_y_points[0]));
        int point_middle = GameManager::get_instance()->getMapPointLock(st_position(map_x_points[1], map_y_points[1]));
        int point_bottom = GameManager::get_instance()->getMapPointLock(st_position(map_x_points[1], map_y_points[2]));

        //std::cout << ">> check water-splash - point_top.x: " <<  map_x_points[1] << ", point_top.y: " << map_y_points[1] << ", pos.x: " << position.x << ", pos.y: " << position.y << ", point_bottom: " << point_bottom << ", point_middle: " << point_middle << ", point_top: " << point_top << std::endl;

        if (incy != 0) {
            if (point_top == TERRAIN_UNBLOCKED && point_middle == TERRAIN_UNBLOCKED && point_bottom == TERRAIN_WATER && _water_splash == false) {
                if (incy < 0) {
                    SoundView::get_instance()->play_sfx(SFX_WATER_LEAVE);
                } else {
                    SoundView::get_instance()->play_sfx(SFX_WATER_LEAVE);
                }
                //std::cout << ">> ADD water splash animation - adjust_y: " << adjust_y << ", point_bottom: " << point_bottom << ", point_middle: " << point_middle << ", point_top: " << point_top << std::endl;
                _water_splash = true;
                //ANIMATION_TYPES pos_type, st_imageData* surface, const st_position &pos, st_position adjust_pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize
                GameManager::get_instance()->get_current_map_obj()->add_animation(ANIMATION_STATIC, &ImageView::get_instance()->water_splash, st_float_position(position.x, (map_y_points[2]-1)*TILESIZE), st_position(0, -6), 100, 0, ANIM_DIRECTION_LEFT, st_size(32, 23));
            } else if (point_top == point_bottom && point_top == point_middle && _water_splash == true) {
                //std::cout << ">> RE-ENABLE water splash animation - point_bottom: " << point_bottom << ", point_middle: " << point_middle << ", point_top: " << point_top << std::endl;
                _water_splash = false;
            }
        }
    }


    //if (is_player() && incx < 0) std::cout << "character::map_collision_v2 - map_block: " << map_block << std::endl;

    return st_map_collision(map_block, terrain_type);

}

bool character::is_on_teleporter_capsulse(GameObject *object)
{
    // check se player está dentro da área Y do objeto
    int obj_y = object->get_position().y;
    if (obj_y < position.y && (obj_y + object->get_size().height > position.y + frameSize.height)) {
        // só teleporta quando estiver no centro (1 TILE), caso contrário, ignora block
        double abs_value = TILESIZE/2 - object->get_size().width;
        int obj_center_diff = abs(abs_value)/2;
        int limit_min = object->get_position().x + obj_center_diff;
        int limit_max = object->get_position().x + object->get_size().width - obj_center_diff;
        int px = position.x + frameSize.width/2;
        std::cout << "px: " << px << ", limit_min: " << limit_min << ", limit_max: " << limit_max << std::endl;
        if (px > limit_min && px < limit_max) {
            return true;
        }
    }
    return false;
}

bool character::is_on_teleport_platform(GameObject *object)
{
    // check if player is above platform
    int obj_y = object->get_position().y;
    int py = position.y + frameSize.height;
    if (py-obj_y <= 2) {
        // só teleporta quando estiver no centro (1 TILE), caso contrário, ignora block
        double abs_value = TILESIZE/2 - object->get_size().width;
        int obj_center_diff = abs(abs_value)/2;
        int limit_min = object->get_position().x + obj_center_diff;
        int limit_max = object->get_position().x + object->get_size().width - obj_center_diff;
        int px = position.x + frameSize.width/2;
        std::cout << "px: " << px << ", limit_min: " << limit_min << ", limit_max: " << limit_max << std::endl;
        if (px > limit_min && px < limit_max) {
            return true;
        }
    }
    return false;
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::addSpriteFrame(int anim_type, int posX, st_imageData &spritesSurface, int delay)
{
	struct st_rectangle spriteArea;

	spriteArea.x = posX*frameSize.width;
    spriteArea.y = 0;
	spriteArea.w = frameSize.width;
    spriteArea.h = frameSize.height;

    // ANIM_TYPE_STAIRS_MOVE and ANIM_TYPE_STAIRS_SEMI have an extra frame that is the mirror of the first one

    for (int anim_direction=0; anim_direction<=1; anim_direction++) {
        for (int i=0; i<ANIM_FRAMES_COUNT; i++) { // find the last free frame
            if ((ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i].frameSurface.surface == nullptr) {

                st_spriteFrame *sprite = &(ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i];
                st_imageData gsurface = ImageView::get_instance()->imageFromRegion(spriteArea, spritesSurface);

                // RIGHT
                if (anim_direction != 0) {
                    ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface);
                // LEFT
                } else {
                    st_imageData gsurface_flip;
                    ImageView::get_instance()->flip_image(gsurface, gsurface_flip, flip_type_horizontal);
                    ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface_flip);
                }


                (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i].delay = delay;

                if (anim_type == ANIM_TYPE_STAIRS_MOVE || anim_type == ANIM_TYPE_STAIRS_SEMI) {
                    st_spriteFrame *sprite = &(ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i+1];
                    if (anim_direction != 0) {
                        st_imageData gsurface_flip;
                        ImageView::get_instance()->flip_image(gsurface, gsurface_flip, flip_type_horizontal);
                        ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface_flip);
                    } else {
                        ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface);
                    }
                    (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i+1].delay = delay;
                }


                break;
            }
        }
    }
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void character::set_is_player(bool set_player)
{
	is_player_type = set_player;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool character::is_player() const
{
	return is_player_type;
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
std::string character::get_name(void) const
{
	return name;
}

// ********************************************************************************************** //
// Returns true of character is over a staircase                                                  //
// ********************************************************************************************** //
st_position character::is_on_stairs(st_rectangle pos)
{
    if (_dropped_from_stairs == true) { // was dropped from stairs, can't grab again until invencibility time ends
        //if (is_player()) std::cout << "is_on_stairs - FALSE 1" << std::endl;
        return st_position(-1, -1);;
    }
    int map_tile_x, map_tile_y;

    int diff_w = pos.w/3;
	map_tile_x = (pos.x+diff_w)/TILESIZE;
	map_tile_y = (pos.y)/TILESIZE;

    if (GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(map_tile_x, map_tile_y)) == TERRAIN_STAIR) {
        //if (is_player()) std::cout << "is_on_stairs - TRUE 1" << std::endl;
		return st_position(map_tile_x, map_tile_y);
    }

    map_tile_x = (pos.x+pos.w-diff_w)/TILESIZE;
    if (GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(map_tile_x, map_tile_y)) == TERRAIN_STAIR) {
        //if (is_player()) std::cout << "is_on_stairs - TRUE 2" << std::endl;
        return st_position(map_tile_x, map_tile_y);
    }

    //if (is_player()) std::cout << "is_on_stairs - FALSE 2" << std::endl;
	return st_position(-1, -1);
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
st_size character::get_size() const
{
    return frameSize;
}

st_rectangle character::get_hitbox(int anim_type)
{
    float x = position.x;
    float y = position.y;
    float w = frameSize.width;
    float h = frameSize.height;

    if (anim_type == -1) {
        anim_type = state.animation_type;
    }

    // player hitbox is hardcoded
    if (is_player()) {
        if (anim_type == ANIM_TYPE_SLIDE) {
            x = position.x + 58;
            y = position.y + frameSize.height - TILESIZE + 4;
            //std::cout << "character::get_hitbox - py[" << position.y << "], frame.h[" << frameSize.height << "], hit.y[" << y << "]" << std::endl;
            // @TODO::IURI ///
            w = 46;
            h = TILESIZE - 4;
        } else { // stand/default
            x = position.x + 58;
            y = position.y + 3;
            // @TODO::IURI ///
            w = 46;
            h = frameSize.height-6;
        }
    } else {
        int anim_n = state.animation_state;
        int anim_type = state.animation_type;

        // prevent getting size from a frame that does not have information, use Vulnerable-area or hitbox from STAND instead
        st_rectangle col_rect;
        if (GameData::get_instance()->get_enemy(_number)->sprites[anim_type][anim_n].used == true) {
            col_rect = GameData::get_instance()->get_enemy(_number)->sprites[anim_type][anim_n].collision_rect;
        } else {
            col_rect = st_rectangle(GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.x,
                                    GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.y,
                                    GameData::get_instance()->get_enemy(_number)->frame_size.width,
                                    GameData::get_instance()->get_enemy(_number)->frame_size.height);
        }

        // IURI: removed this adjust because it was blocking enemies when it should not, like moving up/down
        // I don't know the original reason for this anymore, let's look for it and test if removing is OK
        if (state.direction == ANIM_DIRECTION_LEFT) {
            //x = position.x - (frameSize.width - col_rect.w) + col_rect.x + 2;
            x = position.x - (frameSize.width - col_rect.w) + col_rect.x;
        } else {
            x += col_rect.x - 2;
        }
        y += col_rect.y;
        w = col_rect.w - 4;
        h = col_rect.h;
        if (w <= 0 || h <= 0) {
            file_enemy_v3_1_2* npc_ref = GameData::get_instance()->get_enemy(_number);
            /*
            std::cout << "#### CHAR::GET_HITBOX name[" << name << "], x[" << x << "], y[" << y << "], w[" << w << "], h[" << h << "], animation_state[" << anim_n << "], animation_type[" << anim_type << "]" << std::endl;
            if (GameMediator::get_instance()->get_enemy(_number)->sprites[anim_type][anim_n].used == true) {
                std::cout << "###### using sprite collision rect" << std::endl;
            } else {
                std::cout << "###### using npc basic info for rect" << std::endl;
            }
            */
        }
    }



    return st_rectangle(x, y, w, h);
}

st_rectangle character::get_vulnerable_area(int anim_type)
{
    float x = position.x;
    float y = position.y;
    float w = frameSize.width;
    float h = frameSize.height;

    if (vulnerable_area_box.x == 0 && vulnerable_area_box.y == 0 && vulnerable_area_box.w == frameSize.width && vulnerable_area_box.h == frameSize.height) {
        //std::cout << "#### DEFAULT hitbox" << std::endl;
        return st_rectangle(0, 0, 0, 0);
    }

    if (vulnerable_area_box.w != 0 && vulnerable_area_box.h != 0) { // use vulnerable area
        //std::cout << "CHAR::get_vulnerable_area[" << name << "] - EXISTS - pos[" << position.x << ", " << position.y << "], x[" << vulnerable_area_box.x << "], w[" << vulnerable_area_box.w << "], h[" << vulnerable_area_box.h << "]" << std::endl;
        if (state.direction == ANIM_DIRECTION_LEFT) {
            x += vulnerable_area_box.x;
        } else {
            //std::cout << "%%%%%%% RIGHT - pos.x[" << position.x << "], vulnerable_area_box.x[" << vulnerable_area_box.x << "], hitbox.x[" << x << "]" << std::endl;
            x = position.x + frameSize.width - vulnerable_area_box.w;
        }
        y += vulnerable_area_box.y;
        w = vulnerable_area_box.w;
        h = vulnerable_area_box.h;
        //if (state.animation_type == ANIM_TYPE_SLIDE) { std::cout << "#### CHAR::get_vulnerable_area [" << name << "][" << x << "," << y << "," << w << "," << h << "]" << std::endl; }
        return st_rectangle(x, y, w, h);
    } else {
        //std::cout << "CHAR::get_vulnerable_area[" << name << "] - DO NOT EXISTS" << std::endl;
        return st_rectangle(0, 0, 0, 0);
    }


}




// ********************************************************************************************** //
// adds an entry into character_graphics_list map, if needed                                      //
// ********************************************************************************************** //
void character::add_graphic()
{
    if (name == "") {
        return;
    }

    std::map<std::string, st_char_sprite_data>::iterator it;
    const std::string temp_name(name);

    it = ImageView::get_instance()->character_graphics_list.find(name);
    if (it == ImageView::get_instance()->character_graphics_list.end()) { // there is no graphic with this key yet, add it
        std::pair<std::string, st_char_sprite_data> temp_data(temp_name, st_char_sprite_data());
        ImageView::get_instance()->character_graphics_list.insert(temp_data);
    }
}


bool character::have_frame_graphics()
{
    std::map<std::string, st_char_sprite_data>::iterator it;
    it = ImageView::get_instance()->character_graphics_list.find(name);
    if (it != ImageView::get_instance()->character_graphics_list.end()) { // there is no graphic with this key yet, add it
        for (int i=0; i<2; i++) {
			for (int j=0; j<ANIM_TYPE_COUNT; j++) {
				for (int k=0; k<ANIM_FRAMES_COUNT; k++) {
                    if ((ImageView::get_instance()->character_graphics_list.find(name)->second).frames[i][j][k].frameSurface.surface != nullptr && (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[i][k][k].frameSurface.surface->w > 0) {
						return true;
					}
				}
			}
		}
	}
    return false;
}

void character::clean_character_graphics_list()
{
    if (is_player()) {
        return;
    }
    if (ImageView::get_instance()->character_graphics_list.size() <= 0) {
        return;
    }

    std::map<std::string, st_char_sprite_data>::iterator it;
    it = ImageView::get_instance()->character_graphics_list.find(name);
    if (it != ImageView::get_instance()->character_graphics_list.end()) {
        //std::cout << "CHAR::clean_character_graphics_list[" << name << "]" << std::endl;
        ImageView::get_instance()->character_graphics_list.erase(it);
    }
}

bool character::have_background_graphics()
{
    static std::map<std::string, st_imageData>::iterator it;
    it = ImageView::get_instance()->character_graphics_background_list.find(name);
    if (it != ImageView::get_instance()->character_graphics_background_list.end()) { // there is no graphic with this key yet, add it
        return true;
    }
    return false;
}




int character::frames_count()
{
	for (int i=0; i<ANIM_FRAMES_COUNT; i++) {
        if ((ImageView::get_instance()->character_graphics_list.find(name)->second).frames[state.direction][state.animation_type][i].frameSurface.surface == nullptr || (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[state.direction][state.animation_type][i].frameSurface.surface->w == 0) {
			return i;
		}
	}
	return ANIM_FRAMES_COUNT;
}

void character::advance_to_last_frame()
{
	int frames_n = frames_count();
	if (frames_n > 0) {
		state.animation_state = frames_n - 1;
	} else {
        //std::cout << "### RESET-FRAME-N #5 ###" << std::endl;
		state.animation_state = 0;
    }
}

bool character::have_frame_graphic(int direction, int type, int pos)
{
    if (pos >= ANIM_FRAMES_COUNT) {
        return false;
    }
    if ((ImageView::get_instance()->character_graphics_list.find(name)->second).frames[direction][type][pos].frameSurface.surface == nullptr || (ImageView::get_instance()->character_graphics_list.find(name)->second).frames[direction][type][pos].frameSurface.surface->w == 0) {
		return false;
	}
    return true;
}

void character::reset_animation_type()
{
    if (get_anim_type() != ANIM_TYPE_STAND || get_anim_type() != ANIM_TYPE_WALK) {
        set_animation_type(ANIM_TYPE_STAND);
    }
}

st_characterMovements character::getMoveCommands()
{
    return moveCommands;
}

bool character::is_in_stairs_frame() const
{
	if (state.animation_type == ANIM_TYPE_STAIRS || state.animation_type == ANIM_TYPE_STAIRS_MOVE || state.animation_type == ANIM_TYPE_STAIRS_SEMI || state.animation_type == ANIM_TYPE_STAIRS_ATTACK) {
		return true;
	}
    return false;
}

bool character::is_on_attack_frame()
{
    if (state.animation_type == ANIM_TYPE_ATTACK ||
        state.animation_type == ANIM_TYPE_STAIRS_ATTACK ||
        state.animation_type == ANIM_TYPE_WALK_ATTACK ||
        state.animation_type == ANIM_TYPE_JUMP_ATTACK ||
        state.animation_type == ANIM_TYPE_THROW ||
        state.animation_type == ANIM_TYPE_SPECIAL_ATTACK ||
        state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_DOWN ||
        state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_UP ||
        state.animation_type == ANIM_TYPE_ATTACK_DOWN ||
        state.animation_type == ANIM_TYPE_ATTACK_UP ||
        state.animation_type == ANIM_TYPE_ATTACK_SPECIAL) {
        return true;
    }
    return false;
}

void character::recharge(e_energy_types _en_type, int value)
{
	//std::cout << "character::recharge";
	if (_en_type == ENERGY_TYPE_HP) {
        if (hitPoints.current < hitPoints.total) {
            if (hitPoints.current + value <= hitPoints.total) {
				hitPoints.current += value;
			} else {
                hitPoints.current = hitPoints.total;
			}

            if (value > ENERGY_ITEM_SMALL) {
                SoundView::get_instance()->play_sfx(SFX_GOT_ENERGY_BIG);
            } else {
                SoundView::get_instance()->play_sfx(SFX_GOT_ENERGY);
            }
        }
	}
}



bool character::get_item(object_collision& obj_info)
{
	bool res = false;
	// deal with non-blocking items
	if (obj_info._object != nullptr && obj_info._object->finished() == false) {
        //if (obj_info._object->get_name() != "Treasure Chest") std::cout << "character::get_item::START [" << obj_info._object->get_name() << "]" << std::endl;
        switch (obj_info._object->get_type()) {
            case OBJ_ENERGY_PILL_SMALL:
                recharge(ENERGY_TYPE_HP, ENERGY_ITEM_SMALL);
                res = true;
                obj_info._object->set_finished(true);
                break;
            case OBJ_ENERGY_PILL_BIG:
                recharge(ENERGY_TYPE_HP, ENERGY_ITEM_BIG);
                res = true;
                obj_info._object->set_finished(true);
                break;
            default:
                break;
		}
	}
    return res;
}

// returns type, or -1 if none
int character::is_executing_effect_weapon()
{
    std::vector<projectile>::iterator it;
    for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        int move_type = (*it).get_move_type();
        if (move_type == TRAJECTORY_BOMB) {
            return TRAJECTORY_BOMB;
        } else if (move_type == TRAJECTORY_QUAKE) {
            return TRAJECTORY_QUAKE;
        } else if (move_type == TRAJECTORY_FREEZE) {
            return TRAJECTORY_FREEZE;
        } else if (move_type == TRAJECTORY_CENTERED) {
            return TRAJECTORY_CENTERED;
        } else if (move_type == TRAJECTORY_PUSH_BACK) {
            return TRAJECTORY_PUSH_BACK;
        } else if (move_type == TRAJECTORY_PULL) {
            return TRAJECTORY_PULL;
        }
    }
    return -1;
}

// is all projectiles are normal (-1 or 0) return the character's max_shots,
// otherwise, find the lowest between all fired projectiles
Uint8 character::get_projectile_max_shots(bool always_charged)
{
    bool all_projectiles_normal = true;
    std::vector<projectile>::iterator it;
    short max_proj = 9;
    for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        short id = (*it).get_id();
        // if always charged, and projectile is semi-charged, count as normal
        if (id != -1 && id != 0) {
            if (always_charged == true && id == GameData::get_instance()->game_data.semi_charged_projectile_id) {
                continue;
            }
            all_projectiles_normal = false;
            short proj_max = (*it).get_max_shots();
            if (max_proj > 0 && proj_max < max_proj) {
                max_proj = proj_max;
            }
        }
    }
    if (all_projectiles_normal == true) {
        return max_projectiles;
    }
    return max_proj;
}

bool character::executeCheckSlope(int xinc, int yinc)
{
    if (_obj_jump.is_started() && _obj_jump.get_state() == JUMPUP) {
        return false;
    }
    int calc_y = slopesManager.checkSlope(xinc, yinc, get_hitbox(state.animation_state));
    if (calc_y != 0) {
        //if (is_player()) std::cout << "CHAR::executeCheckSlope - set y from currenbt[" << position.y << "] to value[" << calc_y << "]" << std::endl;
        position.y = calc_y;
        return true;
    }
    return false;
}

void character::push_back(short direction)
{
    int xinc = -(move_speed-0.2);
    if (direction == ANIM_DIRECTION_LEFT) {
        xinc = (move_speed-0.2);
    }

    //std::cout << "CHAR::PUSH_BACK - xinc: " << xinc << std::endl;

    if (test_change_position(xinc, 0)) {
        position.x += xinc;
        moved_dist.x += xinc;
    }
}

void character::pull(short direction)
{
    int xinc = (move_speed-0.2);
    if (direction == ANIM_DIRECTION_LEFT) {
        xinc = -(move_speed-0.2);
    }

    //std::cout << "CHAR::PULL - xinc: " << xinc << std::endl;

    if (test_change_position(xinc, 0)) {
        position.x += xinc;
        moved_dist.x += xinc;
    }
}

bool character::get_can_fly()
{
    return can_fly;
}

bool character::animation_has_restarted()
{
    return _was_animation_reset;
}

void character::set_animation_has_restarted(bool restarted)
{
    _was_animation_reset = restarted;
}



void character::remove_freeze_effect()
{
    state.frozen = false;
    state.frozen_timer = 0;
}


st_position character::get_int_position()
{
    return st_position((int)position.x, (int)position.y);
}

void character::add_projectile(short id, st_position pos, int trajectory, int direction)
{
    std::cout << "CHAR::add_projectile - id[" << id << "], pos[" << pos.x << "," << pos.y << "], trajectory[" << trajectory << "], direction[" << direction << "]" << std::endl;
    projectile_to_be_added_list.push_back(projectile(id, direction, pos, is_player()));
    projectile &new_projectile = projectile_to_be_added_list.back();
    new_projectile.set_is_permanent();
    new_projectile.set_trajectory(trajectory);
}


void character::check_reset_stand()
{
    if (!is_player()) { // NPCs do not need this
        return;
    }
    // is walking without moving, reset to stand
    if (moveCommands.left == 0 && moveCommands.right == 0) {
        if (state.animation_type == ANIM_TYPE_WALK) {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #C" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
        } else if (state.animation_type == ANIM_TYPE_WALK_ATTACK) {
            set_animation_type(ANIM_TYPE_ATTACK);
        }
    }
    if ((state.animation_type == ANIM_TYPE_ATTACK || state.animation_type == ANIM_TYPE_WALK_ATTACK || state.animation_type == ANIM_TYPE_JUMP_ATTACK || state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_DOWN || state.animation_type == ANIM_TYPE_ATTACK_DIAGONAL_UP) && TimerView::get_instance()->getTimer() > state.attack_timer+500) {
        switch (state.animation_type) {
            case ANIM_TYPE_WALK_ATTACK:
                //std::cout << "CHAR::check_reset_stand #1" << std::endl;
                set_animation_type(ANIM_TYPE_WALK);
                break;
            case ANIM_TYPE_JUMP_ATTACK:
                //std::cout << "CHAR::check_reset_stand #2" << std::endl;
                set_animation_type(ANIM_TYPE_JUMP);
                break;
            default:
                //std::cout << "CHAR::check_reset_stand #3" << std::endl;
                set_animation_type(ANIM_TYPE_STAND);
                break;
        }

    }

}


unsigned int character::get_projectile_count()
{
	int pcount = 0;
    std::vector<projectile>::iterator it;
	for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        pcount++;
	}
    //std::cout << "character::get_projectile_count - pcount: " << pcount << ", max_projectiles: " << max_projectiles << std::endl;
	return pcount;
}



// ********************************************************************************************** //
// set the object platform player is over, if any                                                 //
// ********************************************************************************************** //
void character::set_platform(GameObject* obj)
{
	if (obj != nullptr) {
		if (state.animation_type == ANIM_TYPE_JUMP) {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #O" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
            _obj_jump.interrupt();
        } else if (state.animation_type == ANIM_TYPE_JUMP_ATTACK) {
            set_animation_type(ANIM_TYPE_ATTACK);
            _obj_jump.interrupt();
        }
        if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #P" << std::endl;
        set_animation_type(ANIM_TYPE_STAND);
	}
	_platform = obj;
}

GameObject* character::get_platform()
{
    //std::cout << "CHAR::get_platform: " << _platform << std::endl;
	return _platform;
}

int character::get_direction() const
{
	return state.direction;
}

void character::set_direction(int direction)
{
    if (!is_player() && direction != state.direction) {
        // fix to avoid getting stuck into a wall //
        if (direction == ANIM_DIRECTION_LEFT) {
            position.x -= TILESIZE/3;
        } else {
            position.x += TILESIZE/3;
        }
    }
    if (direction != state.direction) {
        reset_dash_effect();
    }
    state.direction = direction;
}

void character::clean_projectiles()
{
	while (!projectile_list.empty()) {
        projectile_list.at(0).finish();
		projectile_list.erase(projectile_list.begin());
    }
}

void character::clean_effect_projectiles()
{
    while (true) {
        bool found_item = false;
        for (int i=0; i<projectile_list.size(); i++) {
            Uint8 move_type = projectile_list.at(i).get_move_type();
            if (move_type == TRAJECTORY_QUAKE || move_type == TRAJECTORY_FREEZE || move_type == TRAJECTORY_CENTERED || move_type == TRAJECTORY_PUSH_BACK || move_type == TRAJECTORY_PULL) {
                found_item = true;
                projectile_list.at(i).finish();
                projectile_list.erase(projectile_list.begin()+i);
                break;
            }
        }
        if (found_item == false) {
            return;
        }
    }
}

void character::damage(unsigned int damage_points, bool ignore_hit_timer = false)
{
    if (damage_points < 1) { // minimum damage is 1. if you don't want damage, don't call this method, ok? :)
        damage_points = 1;
    }
    if (damage_points + _damage_modifier > 0) {
        damage_points += _damage_modifier;
    }


	//std::cout << "1. character::damage - damage_points: " << damage_points << ", hitPoints.current: " << hitPoints.current << std::endl;

	if (hitPoints.current <= 0) { /// already dead
		return;
	}

    if (state.frozen == true && is_player()) {
        state.frozen_timer = 0;
		state.frozen = false;
	}

	unsigned int now_timer = TimerView::get_instance()->getTimer();
    if (now_timer < hit_duration+last_hit_time) { /// is still intangible from last hit
        return;
    }

    _was_hit = true;

    if (is_in_stairs_frame() == true) {
        _dropped_from_stairs = true;
    }

    last_hit_time = now_timer;
    if (now_timer > hit_duration+last_hit_time) {
        hit_animation_timer = now_timer+HIT_BLINK_ANIMATION_LAPSE;
    }

    if (!is_player()) {
        hitPoints.current -= damage_points;
        //std::cout << "CHAR::DAMAGE::damage_points: " << damage_points << ", hitPoints.current: " << hitPoints.current << std::endl;
    }


    if (is_player() == true && state.animation_type != ANIM_TYPE_HIT) {
        SoundView::get_instance()->play_sfx(SFX_PLAYER_HIT);
        set_animation_type(ANIM_TYPE_HIT);
        if (_obj_jump.is_started() == true) {
            hit_moved_back_n = get_hit_push_back_n()/2;
            _obj_jump.finish();
        } else {
            hit_moved_back_n = 0;
        }
        jump_button_released = false;
        if (GameManager::get_instance()->get_current_map_obj() != nullptr) {
            int hit_anim_x = 0;
            if (state.direction == ANIM_DIRECTION_LEFT) {
                hit_anim_x = 3;
            }
        }
	}

	// TODO: add hit animation
	if (hitPoints.current <= 0) {
		//std::cout << "1. character::damage - DEATH" << std::endl;
		hitPoints.current = 0;
		death();
    }
}

void character::damage_spikes(bool ignore_hit_timer)
{
    character::damage(SPIKES_DAMAGE);
}

bool character::is_dead() const
{
	return (hitPoints.current <= 0);
}




st_hit_points character::get_hp() const
{
	return hitPoints;
}




Uint8 character::get_current_hp() const
{
	return hitPoints.current;
}

void character::set_current_hp(Uint8 inc)
{
	hitPoints.current += inc;
}


st_position character::get_real_position() const
{
    return relativePosition;
}

void character::execute_jump_up()
{
    // fall until reaching ground
	/// @TODO
	for (int i=0; i<100; i++) {
		char_update_real_position();
		gravity();
        GameManager::get_instance()->get_current_map_obj()->show();
		show();
        GameManager::get_instance()->get_current_map_obj()->showAbove(0);
	}

    //activate_super_jump();
	// reset command jump, if any
    jump(0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    jump(1, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    //std::cout << "execute_jump::START - " << initial_y << ", position.y: " << position.y << std::endl;
    while (_obj_jump.get_speed() < 0) {
        InputController::get_instance()->read_input();
        char_update_real_position();
        jump(1, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
        GameManager::get_instance()->get_current_map_obj()->show();
		show();
        GameManager::get_instance()->get_current_map_obj()->showAbove();
        TimerView::get_instance()->delay(20);
	}
    _obj_jump.interrupt();
}

void character::execute_jump()
{
	// fall until reaching ground
	fall();

	// reset command jump, if any
    jump(0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    int initial_y = (int)position.y;
    jump(1, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
	std::cout << "execute_jump::START - " << initial_y << ", position.y: " << position.y << std::endl;
    while (position.y != initial_y) {
        InputController::get_instance()->read_input();
        //std::cout << "execute_jump::LOOP - " << initial_y << ", position.y: " << position.y << std::endl;
		char_update_real_position();
        bool resJump = jump(1, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
        if (resJump == false) {
			gravity();
		}
        GameManager::get_instance()->get_current_map_obj()->show();
		show();
        GameManager::get_instance()->get_current_map_obj()->showAbove();
        TimerView::get_instance()->delay(20);
    }
}


void character::fall()
{
    _obj_jump.finish();
    // already on the ground
    if (hit_ground() == true) {
        set_animation_type(ANIM_TYPE_STAND);
        return;
    }
    for (int i=0; i<RES_H; i++) {
		char_update_real_position();
		gravity(false);
		if (hit_ground() == true && state.animation_type == ANIM_TYPE_STAND) {
            GameManager::get_instance()->get_current_map_obj()->show();
			show();
            GameManager::get_instance()->get_current_map_obj()->showAbove();
			return;
		}
        GameManager::get_instance()->get_current_map_obj()->show();
		show();
        GameManager::get_instance()->get_current_map_obj()->showAbove();
        TimerView::get_instance()->delay(10);
    }
}

// @TODO: find first ground from bottom, that have space for player (2 tiles above are free), check 2 tiles on the x-axis also
void character::fall_to_ground()
{
    std::cout << "################## CHAR::fall_to_ground START y[" << position.y << "]" << std::endl;
    _obj_jump.finish();
    if (hit_ground() == true) {
        return;
    }
    for (int i=0; i<RES_H; i++) {
        char_update_real_position();
        position.y++;
        if (hit_ground() == true) {
            //std::cout << "################## CHAR::fall_to_ground STOP - y[" << position.y << "]" << std::endl;
            return;
        } else {
            //std::cout << "################## CHAR::fall_to_ground CONTINUE - y[" << position.y << "]" << std::endl;
        }
    }
    //std::cout << "################## CHAR::fall_to_ground::END y[" << position.y << "]" << std::endl;
}

void character::initialize_position_to_ground()
{
    if (can_fly == true) {
        return;
    }
    //if (is_player() == false) std::cout << ">>> character::initialize_position_to_ground::START[" << name << "]" << std::endl;
    // RES_H is a good enough limit
    for (int i=0; i<RES_H; i++) {
        char_update_real_position();
        //if (is_player() == false) std::cout << ">>> character::initialize_position_to_ground::EXECUTE[" << name << "], y[" << position.y << "], relative.y[" << relativePosition.y << "]" << std::endl;
        gravity(false);
        if (hit_ground() == true) {
            break;
        }
    }
}

bool character::change_position(short xinc, short yinc)
{
    st_map_collision map_col = map_collision(xinc, yinc, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
    short int mapLock = map_col.block;
    //if (is_player()) std::cout << "*** character::change_position - x: " << position.x << ", y: " << position.y << ", xinc: " << xinc << ", yinc: " << yinc << ", BLOCKED (" << mapLock << ")" << std::endl;

	if (mapLock != BLOCK_UNBLOCKED && mapLock != BLOCK_WATER) {
        if (xinc > 0) {
            std::cout << "CAN't MOVE on X AXIS" << std::endl;
        }
		return false;
	}
	position.x += xinc;
    moved_dist.x += xinc;
    //std::cout << "### STAIRS-DOWN #6 ###" << std::endl;
	position.y += yinc;
    moved_dist.y += yinc;
    return true;
}

void character::change_position_x(short xinc)
{
    if (xinc == 0) { // nothing todo
        return;
    }
    for (int i=xinc; i>=0.1; i--) {
        if (state.animation_type == ANIM_TYPE_HIT && hit_ground() == true) {
            hit_moved_back_n += xinc;
        }

        st_map_collision map_col = map_collision(i, 0, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
        int mapLock = map_col.block;
        //mapLock =  gameManager::get_instance()->getMapPointLock(st_position((position.x + frameSize.width + i)/TILESIZE, (position.y + frameSize.height/2)/TILESIZE));
        if (mapLock == BLOCK_UNBLOCKED || mapLock == BLOCK_WATER || mapLock == BLOCK_Y) {
            //std::cout << "character::charMove - temp_move_speed: " << temp_move_speed << ", gameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x: " << gameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x << std::endl;
            if (mapLock == TERRAIN_UNBLOCKED || (mapLock == BLOCK_WATER && abs((float)i*WATER_SPEED_MULT) < 1) || mapLock == BLOCK_Y) {
                position.x += i - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                moved_dist.x += i - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
            } else {
                position.x += i*WATER_SPEED_MULT - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
                moved_dist.x += i*WATER_SPEED_MULT - GameManager::get_instance()->get_current_map_obj()->get_last_scrolled().x;
            }
            if (state.animation_type != ANIM_TYPE_HIT) {
                set_direction(ANIM_DIRECTION_RIGHT);
            } else {
                gravity(false);
                return;
            }
            if (state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_WALK_ATTACK) {
                state.animation_timer = 0;
            }
            if (state.animation_type != ANIM_TYPE_WALK && state.animation_type != ANIM_TYPE_JUMP && state.animation_type != ANIM_TYPE_SLIDE && state.animation_type != ANIM_TYPE_JUMP_ATTACK && state.animation_type != ANIM_TYPE_HIT && (state.animation_type != ANIM_TYPE_WALK_ATTACK || (state.animation_type == ANIM_TYPE_WALK_ATTACK && state.attack_timer+ATTACK_DELAY < TimerView::get_instance()->getTimer()))) {
                set_animation_type(ANIM_TYPE_WALK);
            }
            position.x += xinc;
            moved_dist.x += xinc;
            break;
        }
    }
}

int character::change_position_y(short yinc)
{
    if (yinc == 0) { // nothing todo
        return 0;
    }
    if (test_change_position(0, yinc)) { // can move max
        return yinc;
    } else { // check decrementing xinc
        if (yinc > 0) {
            for (int i=yinc; i>0; i--) {
                if (test_change_position(0, i)) {
                    return i;
                }
            }
        } else {
            for (int i=yinc; i<0; i++) {
                if (test_change_position(0, i)) {
                    return i;
                }
            }
        }
    }
    return 0;
}

bool character::test_change_position(short xinc, short yinc)
{
    if (GameManager::get_instance()->get_current_map_obj() == nullptr) {
        return false;
    }
    if (yinc < 0 && position.y < 0) {
		return false;
	}
    if (yinc > 0 && position.y > RES_H) {
        return true;
	}
    if (xinc < 0 && position.x <= 0) {
        return false;
    }

    if (xinc > 0 && (relativePosition.x - frameSize.width) > RES_W) {
        return false;
    }

    if (is_ghost == false) {
        st_map_collision map_col = map_collision(xinc, yinc, GameManager::get_instance()->get_current_map_obj()->getMapScrolling());
        short int mapLock = map_col.block;

        if (mapLock != BLOCK_UNBLOCKED && mapLock != BLOCK_WATER) {
            return false;
        }
    }

    // check wall-locks
    int map_x_point = (position.x+xinc);
    bool map_wall = GameManager::get_instance()->get_current_map_obj()->get_map_point_wall_lock(map_x_point);

    if (map_wall == true) {
        return false;
    }

    return true;
}


bool character::is_shielded(int projectile_direction) const
{
	if (is_player()) {
        if (InputController::get_instance()->p1_input[BTN_ITEM] == 1 && state.animation_type == ANIM_TYPE_SHIELD) { // player is on SHIELD animation and is keeping the shield button pressed
            if (shield_type == SHIELD_FULL || shield_type == SHIELD_FRONT) { // player can use shield
                return true;
            }
        }
		return false;
	} else {
		//std::cout << ">> classnpc::is_shielded[" << name << "] - shield_type: " << shield_type << ", projectile_direction: " << projectile_direction << ", state.direction: " << state.direction << std::endl;
        if (shield_type == SHIELD_FULL || (shield_type == SHIELD_FRONT && projectile_direction != state.direction && (state.animation_type == ANIM_TYPE_STAND || state.animation_type == ANIM_TYPE_WALK  || state.animation_type == ANIM_TYPE_WALK_AIR)) || (shield_type == SHIELD_STAND && state.animation_type == ANIM_TYPE_STAND)) {
			//std::cout << ">> classnpc::is_shielded[" << name << "] - TRUE" << std::endl;
			return true;
		}
        if (shield_type == SHIELD_STAND_AND_WALK && projectile_direction != state.direction && (state.animation_type == ANIM_TYPE_STAND || state.animation_type == ANIM_TYPE_WALK)) {
            return true;
        }
        if (shield_type == SHIELD_STAND_FRONT && projectile_direction != state.direction && state.animation_type == ANIM_TYPE_STAND) {
            return true;
        }
		//std::cout << ">> classnpc::is_shielded[" << name << "] - FALSE" << std::endl;
		return false;
    }
}

bool character::is_intangible()
{
    if (is_player()) {
        return false;
    }
    if (state.animation_type == ANIM_TYPE_STAND && shield_type == SHIELD_DISGUISE) {
        return true;
    }
    return false;
}



short character::get_anim_type() const
{
	return state.animation_type;
}

st_imageData *character::get_char_frame(int direction, int type, int frame)
{
    if (ImageView::get_instance()->character_graphics_list.find(name) == ImageView::get_instance()->character_graphics_list.end()) {
        return nullptr;
	} else {
        return &(ImageView::get_instance()->character_graphics_list.find(name)->second).frames[direction][type][frame].frameSurface;
	}
}

st_color character::get_color_key(short key_n) const
{
	return color_keys[key_n];
}

short character::get_number() const
{
	return _number;
}


bool character::is_using_circle_weapon()
{
    if (projectile_list.size() == 1) {
        if (projectile_list.at(0).get_move_type() == TRAJECTORY_CENTERED) {
            std::cout << "CHAR::is_using_circle_weapon[" << name << "] - TRUE" << std::endl;
            return true;
        }
    }
    return false;
}

void character::inc_effect_weapon_status()
{
    if (projectile_list.size() == 1) {
        int move_type = projectile_list.at(0).get_move_type() ;
        if (move_type == TRAJECTORY_CENTERED || move_type == TRAJECTORY_BOMB) {
            projectile_list.at(0).inc_status();
        }
    }
}

void character::set_animation_type(ANIM_TYPE type)
{
    //std::cout << "character::set_animation_type type[" << type << "]" << std::endl;
    // if is hit, finish jumping
    if (state.animation_type != type && type == ANIM_TYPE_HIT) {
        _obj_jump.finish();
    }

    if (type != state.animation_type) {
        //std::cout << "### RESET-FRAME-N #6 ###" << std::endl;
        //state.animation_type = ANIM_TYPE_STAND;

        if (is_in_stairs_frame() && type == ANIM_TYPE_HIT) {
            if (state.direction == ANIM_DIRECTION_RIGHT) {
                position.x += 2;
            }
        }

        if (type == ANIM_TYPE_STAIRS) {
            //std::cout << ">>> SET STAIRS ANIM TYPE" << std::endl;
        } else if (type == ANIM_TYPE_STAIRS_MOVE) {
            //std::cout << ">>> SET STAIRS MOOOVE ANIM TYPE, timer: " << TimerView::get_instance()->getTimer() << ", _stairs_falling_timer: " << _stairs_falling_timer << std::endl;
        } else if (type == ANIM_TYPE_JUMP) {
            //std::cout << ">>> SET JUMP ANIM TYPE" << std::endl;
        }
        state.animation_type = type;
        _was_animation_reset = false;


        // avoids slides starting inside wall or object
        if (type == ANIM_TYPE_SLIDE) {
            if (state.direction == ANIM_DIRECTION_LEFT) {
                position.x += 6;
            } else {
                position.x -= 6;
            }
        }
    }
    int frame_delay = 20;
    if (ImageView::get_instance()->character_graphics_list.find(name) != ImageView::get_instance()->character_graphics_list.end()) {
        if (state.direction >= CHAR_ANIM_DIRECTION_COUNT) {
            set_direction(0);
        }
        if (state.animation_type >= ANIM_TYPE_COUNT) {
            state.animation_type = 0;
        }
        if (state.animation_state >= ANIM_FRAMES_COUNT) {
            state.animation_state = 0;
        }

        frame_delay = ImageView::get_instance()->character_graphics_list.find(name)->second.frames[state.direction][state.animation_type][state.animation_state].delay;
    }
    state.animation_timer = TimerView::get_instance()->getTimer() + frame_delay;
    animation_obj.set_type(static_cast<ANIM_TYPE>(state.animation_type));
}

void character::set_animation_frame(unsigned int frame)
{
    state.animation_state = frame;
}


void character::set_progressive_appear_pos(int pos)
{
    _progressive_appear_pos = pos;
}

bool character::is_stage_boss()
{
    return _is_stage_boss;
}


bool character::can_air_dash()
{
    return false;
}



void character::cancel_slide()
{
    state.slide_distance = 999;
    if (state.animation_type == ANIM_TYPE_SLIDE) {
        if (hit_ground() == true) {
            if (name == _debug_char_name) std::cout << "CHAR::RESET_TO_STAND #Y.4" << std::endl;
            set_animation_type(ANIM_TYPE_STAND);
        } else {
            if (is_player()) std::cout << "CHAR::RESET_TO_JUMP #A.7" << std::endl;
            set_animation_type(ANIM_TYPE_JUMP);
        }
    }
}

float character::get_hit_push_back_n()
{
    return TILESIZE*0.8;
}

bool character::have_shoryuken()
{
    return false;
}

int character::get_armor_arms_attack_id()
{
    return -1;
}






