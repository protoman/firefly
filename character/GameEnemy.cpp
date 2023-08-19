#include <cstdlib>
#include <sstream>
#include "GameEnemy.h"
#include "classplayer.h"
#include <math.h>
#include <string.h>

#include "game_data.h"
#include "GameManager.h"
#include "data/shareddata.h"
#include "view/imageview.h"
#include "view/timerview.h"


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
GameEnemy::GameEnemy() : graphic_filename(), first_run(true), _is_player_friend(false)
{
	add_graphic();
	hit_duration = 500;
    if (is_subboss()) {
        hit_duration = BOSS_HIT_DURATION;
    }
	clean_projectiles();
    facing = 0;
    _is_spawn = false;
    _initialized = 0;
    _screen_blinked = false;
    _parent_id = -1;
    is_ghost = false;
}


GameEnemy::GameEnemy(int map_id, int main_id, int id) : _is_player_friend(false) // map-loaded enemy
{
    is_ghost = false;
    build_basic_enemy(map_id, main_id);
    facing = GameData::get_instance()->file_v5_stage_enemy_map.at(map_id).at(id).direction;
    set_direction(facing);

    fflush(stdout);

    start_point.x = ( GameData::get_instance()->file_v5_stage_enemy_map.at(map_id).at(id).start_point.x * TILESIZE) + GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.x;
    if (GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.x != 0) {
        std::cout << ">>>>>>>>>>>>> bg_pos.x[" << GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.x << "]" << std::endl;
    }
    start_point.y = ( GameData::get_instance()->file_v5_stage_enemy_map.at(map_id).at(id).start_point.y * TILESIZE) + GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.y;
    static_bg_pos = st_position( GameData::get_instance()->file_v5_stage_enemy_map.at(map_id).at(id).start_point.x * TILESIZE,  GameData::get_instance()->file_v5_stage_enemy_map.at(map_id).at(id).start_point.y * TILESIZE);
    position.x = start_point.x;
    position.y = start_point.y;
    if (name == "OCTOPUS") {
        std::cout << "ENEMY[" << name << "], x[" << position.x << "], y[" << position.y << "]" << std::endl;
    }
    _is_spawn = false;
    _initialized = 0;
    _screen_blinked = false;
    _parent_id = -1;

    is_ghost = false;

    if (is_static()) {
        can_fly = true;
    }
}

GameEnemy::GameEnemy(int map_id, int main_id, st_position enemy_pos, short int direction, bool player_friend) // spawned enemy
{
    is_ghost = false;
    build_basic_enemy(map_id, main_id);
    _is_player_friend = player_friend;
    facing = direction;
    set_direction(direction);
    start_point.x = enemy_pos.x;
    start_point.y = enemy_pos.y;
    static_bg_pos = st_position(enemy_pos.x * TILESIZE, enemy_pos.y * TILESIZE);
    position.x = enemy_pos.x;
    position.y = enemy_pos.y;
    _is_spawn = true;
    _initialized = 0;
    _screen_blinked = false;
    _parent_id = -1;

    is_ghost = false;
    if (is_static()) {
        can_fly = true;
    }
}




// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
GameEnemy::GameEnemy(std::string set_name) : graphic_filename(), first_run(true), _is_player_friend(false)
{
	name = set_name;
	hit_duration = HIT_BLINK_ANIMATION_LAPSE;
	add_graphic();
    facing = 0;
    _is_spawn = false;
    _initialized = 0;
    _screen_blinked = false;
    _parent_id = -1;

    is_ghost = false;
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
GameEnemy::~GameEnemy()
{
}

void GameEnemy::build_basic_enemy(int map_id, int main_id)
{
	_number = main_id;
	// TODO - usar operador igual e também para cópia de toda a classe para ela mesma
    st_imageData enemy_sprite_surface;

    file_enemy_v3_1_2* copyref = GameData::get_instance()->get_enemy(main_id);

    name = std::string(copyref->name);

	add_graphic();


    hitPoints.total = GameData::get_instance()->get_enemy(main_id)->hp;
	hitPoints.current = hitPoints.total;

    if (state.direction > CHAR_ANIM_DIRECTION_COUNT) {
        set_direction(ANIM_DIRECTION_RIGHT);
	}

    move_speed = GameData::get_instance()->get_enemy(main_id)->speed;
    walk_range = GameData::get_instance()->get_enemy(main_id)->walk_range;
	if (walk_range < 0 || walk_range > 1000) { // fix data errors by setting value to default
		walk_range = TILESIZE*6;
	}

    graphic_filename = GameData::get_instance()->get_enemy(main_id)->graphic_filename;


    frameSize.width = GameData::get_instance()->get_enemy(main_id)->frame_size.width;
    frameSize.height = GameData::get_instance()->get_enemy(main_id)->frame_size.height;
    is_ghost = (GameData::get_instance()->get_enemy(main_id)->is_ghost != 0);
    shield_type = GameData::get_instance()->get_enemy(main_id)->shield_type;
	_is_boss = false;

    _attack_frame_n = GameData::get_instance()->get_enemy(main_id)->attack_frame;

    // TODO - this logic can be passed to the editor
    if (GameData::get_instance()->get_enemy(main_id)->fly_flag != 0) {
        //std::cout << "******** GameEnemy::set_file_data - enemy: " << name << ", canfly: " << can_fly << std::endl;
        can_fly = true;
    }
	relativePosition.x = 0;
	relativePosition.y = 0;

	max_projectiles = 1;			// hardcoded, fix this in editor
	//move_speed = 6;					// hardcoded, fix this in editor
	attack_state = 0;
	last_execute_time = 0;


    // only add graphics if there is no graphic for this enemy yet
	if (have_frame_graphics() == false) {
		// load the graphic from file and set it into frameset


        std::string temp_filename = SharedData::get_instance()->FILEPATH + "images/sprites/enemies/" + graphic_filename;
        //printf(">> temp_filename: '%s'\n", temp_filename.c_str());

        enemy_sprite_surface = ImageView::get_instance()->imageFromFile(temp_filename);
        if (enemy_sprite_surface.surface == nullptr) {
            std::cout << "initFrames - Error loading enemy surface from file\n";
			return;
		}

        for (int i=0; i<ANIM_TYPE_COUNT; i++) {
            for (int j=0; j<ANIM_FRAMES_COUNT; j++) {
                if (GameData::get_instance()->get_enemy(main_id)->sprites[i][j].used == true) {
                    addSpriteFrame(i, GameData::get_instance()->get_enemy(main_id)->sprites[i][j].sprite_graphic_pos_x, enemy_sprite_surface, GameData::get_instance()->get_enemy(main_id)->sprites[i][j].duration);
                }
            }
        }
    }


    if (have_background_graphics() == false) {
        st_imageData bg_surface;
        std::string bg_filename(GameData::get_instance()->get_enemy(main_id)->bg_graphic_filename);
        //std::cout << ">>>>>>>>> enemy[" << name << "].bg_filename: '" << bg_filename << "', length: " << bg_filename.length() << ", size: " << bg_filename.size() << std::endl;
        if (bg_filename.size() > 0) {
            std::string full_bggraphic_filename = SharedData::get_instance()->FILEPATH + "images/sprites/enemies/backgrounds/" + bg_filename;
            bg_surface = ImageView::get_instance()->imageFromFile(full_bggraphic_filename);
            if (bg_surface.surface == nullptr) {
                std::cout << "initFrames - Error loading Enemy background surface from file '" << full_bggraphic_filename << std::endl;
                return;
            }
            /*
            ImageView::get_instance()->character_graphics_background_list.insert(std::pair<std::string, st_imageData>(name, bg_surface));
            _has_background = true;
            */
        }
    }

    // can't have ghosts that don't fly
    if (is_ghost == true && can_fly == false) {
        can_fly = true;
    }
    if (is_static()) {
        can_fly = true;
    }


    vulnerable_area_box = GameData::get_instance()->get_enemy(_number)->vulnerable_area;
}



bool GameEnemy::is_able_to_fly()
{
    return can_fly;
}

bool GameEnemy::is_spawn()
{
    return _is_spawn;
}

bool GameEnemy::is_subboss()
{
    return GameData::get_instance()->get_enemy(_number)->is_sub_boss;
}

void GameEnemy::reset_position()
{
    position.x = start_point.x;
    position.y = start_point.y;
    // if the enemy uses fly/fall, it means, we need to respawn it inside the hole
    if (uses_fly_fall()) {
        position.y = RES_H + TILESIZE;
    }
}

st_position GameEnemy::get_start_position()
{
    return st_position(start_point.x, start_point.y);
}

st_position GameEnemy::get_bg_position()
{
    return static_bg_pos;
}


void GameEnemy::show()
{
#ifdef SHOW_HITBOXES
    st_rectangle hitbox = get_hitbox();
    hitbox.x -= gameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
    ImageView::get_instance()->draw_rectangle(hitbox, 0, 0, 255, 100);
#endif
    artificial_inteligence::show();
    // show tooltip, if needed
    //std::cout << "ENEMY.SHOW[" << name << "]" << std::endl;
}

bool GameEnemy::enemy_is_ghost()
{
    return is_ghost;
}

void GameEnemy::enemy_set_position(st_float_position pos)
{
    position = pos;
}

void GameEnemy::enemy_set_direction(short dir)
{
    set_direction(dir);
}

void GameEnemy::enemy_set_initialized(short init)
{
    _initialized = init;
}

void GameEnemy::set_parent_id(int parent_id)
{
    _parent_id = parent_id;
}

int GameEnemy::get_parent_id()
{
    return _parent_id;
}

void GameEnemy::reset_timers()
{
    character::reset_sprite_animation_timer();
}

bool GameEnemy::is_static()
{
    if (GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.x != 0 && GameData::get_instance()->get_enemy(_number)->sprites_pos_bg.y != 0) {
        return true;
    }
    return false;
}

int GameEnemy::get_id()
{
    return _number;
}

void GameEnemy::enemy_set_hp(st_hit_points new_hp)
{
    hitPoints = new_hp;
}




// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void GameEnemy::initFrames()
{
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void GameEnemy::execute()
{
    if (is_dead() == true) {
        move_projectiles();
    } else {
        if (is_boss() || is_stage_boss()) {
            boss_move();
        } else {
            move();
        }
        charMove();
    }
}

void GameEnemy::init_animation()
{
    animation_obj.init(name, SharedData::get_instance()->FILEPATH + "images/sprites/enemies/" + graphic_filename, frameSize,  GameData::get_instance()->get_enemy(_number)->sprites);
}

void GameEnemy::boss_move()
{
    //std::cout << "Enemy::boss_move::BEGIN" << std::endl;
    if (hitPoints.current <= 0 || position.x < GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x-TILESIZE*2 || position.x > GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x+RES_W+TILESIZE*2) {
        //std::cout << "classboss::execute - LEAVE #1" << std::endl;
        return;
    }
    if (last_execute_time > TimerView::get_instance()->getTimer()) {
        //std::cout << "classboss::execute - LEAVE #2" << std::endl;
        return;
    }

    //std::cout << "classboss::boss_move[" << name << "] - _initialized: " << _initialized << std::endl;

    move_projectiles();
    bool is_static_boss = is_static();

    if (is_entirely_on_screen() == true && _initialized == 0 && _is_boss == true) { /// @TODO: move this logic to map (player should not move while boss is presenting)
        _initialized++;
        set_animation_type(ANIM_TYPE_TELEPORT);
        GameManager::get_instance()->map_present_boss(is_stage_boss(), is_static_boss);
        // set temp-background in map
        return;
    } else if (is_entirely_on_screen() == false && is_on_screen() == true &&  _initialized == 0 && _is_boss == true) {
        fall_to_ground();
        _initialized = 1;
        return;
    } else if (_initialized == 1 && _is_boss == true && is_static_boss == false) {
        if (position.x > RES_H/3 && gravity(true) == false) {
            _initialized++;
        }
        return;
    }

    if (first_run == 0) {
        first_run = 1;
    }

    if (_ai_timer > TimerView::get_instance()->getTimer()) {
        return;
    }

    execute_ai();
    gravity(false);
}



void GameEnemy::copy(GameEnemy *from)
{
	int i;

	facing = from->facing;
	start_point = from->start_point;
    static_bg_pos = from->static_bg_pos;

	walk_range = from->walk_range;
	graphic_filename = from->graphic_filename;
    is_ghost = from->is_ghost;
	shield_type = from->shield_type;

	_is_boss = from->_is_boss;
	hitPoints = from->hitPoints;
	name = from->name;
	position = from->position;
	relativePosition = from->relativePosition;
	last_execute_time = 0;
	frameSize = from->frameSize;
	moveCommands = from->moveCommands;
	max_projectiles = from->max_projectiles;
	move_speed = from->move_speed;
    can_fly = from->can_fly;
    attack_state = 0;
	state = from->state;

}



void GameEnemy::move_projectiles()
{
	//int i = 0;
	// animate projectiles
    //if (name == "Dynamite Bot") std::cout << "******* Enemy::move_projectiles - projectile_list.size: " << projectile_list.size() << std::endl;
    std::vector<projectile>::iterator it;
    st_rectangle player_hitbox = GameManager::get_instance()->get_current_map_obj()->get_player_hitbox();

	for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        st_size moved = (*it).move();
        // check collision agains players

        if ((*it).is_reflected == true) {
			continue;
		}

        if (_is_player_friend == false) { // Enemy attacking players

            if ((*it).is_finished == true) {
                projectile_list.erase(it);
                break;
            }
            if ((*it).get_move_type() == TRAJECTORY_QUAKE) {
                ground_damage_players();
                continue;
            } else if ((*it).get_move_type() == TRAJECTORY_PUSH_BACK) {
                //@TODO
                push_back_players(!state.direction);
                continue;
            } else if ((*it).get_move_type() == TRAJECTORY_PULL) {
                //@TODO
                pull_players(!state.direction);
                continue;
            }

            if ((*it).check_collision(player_hitbox, st_position(moved.width, moved.height)) == true) {
                if (GameManager::get_instance()->get_player()->is_shielded((*it).get_direction()) == true && (*it).get_trajectory() != TRAJECTORY_BOMB && (*it).get_trajectory() != TRAJECTORY_LIGHTING&& (*it).get_trajectory() != TRAJECTORY_SLASH) {
                    (*it).reflect();
                } else if (GameManager::get_instance()->get_player()->is_using_circle_weapon() == true) {
                    //std::cout << "consume_projectile #0" << std::endl;
                    (*it).consume_projectile();
                    GameManager::get_instance()->get_player()->consume_projectile();
                } else {
                    int damage_pts = (*it).get_damage();
                    if (damage_pts < 2) {
                        damage_pts = 2;
                    }
                    GameManager::get_instance()->get_player()->damage(damage_pts, false);
                    if ((*it).get_vanishes_on_hit() == true) {
                        //std::cout << "consume_projectile #1" << std::endl;
                        (*it).consume_projectile();
                    }
                }
            }
        } else { // Enemy attacking other Enemies

            for (unsigned int i=0; i<GameManager::get_instance()->get_current_map_obj()->map_enemy_list.size(); i++) {
                st_rectangle other_enemy_hitbox = GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).get_vulnerable_area();
                if (other_enemy_hitbox.is_empty() == false && (*it).check_collision(other_enemy_hitbox, st_position(moved.width, moved.height)) == true) {
					//std::cout << "is_shielded::CALL 2" << std::endl;
                    if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_intangible() == true) {
                        continue;
                    } else if (GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).is_shielded((*it).get_direction()) == true && (*it).get_trajectory() != TRAJECTORY_BOMB && (*it).get_trajectory() != TRAJECTORY_LIGHTING&& (*it).get_trajectory() != TRAJECTORY_SLASH) {
                        (*it).reflect();
					} else {
                        GameManager::get_instance()->get_current_map_obj()->map_enemy_list.at(i).damage((*it).get_damage(), false);
                        if ((*it).get_move_type() != TRAJECTORY_CHAIN) { /// @TODO non-destructable types
                            //std::cout << "consume_projectile #2" << std::endl;
                            (*it).consume_projectile();
						}
					}
				}
                if ((*it).is_finished == true) {
					projectile_list.erase(it);
					break;
				}
			}
		}
	}
    projectile_list.insert(projectile_list.end(), projectile_to_be_added_list.begin(), projectile_to_be_added_list.end());
    projectile_to_be_added_list.clear();
}

void GameEnemy::show_projectiles()
{
    std::vector<projectile>::iterator it;
    for (it=projectile_list.begin(); it<projectile_list.end(); it++) {
        (*it).draw();
    }
}


// executes the Enemy sub-IA behavior
void GameEnemy::move() {
    if (state.direction > CHAR_ANIM_DIRECTION_COUNT-1) {
        set_direction(ANIM_DIRECTION_LEFT);
    }

    /// @TODO: this check must be placed in game.cpp and Enemy must have a set_frozen() method, for individual effect

    move_projectiles();


    if (last_execute_time > TimerView::get_instance()->getTimer()) {
		return;
	}

    if (state.frozen == true) {
		return;
	}


    if (hitPoints.current > 0) {
        execute_ai();
	}

    //std::cout << "GameEnemy::move - position.y: " << position.y << std::endl;

    if (can_fly == false && position.y >= RES_H+1) {
        // death because felt in a hole
        damage(999, true);
        position.x = start_point.x;
        position.y = start_point.y;
        //std::cout << "GameEnemy::move - FELT IN HOLE - DEATH - pos.x: " << position.x << ", pos.y: " << position.y << std::endl;
        return;
    }

}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //



short GameEnemy::get_dead_state()
{
    if (hitPoints.current > 0) {
        _dead_state = 0;
    } else {
        if (_dead_state == 0) {
            death();
            _dead_state = 1;
        } else if (_dead_state == 1) {
            _dead_state = 2;
        }
    }
    return _dead_state;
}


void GameEnemy::death()
{
    _obj_jump.interrupt();
    _obj_jump.finish();
    dead = true;
    _auto_respawn_timer = TimerView::get_instance()->getTimer() + GameData::get_instance()->get_enemy(_number)->respawn_delay;
    if (is_stage_boss()) {
        GameManager::get_instance()->get_current_map_obj()->clear_animations();
    }
}

bool GameEnemy::is_boss()
{
	return _is_boss;
}




void GameEnemy::set_is_boss(bool set_boss)
{
    _is_boss = set_boss;
    if (_is_boss == true) {
        _screen_blinked = false;
        // only set initial y if not fixed position type
        if (is_static() == false) {
            fall_to_ground();
            _ai_state.initial_position.y = position.y;
        } else {
            position.y = start_point.y;
        }
        hitPoints.total = BOSS_INITIAL_HP;
        hitPoints.current = hitPoints.total;
        hit_duration = BOSS_HIT_DURATION;
    }
}

bool GameEnemy::is_player_friend()
{
	return _is_player_friend;
}

void GameEnemy::set_stage_boss(bool boss_flag)
{
    _is_boss = boss_flag;
    _is_stage_boss = boss_flag;
    if (boss_flag == true) {
        _screen_blinked = false;
        if (is_static() == false) {
            _ai_state.initial_position.y = -(frameSize.height+1);
            position.y = _ai_state.initial_position.y;
        } else {
            position.y = start_point.y;
        }
        hitPoints.total = BOSS_INITIAL_HP;
        hitPoints.current = hitPoints.total;
        hit_duration = BOSS_HIT_DURATION;
    }
}

void GameEnemy::revive()
{
    //std::cout << "**** GameEnemy::revive[" << name << " ****" << std::endl;
	//position.x = start_point.x;
    //position.y = start_point.y;
    if (GameData::get_instance()->ai_list.at(_number).states[_ai_chain_n].extra_parameter == AI_ACTION_FLY_OPTION_DRILL_DOWN) {
        position.y = -TILESIZE;
    }
	hitPoints.current = hitPoints.total;
}





void GameEnemy::invert_direction()
{
	if (state.direction == ANIM_DIRECTION_LEFT) {
        set_direction(ANIM_DIRECTION_RIGHT);
	} else {
        set_direction(ANIM_DIRECTION_LEFT);
	}
    _ai_state.main_status = IA_STAND;
}




