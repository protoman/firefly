#include <cstring>
#include <cstdlib>
#include "GameManager.h"

#include "data/shareddata.h"

#include "view/option_picker.h"
#include "view/textview.h"
#include "defines.h"
#include "file/file_io.h"
#include "strings_map.h"

#include "controller/inputcontroller.h"
#include "options/pausemenu.h"

#include "aux_tools/fps_control.h"

#include "view/ingame_presentation.h"


GameManager* GameManager::_instance = nullptr;

#define DEATH_ANIMATION_DELAY 120

// ********************************************************************************************** //
// class constructor                                                                              //
// ********************************************************************************************** //
GameManager::GameManager() : _show_boss_hp(false), player1(0)
{
	_frame_duration = 1000/80; // each frame must use this share of time
    invencible_old_value = false;
    _dark_mode = false;
    SharedData::get_instance()->is_showing_boss_intro = false;
    current_save_slot = 0;
    show_fps_enabled = true;

    map_interstage_points.push_back(st_position(11, 27));
    map_interstage_points.push_back(st_position(54, 31));
    map_interstage_points.push_back(st_position(105, 27));
    map_interstage_points.push_back(st_position(160, 70));
    map_interstage_points.push_back(st_position(198, 7));
    map_interstage_points.push_back(st_position(288, 26));
    map_interstage_points.push_back(st_position(265, 92));
    map_interstage_points.push_back(st_position(161, 121));
    map_interstage_points.push_back(st_position(78, 120));
    map_interstage_points.push_back(st_position(41, 174));
    map_interstage_points.push_back(st_position(120, 204));
    map_interstage_points.push_back(st_position(201, 205));
    map_interstage_points.push_back(st_position(203, 170));
    map_interstage_points.push_back(st_position(282, 171));

}

// ********************************************************************************************** //
// class destructor                                                                               //
// ********************************************************************************************** //
GameManager::~GameManager()
{
}


// ********************************************************************************************** //
// initializar game, can't be on constructor because it needs other objects (circular)            //
// ********************************************************************************************** //
GameManager *GameManager::get_instance()
{
    if (!_instance) {
        _instance = new GameManager();
    }
    return _instance;
}

void GameManager::initHardwareLayer()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO | SDL_INIT_HAPTIC) < 0) {
        std::cout << "SDL could not initialize! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }
    SharedData::get_instance()->window = SDL_CreateWindow( "Project Firefly ALPHA v0.0.1", RES_W, RES_H, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL );
    if (SharedData::get_instance()->window == nullptr) {
        std::cout << "Window could not be created! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    //SDL_Window *window, const char *name
    gRenderer = SDL_CreateRenderer(SharedData::get_instance()->window, nullptr);
    if (gRenderer == nullptr) {
        std::cout << "Renderer could not be created! SDL Error: [" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }


    InputController::get_instance()->init();
    ImageView::get_instance()->init();
    TextView::get_instance()->init();
    SoundView::get_instance()->init();

    //Get window surface
    SharedData::get_instance()->screenSurface = SDL_GetWindowSurface(SharedData::get_instance()->window);
    SDL_UpdateWindowSurface(SharedData::get_instance()->window);

}

void GameManager::preloadGameData()
{
    GameData::get_instance()->load_data();
    mapController.loadMap();
    ImageView::get_instance()->preload();
    Draw::get_instance()->preload();
    read_save();

}

void GameManager::introScreen()
{
    game_menu menu;
    menu.show_intro_menu();
}



int GameManager::mapNumberFromAreaPosition(int area_n, int x, int y)
{
    // @TODO: optimize and store links number on loading //
    int res = -1;
    /*
    if (area_n == SharedData::get_instance()->v6_selected_area) {
        res = SharedData::get_instance()->v6_current_level_data.rooms[x][y].area_n;
    } else {
        file_v6_level_point new_level_data = fio_cmm.load_single_object_from_list<file_v6_level_point>(SharedData::get_instance()->FILEPATH + "/" + FILE_V6_LEVEL_LIST, area_n);
        res = new_level_data.rooms[x][y].area_n;
    }
    */
    // DEBUG //
    res = 0;
    std::cout << "gameManager::mapNumberFromAreaPosition - area[" << area_n << "], x[" << x << "], y[" << y << "], res[" << res << "]" << std::endl;
    return res;
}

bool GameManager::check_map_link(int xinc, int yinc)
{
    if (xinc == 0 && yinc == 0) {
        std::cout << "check_map_link::LEAVE #1" << std::endl;
        return false;
    }

    st_rectangle hitbox = player1.get_hitbox();

    int px = hitbox.x-xinc+hitbox.w/2;
    if (xinc > 0) {
        px = hitbox.x+hitbox.w+xinc;
    }
    int py = hitbox.y-yinc;
    if (yinc > 0) {
        py = hitbox.y+hitbox.h+yinc;
    }
    int tile_x = px/TILESIZE;
    int tile_y = py/TILESIZE;
    if (tile_x < 0) {
        tile_x = 0;
    } else if (tile_x > mapController.get_size().width-1) {
        tile_x = mapController.get_size().width-1;
    }
    if (tile_y < 0) {
        tile_y = 0;
    } else if (tile_y > mapController.get_size().height-1) {
        tile_y = mapController.get_size().height-1;
    }

    std::cout << "######### gameManager::check_map_link - xinc[" << xinc << "], player.pos.x[" << player1.get_int_position().x << "], px[" << px << "], tile_x[" << tile_x << "], tile_y[" << tile_y << "]" << std::endl;
    if (xinc > 0) {
        tile_x++;
    } else if (xinc < 0) {
        tile_x--;
    }

    /// @TODO: refazer links como objetos apenas, talvez esse método inteiro possa ser removido ///

    return false;
}


void GameManager::show_at_texture_renderer()
{
    ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    mapController.show();
    mapController.show_objects();
    mapController.show_enemies();
    mapController.show_npcs();
    player1.show();
    mapController.show_above_objects();
    mapController.showAbove();
    show_hud(false);
    ImageView::get_instance()->change_render_target(RENDER_TARGET_DIRECT_SCREEN);

}

void GameManager::show_hud(bool update_room)
{

    int area_room_x = (SharedData::get_instance()->area_scroll_x+player1.get_real_position().x+TILESIZE)/RES_W;
    int area_room_y = (SharedData::get_instance()->area_scroll_y+player1.get_real_position().y)/AREA_H;

    if (area_room_x < 0 || area_room_y < 0) {
        return;
    }

    /// @TODO: show mini-map ///

    Draw::get_instance()->show_hud(player1.get_current_hp(), 1, 0, 0, area_room_x, area_room_y);


}

void GameManager::consume_dialogs_from_queue()
{
    dialog_queue.erase(dialog_queue.begin(), dialog_queue.begin()+1);
    dialog_status.reset();
}

void GameManager::add_queue_dialog(st_dialog dialog)
{
    if (dialog.timer > 0) {
        dialog.timer = TimerView::get_instance()->getTimer() + dialog.timer;
    }
    dialog_queue.push_back(dialog);
    InputController::get_instance()->clean();
}

void GameManager::wait_until_dialog_is_consumed()
{
    while (dialog_queue.size() > 0) {
        show_game(false, false);
    }
}

st_position GameManager::get_player_relative_center_position()
{
    st_position real_pos = player1.get_real_position();
    st_size player_size = player1.get_size();
    return st_position(real_pos.x+player_size.width/2, real_pos.y+player_size.height/2);
}

void GameManager::initGame()
{
    player1.initialize();
    player1.initFrames();
    player1.set_is_player(true);
    player1.reset_hp();
    config_manager.set_player_ref(&player1);

    fps_manager.initialize();
    mapController.loadMap();

    // IURI: IMPROVE LATER
    std::string tiled_map_filename = SharedData::get_instance()->FILEPATH + "/data/tiled/swamp.tmx";
    tiled_map.initialize(tiled_map_filename, gRenderer);

    Box2dDebugDraw::setRenderer(gRenderer);
    debugDrawer = b2DefaultDebugDraw();
    debugDrawer.drawShapes = true;
    debugDrawer.DrawSolidPolygonFcn = Box2dDebugDraw::DrawSolidPolygon;

    box2d_manager.add_static_body_rectangles(tiled_map.get_tiles_collision(0));
    ImageView::get_instance()->load_layers_data();

    init_map_and_player_to_bottom();
    InGamePresentation::get_instance()->start_show_ready();
}

void GameManager::start_stage_music()
{
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_music(GameData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).music_filename);
    SoundView::get_instance()->play_music();
}


// Builds list of enemies, projectiles and objects that are near screen
// Other loops will use this instead of the whole list
void GameManager::build_screen_area_lists()
{
    mapController.build_screen_area_object_list();
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void GameManager::show_game(bool can_characters_move, bool can_scroll_stage)
{
    if (SharedData::get_instance()->window_size_changed == true) {
        ImageView::get_instance()->change_render_size();
        SharedData::get_instance()->window_size_changed = false;
    }
    ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);

    SharedData::get_instance()->lightpoint_list.clear();
    if (SharedData::get_instance()->leave_game == true) {
        exit_game();
    }
    if (player1.is_teleporting() == false && InGamePresentation::get_instance()->is_showing_ready() == false) { // ignore input while player is teleporting because it caused some issues
        InputController::get_instance()->read_input();
    } else {
        InputController::get_instance()->clean();
    }

    if (dialog_queue.size() > 0) {
        if (dialog_queue.at(0).timer == 0 && InputController::get_instance()->p1_input[BTN_JUMP] == 1) {
            //std::cout << "Remove dialog #1" << std::endl;
           consume_dialogs_from_queue();
           InputController::get_instance()->clean();
        } else if (dialog_queue.at(0).timer > 0 && TimerView::get_instance()->getTimer() > dialog_queue.at(0).timer) {
            //std::cout << "Remove dialog #2, timer[" << (int)dialog_queue.at(0).timer << "]" << std::endl;
            consume_dialogs_from_queue();
            InputController::get_instance()->clean();
        }
    }

    if (InputController::get_instance()->p1_input[BTN_FULL_SCREEN] == 1) {
        SharedData::get_instance()->fullscreen_mode = !SharedData::get_instance()->fullscreen_mode;
        ImageView::get_instance()->set_fullscreen(SharedData::get_instance()->fullscreen_mode);
        InputController::get_instance()->clean();
        InputController::get_instance()->p1_input[BTN_FULL_SCREEN] = 0;
    }

    // TODO::IURI - move to its own class //
    if (PauseMenu::get_instance()->execute_pause_menu() == false) { // game is not paused
        _is_paused = false;
        build_screen_area_lists();

        // must jump a frame
        if (fps_manager.get_frame_drop_n() > 0 && fps_manager.get_current_frame_n() > 0) {
            int modulus = fps_manager.get_frame_drop_n() % fps_manager.get_current_frame_n();
            //std::cout << "MUST JUMP-FRAMES AT [" << fps_manager.get_frame_drop_n() << "], current-frame[" << fps_manager.get_current_frame_n() << "], modulus[" << modulus << "]" << std::endl;
            if (modulus == 0 && show_fps_enabled == true) {
                //std::cout << "JUMP FRAME[" << fps_manager.get_current_frame_n() << "]" << std::endl;
                fps_manager.fps_count();
                return;
            }
        }

        if (InputController::get_instance()->p1_input[BTN_QUIT] == 1) {
            exit_game();
        }


        if (TimerView::get_instance()->is_paused() == false) {
            box2d_manager.execute();

            if (can_scroll_stage == true) {
                update_stage_scrolling();
            }
            mapController.move_objects(TimerView::get_instance()->is_paused());
        }

        // TODO - move this to map-controller
        st_color bg_color = tiled_map.get_map_background_color();
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, bg_color.r, bg_color.g, bg_color.b);
        if (_dark_mode == false) {
            mapController.show();
        }

        tiled_map.draw(gRenderer, mapController.getMapScrolling());

        if (dialog_queue.size() == 0 && can_characters_move == true && SharedData::get_instance()->must_interrupt_character_execution == false) {
            player1.execute();
            // TODO - move to another part of the code
            //std::cout << "GAMEMANAGER::show_game.player1.getMoveCommands().right[" << player1.getMoveCommands().right << "]" << std::endl;
            if (player1.getMoveCommands().right == 1) {
                box2d_manager.change_player_position(st_float_position(2.0f, 0.0f));
            } else if (player1.getMoveCommands().left == 1) {
                box2d_manager.change_player_position(st_float_position(-2.0f, 0.0f));
            }
            if (player1.getMoveCommands().jump == 1) {
                box2d_manager.player_jump();
            }
            st_position p1_real_pos = get_player_relative_center_position();
            SharedData::get_instance()->lightpoint_list.push_back(st_light_point(p1_real_pos.x, p1_real_pos.y, LIGHT_POINT_COLOR_WHITE));
            mapController.move_enemies();
            mapController.move_npcs();
        }

        /// @TODO - move this to the player, so we don't need to check every single loop
        if (player1.is_dead() == true) {
            //std::cout << "### DEAD - RESTART_STAGE ###" << std::endl;
            restart_stage();
            return;
        }

        if (_dark_mode == false) {
            mapController.show_objects();
            mapController.show_enemies();
            mapController.show_npcs();
            player1.show();
            mapController.show_above_objects();
            mapController.showAbove();
        } else {
            ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
        }


        //std::cout << "GFX_MODE[" << (int)mapController.get_current_map_gfx_mode() << "]" << std::endl;

        // TODO::IURI //
        /*
        if (mapController.get_current_map_gfx_mode() == SCREEN_GFX_MODE_OVERLAY) {
            draw::get_instance()->show_gfx();
        }
        */

        // draw HUD



        player1.moved_dist = st_float_position(0, 0);

        ImageView::get_instance()->change_render_target(RENDER_TARGET_HUD_TEXTURE);
        show_hud(true);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);


        if (show_fps_enabled == true) {
            fps_manager.fps_count();
        }
        fps_manager.limit();

        st_rectangle box2d_player_pos = box2d_manager.get_player_box();
        box2d_manager.run_debug_draw(&debugDrawer);


        st_float_position mapScroll = mapController.getMapScrolling();
        ImageView::get_instance()->clearScreenArea(box2d_player_pos.x - mapScroll.x, box2d_player_pos.y - mapScroll.y, box2d_player_pos.w, box2d_player_pos.h, 200, 0, 0);
        TimerView::get_instance()->udelay(2000);


        //std::cout << "$$$ clear_point_x[" << SharedData::get_instance()->clear_point_x << "], clear_point_y[" << SharedData::get_instance()->clear_point_y << "]" << std::endl;
        //ImageView::get_instance()->clearScreenArea(SharedData::get_instance()->clear_point_x, SharedData::get_instance()->clear_point_y, 1, 1, 255, 0, 0);


    } else {
        _is_paused = true;
        fps_manager.limit();
    }


    Draw::get_instance()->update_screen();

    mapController.reset_map_loaded_flag();
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
Uint8 GameManager::getMapPointLock(struct st_position pos)
{
    return mapController.getMapPointLock(pos);
}

st_size GameManager::get_map_size()
{
    return mapController.get_size();
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
st_float_position GameManager::checkScrolling()
{
    st_float_position move;
    st_float_position mapScroll = mapController.getMapScrolling();
    st_float_position p1Pos(player1.getPosition().x,  player1.getPosition().y);

    move.x += (p1Pos.x - mapScroll.x) - RES_W/2;

    //std::cout << "GameManager::checkScrolling - move.x[" << move.x << "]" << std::endl;

    if (mapScroll.x + move.x < 0 || mapScroll.x + move.x > mapController.get_size().width*TILESIZE) {
        move.x = 0;
	}

	return move;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void GameManager::start_stage()
{
	_show_boss_hp = false;
    InputController::get_instance()->clean();


    mapController.reset_map();

    /// @TODO - this must be on a single method in soundlib

    init_map_and_player_to_bottom();

	SoundView::get_instance()->stop_music();

    SoundView::get_instance()->load_stage_music(GameData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).music_filename);

    mapController.loadMap();
    mapController.set_scroll_to_bottom();

    player1.cancel_slide();
    player1.reset_jump();

    player1.clean_projectiles();
    player1.set_direction(ANIM_DIRECTION_RIGHT);
    player1.reset_hp();

    mapController.show();
    mapController.showAbove();
    Draw::get_instance()->fade_screen(0, 0, 0, 1000, true);

    game_unpause();

    SoundView::get_instance()->play_music();

    for (int i=0; i<AUTOSCROLL_START_DELAY_FRAMES; i++) { // extra delay to show dialogs
        InputController::get_instance()->read_input();
        InputController::get_instance()->clean_confirm_button();
        mapController.show();
        mapController.showAbove();
        Draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }
    // TODO::IURI //
    //mapController.add_autoscroll_delay();

    show_player_teleport(PLAYER_INITIAL_X_POS, -1);
    show_game(false, false);
    // reset timers for objects
    mapController.reset_objects_timers();


}

void GameManager::set_player_position_teleport_in(int initial_pos_x, int initial_pos_y)
{
    int first_unlocked_from_bottom = mapController.get_first_lock_on_bottom(initial_pos_x, initial_pos_y, player1.get_size().width, player1.get_size().height);

    //std::cout << ">>>>>>>>>> GAME::set_player_position_teleport_in::first_unlocked_from_bottom[" << first_unlocked_from_bottom << "]" << std::endl;

    player1.set_position(st_position(initial_pos_x, (first_unlocked_from_bottom+1)*TILESIZE-player1.get_size().height));
    player1.char_update_real_position();
    player1.set_animation_type(ANIM_TYPE_TELEPORT);
    player1.set_animation_frame(0);

    //std::cout << ">>>>>>>>>> GAME::set_player_position_teleport_in::DONE" << std::endl;
}

void GameManager::show_player_teleport(int pos_x, int pos_y)
{
    //std::cout << "GAME::show_player_telport #2" << std::endl;

    // find ground for player
    set_player_position_teleport_in(pos_x, pos_y);
    unsigned long end_time = TimerView::get_instance()->getTimer() + 1500;

    //std::cout << "GAME::show_player_telport #2" << std::endl;

    while (TimerView::get_instance()->getTimer() < end_time) {
        mapController.show();
        mapController.showAbove();
        if (player1.animation_has_restarted()) {
            player1.set_animation_frame(1);
            player1.set_animation_has_restarted(false);
        }
        player1.show();
        Draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }

    //std::cout << "GAME::show_player_telport #3" << std::endl;
    player1.set_animation_frame(2);
    player1.show();
    Draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(20);
    //std::cout << "GAME::show_player_telport #4" << std::endl;

    //std::cout << "GAME::show_player_telport #5" << std::endl;
    // force stand to avoid gravity not doing it for any reason
    player1.set_animation_type(ANIM_TYPE_STAND);
    mapController.show();
    mapController.showAbove();
    player1.show();
    Draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(20);

}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void GameManager::restart_stage()
{

    InputController::get_instance()->clean_all();

    //std::cout << "### RESTART_STAGE::START ###" << std::endl;

    if (SharedData::get_instance()->checkpoint.x < TILESIZE*4) {
        SharedData::get_instance()->checkpoint.x = TILESIZE*4;
    }
    // remove any used teleporter
    Draw::get_instance()->fade_screen(0, 0, 0, 500, true);

    player1.set_teleporter(-1);
    _player_teleporter.active = false;

    remove_all_projectiles();
    remove_players_slide();

	_show_boss_hp = false;
    InputController::get_instance()->clean();
    mapController.loadMap();
    mapController.set_scroll_to_bottom();
	// TODO - this must be on a single method in soundlib

    player1.clean_projectiles();
    player1.set_animation_type(ANIM_TYPE_TELEPORT);

    player1.reset_hp();
    player1.reset_jump();
    player1.cancel_slide();

    game_unpause();

    mapController.show();
    mapController.showAbove();
    Draw::get_instance()->update_screen();
    // if was on stage-boss, mneeds to reload music
    if (SoundView::get_instance()->get_is_playing_boss_music() == true) {
        SoundView::get_instance()->load_stage_music(GameData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).music_filename);
    }
    SoundView::get_instance()->restart_music();
    if (SharedData::get_instance()->checkpoint.y == -1) { // did not reached any checkpoint, use the calculated value from stage start
        // find teleport stop point
        show_player_teleport(PLAYER_INITIAL_X_POS, -1);
    } else {
        show_player_teleport(SharedData::get_instance()->checkpoint.x, SharedData::get_instance()->checkpoint.y);
    }

    while (player1.get_anim_type() == ANIM_TYPE_TELEPORT) {
        InputController::get_instance()->clean_all();
        show_game(true, false);
        Draw::get_instance()->update_screen();
    }
    for (int i=0; i<AUTOSCROLL_START_DELAY_FRAMES; i++) { // extra delay to teleport without moving screen
        InputController::get_instance()->clean_all();
        show_game(false, false);
        Draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }

}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool GameManager::show_game_intro()
{


    show_notice();

#ifdef BETA_VERSION
    show_beta_version_warning();
#endif

    SharedData::get_instance()->v6_selected_stage = 0;

    scenes.main_screen();
	initGame();

    SoundView::get_instance()->stop_music();
    start_stage();

    return true;
}

void GameManager::show_beta_version_warning()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    Draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    TextView::get_instance()->renderText(0, 30, st_color(255, 130, 0), true, "-- BETA VERSION WARNING --");
    TextView::get_instance()->renderCenteredText(60, "THIS IS A TEST VERSION OF ROCKDROID,");
    TextView::get_instance()->renderCenteredText(75, "IT DOES CONTAIN ERRORS AND IS NOT");
    TextView::get_instance()->renderCenteredText(90, "COMPLETE MISSING SOME FEATURES.");

    TextView::get_instance()->renderCenteredText(120, "SOFTWARE IS PROVIDED \"AS IS\"");
    TextView::get_instance()->renderCenteredText(135, "WITHOUT WARRANTY OF ANY KIND,");
    TextView::get_instance()->renderCenteredText(150, "EXPRESS OR IMPLIED FROM AUTHOR.");

    TextView::get_instance()->renderCenteredText(170, "REPORT ANY FOUND ISSUES TO");
    TextView::get_instance()->renderCenteredText(185, "bugs@upperland.net");
    TextView::get_instance()->renderCenteredText(210, "PRESS A BUTTON OR KEY TO CONTINUE.");
    Draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
}

void GameManager::show_free_version_warning()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    Draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    TextView::get_instance()->renderText(0, 10, st_color(255, 130, 0), true, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning_title, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(30, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning1, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(45, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning2, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(60, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning3, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(75, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning4, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(90, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning5, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(105, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning6, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(130, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning7, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(145, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning8, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(160, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning9, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(175, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning10, SharedData::get_instance()->current_language));

    TextView::get_instance()->renderCenteredText(205, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning11, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(220, strings_map::get_instance()->get_ingame_string(string_press_key_or_button, SharedData::get_instance()->current_language));
    Draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
}

void GameManager::show_notice()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    Draw::get_instance()->update_screen();

    st_imageData upperland_surface;
    upperland_surface = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->GAMEPATH + "/shared/images/upperland.png");

    st_position logo_pos(RES_W/2 - (upperland_surface.surface->w/6)/2, RES_H/2 - upperland_surface.surface->h/2);


    TextView::get_instance()->renderText(0, logo_pos.y + upperland_surface.surface->h + 7, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_upperland_studios, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderText(0, logo_pos.y + upperland_surface.surface->h + 19, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_presents, SharedData::get_instance()->current_language));


    //std::cout << ">> logo_pos.x: " << logo_pos.x << ", logo_pos.y: " << logo_pos.y << std::endl;
    ImageView::get_instance()->renderTexturePortionAt(0, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);
    TextView::get_instance()->renderCenteredText(220, "HTTP://ROCKBOT.UPPERLAND.NET");
    Draw::get_instance()->update_screen();
    InputController::get_instance()->clean_and_wait_scape_time(400);
    for (int i=1; i<6; i++) {
        ImageView::get_instance()->renderTexturePortionAt((upperland_surface.surface->w/6)*i, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);
        Draw::get_instance()->update_screen();
        InputController::get_instance()->wait_scape_time(30);
    }
    ImageView::get_instance()->renderTexturePortionAt(0, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);


    Draw::get_instance()->update_screen();

    InputController::get_instance()->clean_and_wait_scape_time(1200);


    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    TextView::get_instance()->renderText(0, 10, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_engine1, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(30, strings_map::get_instance()->get_ingame_string(string_intro_engine2, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(50, strings_map::get_instance()->get_ingame_string(string_intro_engine3, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(70, strings_map::get_instance()->get_ingame_string(string_intro_engine4, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(90, strings_map::get_instance()->get_ingame_string(string_intro_engine5, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(110, strings_map::get_instance()->get_ingame_string(string_intro_engine6, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(130, strings_map::get_instance()->get_ingame_string(string_intro_engine7, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(150, strings_map::get_instance()->get_ingame_string(string_intro_engine8, SharedData::get_instance()->current_language));

    Draw::get_instance()->update_screen();

    TimerView::get_instance()->delay(10000);

    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    TextView::get_instance()->renderText(0, 10, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning_title, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(30, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning1, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(50, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning2, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(70, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning3, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(90, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning4, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(110, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning5, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(130, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning6, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(150, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning7, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(170, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning8, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(200, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning9, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(220, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning10, SharedData::get_instance()->current_language));

    Draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(10000);
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
}








void GameManager::set_current_map(unsigned int temp_map_n)
{
    SharedData::get_instance()->v6_selected_stage = temp_map_n;
    mapController.loadMap();
}

unsigned int GameManager::get_current_map()
{
    return SharedData::get_instance()->v6_selected_stage;
}

int GameManager::get_current_area()
{
    return SharedData::get_instance()->v6_selected_stage;
}


void GameManager::map_present_boss(bool show_dialog, bool is_static_boss)
{
    SharedData::get_instance()->is_showing_boss_intro = true;

    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->unload_music();

    // 1. keep showing game screen until player reaches ground
    player1.clear_move_commands();
	bool loop_run = true;
	while (loop_run == true) {
        mapController.show();
        player1.charMove();
        int anim_type = player1.get_anim_type();
        if (player1.hit_ground() == true && anim_type == ANIM_TYPE_STAND) {
			loop_run = false;
		}
        player1.show();
        mapController.showAbove();
        TimerView::get_instance()->delay(8);
        Draw::get_instance()->update_screen();
	}

	// 2. blink screen
	ImageView::get_instance()->blink_screen(255, 255, 255);

	// 3. move boss from top to ground
    GameEnemy* boss_ref = mapController.get_near_boss();
    if (boss_ref != nullptr) {
        if (is_static_boss == false) {
            loop_run = true;
            while (loop_run == true) {
                if (mapController.boss_hit_ground(boss_ref) == true) {
                    loop_run = false;
                    show_stage(0, false);
                } else {
                    show_stage(0, true);
                }
            }
        } else {
            // TODO //
        }
    } else {
        return;
    }

    TimerView::get_instance()->delay(5000);

        // 4. show boss intro sprites animation
    loop_run = true;
    while (loop_run == true) {
        std::cout << "### GAME::map_present_boss #4::LOOP" << std::endl;
        if (boss_show_intro_sprites(boss_ref) == true) {
            loop_run = false;
            show_stage(0, false);
        } else {
            show_stage(0, true);
        }
    }

    std::cout << "### GAME::map_present_boss #5" << std::endl;
    TimerView::get_instance()->delay(5000);


    // 5. show boss dialog
    dialogs boss_dialog;
    //boss_dialog.show_boss_dialog(loaded_stage.get_number());

    show_stage(8, false);


    SoundView::get_instance()->play_boss_music();

    TimerView::get_instance()->delay(100);

	_show_boss_hp = true;
    SharedData::get_instance()->is_showing_boss_intro = false;

}

GameObject* GameManager::get_player_platform()
{
    return player1.get_platform();
}

void GameManager::check_player_return_teleport()
{
    remove_all_projectiles();
    remove_players_slide();
	if (is_player_on_teleporter() == true) {
        finish_player_teleporter();
    }
}

bool GameManager::must_show_boss_hp()
{
    return (_show_boss_hp && get_current_map_obj()->is_boss_on_extended_screen());
}



// ********************************************************************************************** //
// remove the projectiles from the list of all players and npcs                                   //
// ********************************************************************************************** //
void GameManager::remove_all_projectiles()
{
    player1.clean_projectiles();
    mapController.clean_map_enemies_projectiles();
    player1.remove_freeze_effect();
}

void GameManager::reset_beam_objects()
{
    mapController.reset_beam_objects();
}

void GameManager::remove_temp_objects()
{
    mapController.remove_temp_objects();
}

void GameManager::remove_players_slide()
{
    player1.cancel_slide();
}

// TODO::IURI - refazer, pois não deve desenhar na tela, e sim ter 2 mapas ao mesmo tempo //
//TRANSITION_TOP_TO_BOTTOM, TRANSITION_BOTTOM_TO_TOP
void GameManager::transition_screen(Uint8 type, Uint8 map_n, short int adjust_x, classPlayer *pObj) {


    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%% transition_screen" << std::endl;

    st_imageData temp_screen;
    short i = 0;
    temp_screen = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H*2));

    mapController.set_bg_scroll(mapController.get_bg_scroll());

    ImageView::get_instance()->renderTexturePortionAt(0, i*TRANSITION_STEP, RES_W, RES_H, 0, 0, temp_screen.texture);

    // if map destiny and map origin are the same, adjust player's X position
    if (SharedData::get_instance()->v6_selected_stage == map_n) {
        //std::cout << "p.x[" << (int)test_player->getPosition().x << "], p.real.x[" << test_player->get_real_position().x << "]" << std::endl;
        pObj->set_position(st_position(pObj->get_real_position().x+adjust_x, pObj->get_real_position().y));
        //adjust_x += TILESIZE;
    }

    // TODO: adjust player X position when changing from the same map
    // pegar posição relativa do jogador em relação à tela
    // posição nova é o scroll-x novo mais essa diferença

    // draw map in the screen, erasing all players/objects/npcs/GFX
    Draw::get_instance()->set_gfx(SCREEN_GFX_NONE, BG_SCROLL_MODE_NONE);
    mapController.show();


    // draw the offscreen with the new loaded map
	if (type == TRANSITION_TOP_TO_BOTTOM || type == TRANSITION_BOTTOM_TO_TOP) {
        // copy current screen to temp
		if (type == TRANSITION_TOP_TO_BOTTOM) {
            ImageView::get_instance()->copyScreenAreaToImage(0, 0, RES_W, RES_H, 0, 0, temp_screen);
		} else if (type == TRANSITION_BOTTOM_TO_TOP) {
            ImageView::get_instance()->copyScreenAreaToImage(0, 0, RES_W, RES_H, 0, 0, temp_screen);
        }

        mapController.set_scrolling(st_float_position(adjust_x, 0));

		// copy the new screen to the temp_area
        st_imageData temp_map_area;
        mapController.get_map_area_surface(temp_map_area);
		if (type == TRANSITION_TOP_TO_BOTTOM) {
            ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, RES_H, 0, RES_H, temp_map_area.texture);
		} else if (type == TRANSITION_BOTTOM_TO_TOP) {
            ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, RES_H, 0, 0, temp_map_area.texture);
		}
        temp_map_area.freeGraphic();

		// now, show the transition
        short int extra_y = 0;
        for (i=0; i<RES_H/TRANSITION_STEP; i++) {
            if (type == TRANSITION_TOP_TO_BOTTOM) {
                ImageView::get_instance()->renderTexturePortionAt(0, i*TRANSITION_STEP, RES_W, RES_H, 0, 0, temp_screen.texture);
            } else if (type == TRANSITION_BOTTOM_TO_TOP) {
                ImageView::get_instance()->renderTexturePortionAt(0, RES_H-i*TRANSITION_STEP, RES_W, RES_H, 0, 0, temp_screen.texture);
			}

            if (i % 5 == 0) {
				extra_y = 1;
			} else {
                extra_y = 0;
			}

			if (type == TRANSITION_TOP_TO_BOTTOM) {
                //std::cout << "TRANSITION_TOP_TO_BOTTOM, px[" << (int)pObj->getPosition().x << "], py[" << (int)pObj->getPosition().y << "]" << std::endl;
				if (pObj->getPosition().y > 6) {
					pObj->set_position(st_position(pObj->getPosition().x, pObj->getPosition().y - TRANSITION_STEP + extra_y));
				}
			} else if (type == TRANSITION_BOTTOM_TO_TOP) {
                //std::cout << "TRANSITION_BOTTOM_TO_TOP, px[" << (int)pObj->getPosition().x << "], py[" << (int)pObj->getPosition().y << "]" << std::endl;
				if (pObj->getPosition().y < RES_H-TILESIZE*2) {
					pObj->set_position(st_position(pObj->getPosition().x, pObj->getPosition().y + TRANSITION_STEP - extra_y));
				}
			}


            int temp_map_3rdlevel_pos = (RES_H+TILESIZE*0.5) - i*TRANSITION_STEP - 8;
            if (type == TRANSITION_TOP_TO_BOTTOM) {
                mapController.show_objects(-i*TRANSITION_STEP);
                mapController.show_objects(temp_map_3rdlevel_pos, adjust_x);
            } else {
                temp_map_3rdlevel_pos = -(RES_H+TILESIZE*0.5) + i*TRANSITION_STEP + 8; // 8 is a adjust for some error I don't know the reason
                mapController.show_objects(i*TRANSITION_STEP);
                mapController.show_objects(temp_map_3rdlevel_pos, adjust_x);
            }

			pObj->char_update_real_position();
			pObj->show();


			if (type == TRANSITION_TOP_TO_BOTTOM) {
                mapController.showAbove(-i*TRANSITION_STEP, false);
                mapController.show_above_objects(-i*TRANSITION_STEP);
                mapController.show_above_objects(temp_map_3rdlevel_pos, adjust_x);
                mapController.showAbove(temp_map_3rdlevel_pos, adjust_x);
			} else {
                mapController.showAbove(i*TRANSITION_STEP, false);
                mapController.show_above_objects(i*TRANSITION_STEP);
                mapController.show_above_objects(temp_map_3rdlevel_pos, adjust_x);
                mapController.showAbove(temp_map_3rdlevel_pos, adjust_x);
			}

            // draw HUD
            show_hud(false);


            Draw::get_instance()->update_screen();
#if !defined(PLAYSTATION2) && !defined(ANDROID)
            TimerView::get_instance()->delay(6);
            //TimerView::get_instance()->delay(100); // DEBUG //
#endif
		}

        if (type == TRANSITION_TOP_TO_BOTTOM) {
            mapController.changeScrolling(st_float_position(mapController.getMapScrolling().x, 0), false);
        }
        if (type == TRANSITION_TOP_TO_BOTTOM) {
            if (pObj->getPosition().y > TILESIZE) {
                pObj->set_position(st_position(pObj->getPosition().x, pObj->getPosition().y - TRANSITION_STEP - 2));
            }
        } else if (type == TRANSITION_BOTTOM_TO_TOP) {
            if (pObj->getPosition().y < RES_H-TILESIZE*2) {
                pObj->set_position(st_position(pObj->getPosition().x, pObj->getPosition().y + TRANSITION_STEP));
            }
        }
    }

    // when transition finished, spawned npcs and objects such as jet/coild, must be removed
    //temp_map->remove_temp_objects();


    temp_screen.freeGraphic();
	pObj->set_teleporter(-1);
	pObj->char_update_real_position();
}


void GameManager::horizontal_screen_move(short direction, bool is_door, short tileX, short tileY)
{
    std::cout << "GameManager::horizontal_screen_move::START" << std::endl;
    st_float_position scroll_move;

    game_pause();

	if (direction == ANIM_DIRECTION_LEFT) {
        scroll_move.x = -TRANSITION_STEP;
	} else {
        scroll_move.x = TRANSITION_STEP;
	}

	if (is_door == true) {
        remove_all_projectiles();
        // if there is a subboss alive, near left, you can't open
        if (subboss_alive_on_left(tileX) == true) {
            std::cout << "[ERROR]: Oh no! Door can't be open because there is an alive sub-boss on its left side." << std::endl;
            game_unpause();
            return;
        }
        mapController.show();
	}

    int move_limit_top = (RES_W-TILESIZE);
    // check if next tile is also HSCROLL and move one extra tile, if needed
    int check_tile_x = tileX+1;
    if (direction == ANIM_DIRECTION_LEFT) {
        check_tile_x = tileX-1;
    }
    if (mapController.getTileFromPosition(check_tile_x, tileY).locked == TERRAIN_HSCROLL_LOCK) {
        move_limit_top += TILESIZE;
    }


    int move_limit = move_limit_top/abs(TRANSITION_STEP);
    float player_move_x = (float)(TILESIZE*2.5)/(float)move_limit; // player should move two tilesize, to avoid doors
    if (scroll_move.x < 0) {
        player_move_x = player_move_x * -1;
    }

    if (player1.get_anim_type() == ANIM_TYPE_SLIDE) {
        player_move_x += 1;
    }

    std::cout << "GameManager::horizontal_screen_move - move_limit[" << move_limit << "], player_move_x[" << player_move_x << "], scroll_move.x[" << scroll_move.x << "]" << std::endl;

    int static_scroll_x = mapController.getMapScrolling().x;



    ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    for (int i=0; i<move_limit; i++) {
        change_map_scroll(scroll_move, false);
        mapController.show();
        if (mapController.must_show_static_bg() == false) {
            mapController.show_enemies();
            mapController.show_npcs();
        } else {
            mapController.show_enemies_to_left(static_scroll_x+RES_W);
        }
        player1.show();
        mapController.showAbove();
        mapController.show_above_objects();
#if defined(PC)
        TimerView::get_instance()->delay(2);
#endif
        Draw::get_instance()->update_screen();

        player1.inc_position(player_move_x, 0);
        /*
        if (i%(TILESIZE/4) == 0) {
            player1.set_position(st_position(player1.getPosition().x+scroll_move.x, player1.getPosition().y));
            player1.char_update_real_position();
		}
        */

        ImageView::get_instance()->change_render_target(RENDER_TARGET_HUD_TEXTURE);
        show_hud(true);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);

	}
    if (is_door == true) {
        remove_players_slide();
    }
    TimerView::get_instance()->delay(6);
    game_unpause();
    add_autoscroll_delay();
    mapController.show();
}

void GameManager::vertical_screen_move(short direction, bool is_door, short tileX)
{
    std::cout << "gameManager::vertical_screen_move::START" << std::endl;
    st_float_position scroll_move;

    game_pause();

    if (direction == ANIM_DIRECTION_UP) {
        scroll_move.y = -TRANSITION_STEP;
    } else {
        scroll_move.y = TRANSITION_STEP;
    }

    if (is_door == true) {
        remove_all_projectiles();
        // if there is a subboss alive, near left, you can't open
        // TODO: check if boss is alive on top
        if (subboss_alive_on_left(tileX) == true) {
            std::cout << "[ERROR]: Oh no! Door can't be open because there is an alive sub-boss on its left side." << std::endl;
            game_unpause();
            return;
        }
        mapController.show();
    }



    int move_limit = AREA_H/abs(TRANSITION_STEP);

    std::cout << "$$$$$$$$$$$$$ move_limit[" << move_limit << "], scroll_move.y[" << scroll_move.y << "]" << std::endl;

    float player_move_y = (float)(TILESIZE*4)/(float)move_limit; // player should move two tilesize, to avoid doors
    if (scroll_move.y < 0) {
        player_move_y = player_move_y * -1;
    }
    int static_scroll_y = mapController.getMapScrolling().y;



    if (direction != ANIM_DIRECTION_UP) {
        move_limit -= player_move_y/2;
    }
    std::cout << ">>>>>> player_move_y[" << player_move_y << "], move_limit[" << move_limit << "]" << std::endl;


    ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    for (int i=0; i<move_limit; i++) {
        //std::cout << ">>>> gameManager::vertical_screen_move scroll_move.x[" << scroll_move.x << "], scroll_move.y[" << scroll_move.y << "]" << std::endl;

        change_map_scroll(scroll_move, false);
        mapController.show();
        if (mapController.must_show_static_bg() == false) {
            mapController.show_enemies();
            mapController.show_npcs();
        } else {
            mapController.show_enemies_to_left(static_scroll_y+RES_W);
        }
        player1.show();
        mapController.showAbove();
        mapController.show_above_objects();
#if defined(PC)
        TimerView::get_instance()->delay(2);
#endif
        Draw::get_instance()->update_screen();

        player1.inc_position(0, player_move_y);

        ImageView::get_instance()->change_render_target(RENDER_TARGET_HUD_TEXTURE);
        show_hud(true);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    }
    if (is_door == true) {
        remove_players_slide();
    }
    TimerView::get_instance()->delay(6);
    game_unpause();
    add_autoscroll_delay();
    std::cout << "scroll.y[" << mapController.getMapScrolling().y << "]" << std::endl;
    mapController.show();
}


void GameManager::transition_area_horizontal(short direction, short tileX, short tileY)
{
    // TODO: understand and simplify the transition logic
    //std::cout << "GameManager::transition_area_horizontal::START" << std::endl;
    game_pause();
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    int total_move = RES_W;
    int total_steps = total_move/TRANSITION_STEP;
    int screen_move_step = TRANSITION_STEP;

    // player needs to move just a bit to compensate the map scroll and move from its position to the other side of the screen
    player1.char_update_real_position();
    int player_x_dist = RES_W - player1.get_real_position().x;
    float player_move_x = player_x_dist/total_steps;

    if (direction == ANIM_DIRECTION_LEFT) {
        int player_x_dist = TILESIZE*4;
        player_move_x = -(player_x_dist/total_steps);
        screen_move_step = -TRANSITION_STEP;
    }

    for (int i=0; i<total_steps; i++) {
        // TODO - change_map_scroll should respect limits and do not scroll more than it can
        change_map_scroll(st_float_position(screen_move_step, 0), false);
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
        player1.show();
#if defined(PC)
        TimerView::get_instance()->delay(2);
#endif
        Draw::get_instance()->update_screen();
        player1.inc_position(player_move_x, 0);
        player1.char_update_real_position();
        ImageView::get_instance()->change_render_target(RENDER_TARGET_HUD_TEXTURE);
        show_hud(true);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
        mapController.show();
    }
    TimerView::get_instance()->delay(6);
    game_unpause();
    add_autoscroll_delay();
    update_current_area_number();
    mapController.load_new_style();
    mapController.show();
}


void GameManager::transition_area_vertical(short direction, short tileX)
{
    std::cout << "gameManager::vertical_screen_move::START" << std::endl;
    st_float_position scroll_move;

    game_pause();
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    if (direction == ANIM_DIRECTION_UP) {
        scroll_move.y = -TRANSITION_STEP;
    } else {
        scroll_move.y = TRANSITION_STEP;
    }

    int move_limit = AREA_H/abs(TRANSITION_STEP);

    std::cout << "$$$$$$$$$$$$$ move_limit[" << move_limit << "], scroll_move.y[" << scroll_move.y << "]" << std::endl;

    float player_move_y = (float)(TILESIZE*4)/(float)move_limit; // player should move two tilesize, to avoid doors
    if (scroll_move.y < 0) {
        player_move_y = player_move_y * -1;
    }

    if (direction != ANIM_DIRECTION_UP) {
        move_limit -= player_move_y/2;
    }
    std::cout << ">>>>>> player_move_y[" << player_move_y << "], move_limit[" << move_limit << "]" << std::endl;


    ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    for (int i=0; i<move_limit; i++) {
        //std::cout << ">>>> gameManager::vertical_screen_move scroll_move.x[" << scroll_move.x << "], scroll_move.y[" << scroll_move.y << "]" << std::endl;

        change_map_scroll(scroll_move, false);
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
        player1.show();
#if defined(PC)
        TimerView::get_instance()->delay(2);
#endif
        Draw::get_instance()->update_screen();
        player1.inc_position(0, player_move_y);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_HUD_TEXTURE);
        show_hud(true);
        ImageView::get_instance()->change_render_target(RENDER_TARGET_GAME_TEXTURE);
    }
    TimerView::get_instance()->delay(6);
    game_unpause();
    add_autoscroll_delay();
    std::cout << "scroll.y[" << mapController.getMapScrolling().y << "]" << std::endl;
    update_current_area_number();
    mapController.load_new_style();
    mapController.show();
}

void GameManager::update_current_area_number()
{
    // find the current_area from the map center
    int map_tile_x = (mapController.getMapScrolling().x/TILESIZE + AREA_ROOM_TILES_W/2)/AREA_ROOM_TILES_W;
    int map_tile_y = (mapController.getMapScrolling().y/TILESIZE + AREA_ROOM_TILES_H/2)/AREA_ROOM_TILES_H;
    //std::cout << "GameManager::update_current_area_number - x[" << map_tile_x << "], y[" << map_tile_y << "]" << std::endl;
    st_position map_pos = st_position(map_tile_x, map_tile_y);
    SharedData::get_instance()->v6_selected_area = GameData::get_instance()->v6_area_room_list.at(map_pos).area_n;
}

void GameManager::show_door_animation()
{
    remove_players_slide();

    TimerView::get_instance()->delay(6);
    game_unpause();
    mapController.show();
}





void GameManager::leave_stage()
{
    save_game();
    Draw::get_instance()->set_flash_enabled(false);

    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(200);

    // return to stage selection
    player1.reset_charging_shot();
    // @TODO: last stage must be set by game_data.final_boss_id //

    // show boss intro with stars, if needed
    SoundView::get_instance()->stop_music();
    SharedData::get_instance()->checkpoint.map = 0;
    SharedData::get_instance()->checkpoint.map_scroll_x = 0;
    SharedData::get_instance()->checkpoint.reset();
    start_stage();
}

void GameManager::return_to_intro_screen()
{
    save_game();

    Draw::get_instance()->set_flash_enabled(false);

    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(200);

    // return to stage selection
    player1.reset_charging_shot();

    scenes.main_screen();
    SharedData::get_instance()->leave_game = false;

    player1.initialize();
    player1.initFrames();
    player1.set_is_player(true);
    player1.reset_hp();
    InputController::get_instance()->clean();
    start_stage();
}

void GameManager::game_pause()
{
    TimerView::get_instance()->pause();
    // @TODO - save-player-input
    player1.save_input();
    _is_paused = true;
}

void GameManager::game_unpause()
{
    TimerView::get_instance()->unpause();
    InputController::get_instance()->read_input();
    player1.restore_input();
    player1.reset_sprite_animation_timer();
    mapController.reset_map_timers();
    _is_paused = false;
}

bool GameManager::is_paused()
{
    return _is_paused;
}

void GameManager::exit_game()
{

    dialogs dialogs_obj;
    if (dialogs_obj.show_leave_game_dialog() != true) { // cuidar se isso não cria loophole
        SharedData::get_instance()->leave_game = false;
        return;
    }


#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKDROID2###", "### GAME::exit_game ###");
#endif


    save_game();

    SharedData::get_instance()->run_game = false;

}


void GameManager::show_ending()
{
    // save the data indicating game was finished, so user can see ending later or get access to more features
    SharedData::get_instance()->game_config.game_finished = true;
    fio.save_config(SharedData::get_instance()->game_config);

    Draw::get_instance()->show_credits(false);

    return_to_intro_screen();
}

void GameManager::quick_load_game()
{
    if (fio.save_exists(current_save_slot)) {
        std::cout << "GameManager::quick_load_game - load save" << std::endl;
        fio.read_save(SharedData::get_instance()->game_save, current_save_slot);
    } else {
        std::cout << "GameManager::quick_load_game - NO SAVE TO LOAD" << std::endl;
    }

    SharedData::get_instance()->game_save.selected_player = PLAYER_1;

    scenes.preloadScenes();



    // DEBUG //

    initGame();

    // DEBUG //
    //show_ending();

    //game_save.armor_pieces[ARMOR_TYPE_LEGS] = true;

    start_stage();

}

void GameManager::set_player_direction(ANIM_DIRECTION dir)
{
    player1.set_direction(dir);
}

void GameManager::update_stage_scrolling()
{
    if (TimerView::get_instance()->is_paused() == true) {
        return;
    }
    mapController.changeScrolling(checkScrolling(), true);
    st_position p_pos = player1.get_real_position();
    p_pos.x += player1.get_size().width/2;
    //std::cout << "p_pos.x: " << p_pos.x << std::endl;
    //if (p_pos.x < 0.0) {
    if (p_pos.x < -TILESIZE/2) {
        std::cout << "GameManager::update_stage_scrolling.change_position_x" << std::endl;
        player1.change_position_x(1);
        // out of screen, probably because was pushed out on a autoscroll stage
        if (p_pos.x < -(TILESIZE-2)) {
            player1.damage(999, true);
        }
    }
}




void GameManager::draw_explosion(st_position center, bool show_players) {
    Draw::get_instance()->update_screen();
    int angle_inc = 0;


    for (int i=5; i<RES_W; i+=6) {
        mapController.show();
        if (show_players) {
            player1.show();
        }
        mapController.showAbove();
        angle_inc += 5;
        if (angle_inc > 360) {
            angle_inc = 0;
        }
        Draw::get_instance()->draw_explosion(center, i, angle_inc);
        for (int k=50; k<250; k+=80) {
            if (i > k) {
                Draw::get_instance()->draw_explosion(center, i-k, angle_inc);
            }
        }
        Draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(10);
    }

    /*
    //ANIMATION_TYPES pos_type, st_imageData* surface, const st_float_position &pos, st_position adjust_pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize
    st_float_position anim_pos = st_float_position(centerX-23+get_current_map_obj()->get_map_scrolling_ref()->x, centerY-23);
    get_current_map_obj()->add_animation(ANIMATION_STATIC, &ImageView::get_instance()->explosion_player_death, anim_pos, st_position(0, 0), 100, 6, player1.get_direction(), st_size(47, 47));

    while (TimerView::get_instance()->getTimer() < timerInit+2000) {
        mapController.show();
        if (show_players) {
            player1.show();
        }
        mapController.showAbove();
        draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(10);
    }
    TimerView::get_instance()->delay(300);
    */
}

void GameManager::show_player()
{
    player1.show();
}

void GameManager::set_player_position(st_position pos)
{
    std::cout << "#### GameManager::set_player_position" << std::endl;
    player1.set_position(pos);
    player1.char_update_real_position();
}

void GameManager::change_player_position(short xinc, short yinc)
{
    std::cout << "#### GameManager::change_player_position[" << xinc << "][" << yinc << "]" << std::endl;
    player1.change_position(xinc, yinc);
    player1.char_update_real_position();
    box2d_manager.change_player_position(st_float_position(xinc, yinc));
}

void GameManager::set_player_anim_type(ANIM_TYPE anim_type)
{
    player1.set_animation_type(anim_type);
}

void GameManager::show_player_at(int x, int y)
{
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKDROID2###", "### GAME::show_player_at[%d, %d] ###", x, y);
#endif
    //std::cout << "show_player_at[" << x << ", " << y << "]" << std::endl;
        player1.show_at(st_position(x, y));
}

void GameManager::draw_player_death(st_position center)
{
    center.x += 29/2;
    center.y += 29/2;

    for (int i=0; i<Draw::get_instance()->get_death_animation_frames_n(); i++) {
        show_stage(0, false);
        Draw::get_instance()->draw_player_death(center, i);
        Draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(DEATH_ANIMATION_DELAY);
        if (i == 5) {
            show_stage(0, false);
            Draw::get_instance()->draw_player_death(center, i-1);
            Draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(DEATH_ANIMATION_DELAY);
            show_stage(0, false);
            Draw::get_instance()->draw_player_death(center, i);
            Draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(DEATH_ANIMATION_DELAY);
        }
    }
}

st_position GameManager::get_player_position()
{
    return st_position(player1.getPosition().x, player1.getPosition().y);
}

st_position GameManager::get_player_center_position()
{
    return st_position(player1.get_real_position().x+player1.get_size().width/2, player1.get_real_position().y+player1.get_size().height/2);
}

st_size GameManager::get_player_size()
{
    return player1.get_size();
}

void GameManager::set_player_direction(Uint8 direction)
{
    player1.set_direction(direction);
}

void GameManager::walk_character_to_screen_point_x(character *char_obj, short pos_x)
{
	/// @TODO: jump obstacles
	if (char_obj->get_real_position().x+char_obj->get_size().width/2 > pos_x) {
        char_obj->set_animation_type(ANIM_TYPE_WALK);
		char_obj->set_direction(ANIM_DIRECTION_LEFT);
		while (char_obj->get_real_position().x+char_obj->get_size().width/2 > pos_x) {
			char_obj->set_position(st_position(char_obj->getPosition().x-2, char_obj->getPosition().y));
            mapController.show();
            mapController.showAbove();
            mapController.show_enemies();
            mapController.show_npcs();
            player1.show();
            Draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(20);
		}
	} else if (char_obj->get_real_position().x+char_obj->get_size().width/2 < pos_x) {
		char_obj->set_direction(ANIM_DIRECTION_RIGHT);
        char_obj->set_animation_type(ANIM_TYPE_WALK);
		while (char_obj->get_real_position().x+char_obj->get_size().width/2 < pos_x) {
			char_obj->set_position(st_position(char_obj->getPosition().x+2, char_obj->getPosition().y));
            mapController.show();
            mapController.showAbove();
            mapController.show_enemies();
            mapController.show_npcs();
            player1.show();
            Draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(20);
		}
	}

}

void GameManager::set_player_teleporter(short set_teleport_n, st_position set_player_pos, bool is_object)
{
    _player_teleporter.is_object = is_object;
	_player_teleporter.teleporter_n = set_teleport_n;
	_player_teleporter.old_player_pos.x = set_player_pos.x;
	_player_teleporter.old_player_pos.y = set_player_pos.y;

    std::cout << "################### SET PLAYER TELEPORTER ###################" << std::endl;

	_player_teleporter.active = true;
	_player_teleporter.finished = false;
    _player_teleporter.old_map_scroll = mapController.getMapScrolling();
    _player_teleporter.old_map_n = SharedData::get_instance()->v6_selected_stage;
}

bool GameManager::is_player_on_teleporter()
{
    std::cout << "######## is_player_on_teleporter[" << _player_teleporter.active << "] ########" << std::endl;
    return _player_teleporter.active;
}

void GameManager::show_ability_item_dialog(int ability_n)
{
    std::vector<std::string> msgs;
    // @TODO: i18n //
    msgs.push_back("You have acquired an item that gives you");
    msgs.push_back("the following ability:");
    msgs.push_back("Sliding");

    st_dialog dialog;
    dialog.msgs = msgs;
    dialog.timer = 9000;
    dialog.music_filename = "got_ability.mod";

    InputController::get_instance()->clean();
    add_queue_dialog(dialog);

    /*

    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_music("got_ability.mod");
    SoundView::get_instance()->play_music_once();
    TimerView::get_instance()->delay(7000);
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_stage_music(SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).music_filename);
    SoundView::get_instance()->play_music();
    */
}


short GameManager::get_current_save_slot()
{
    return current_save_slot;
}

void GameManager::set_current_save_slot(short n)
{
    current_save_slot = n;
}

void GameManager::save_game()
{
    if (fio.write_save(SharedData::get_instance()->game_save, current_save_slot) == false) {
        show_savegame_error();
        return;
    }
}

void GameManager::read_save()
{
    if (fio.save_exists(current_save_slot)) {
        fio.read_save(SharedData::get_instance()->game_save, current_save_slot);
    }
}

void GameManager::set_show_fps_enabled(bool enabled)
{
    show_fps_enabled = enabled;
}

bool GameManager::get_show_fps_enabled()
{
    return show_fps_enabled;
}

void GameManager::add_autoscroll_delay()
{
    autoscroll_timer = TimerView::get_instance()->getTimer()+800;
}




void GameManager::remove_current_teleporter_from_list()
{
    if (_player_teleporter.teleporter_n != -1) {
        _last_stage_used_teleporters.erase(_player_teleporter.teleporter_n);
    }
    player1.set_teleporter(-1);
}

void GameManager::select_game_screen()
{
    std::vector<std::string> game_list = fio.read_game_list();
    if (game_list.size() < 1) {
        _selected_game = std::string("");
        std::cout << "select_game_screen [NO GAMES]" << std::endl;
        exit(-1);
    } else if (game_list.size() == 1) {
        _selected_game = game_list.at(0);
        std::cout << "select_game_screen [" + _selected_game + "]" << std::endl;
        return;
    }
    ImageView::get_instance()->show_config_bg();
    TextView::get_instance()->renderText(10, 20, "SELECT GAME:");

    option_picker main_picker(false, st_position(30, 40), game_list, false);
    main_picker.enable_check_input_reset_command();
    Draw::get_instance()->update_screen();
    bool repeat_menu = true;
    int picked_n = -1;
    while (repeat_menu == true) {
        picked_n = main_picker.pick();
        if (picked_n >= 0 && picked_n < game_list.size()) {
            repeat_menu = false;
        }
        main_picker.draw();
    }
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(200);

    //std::string game_dir = std::string("/games/") + game_list.at(picked_n) + std::string("/");

    _selected_game = game_list.at(picked_n);
}

std::string GameManager::get_selected_game()
{
    return _selected_game;
}

void GameManager::finish_player_teleporter()
{
    remove_all_projectiles();
    remove_players_slide();
    player1.recharge(ENERGY_TYPE_HP, ENERGY_ITEM_BIG);
    Draw::get_instance()->fade_screen(0, 0, 0, 500, true);
    TimerView::get_instance()->delay(1000);

    std::cout << "################### RESET PLAYER TELEPORTER ###################" << std::endl;
    _player_teleporter.active = false;
    _last_stage_used_teleporters.insert(std::pair<int,bool>(_player_teleporter.teleporter_n, true));
	// teleport out
	SoundView::get_instance()->play_sfx(SFX_TELEPORT);
    Draw::get_instance()->fade_screen(0, 0, 0, 500, true);
    _player_teleporter.old_player_pos.y -= 5;
    player1.set_position(_player_teleporter.old_player_pos);
    SharedData::get_instance()->v6_selected_stage = _player_teleporter.old_map_n;
    if (_last_stage_used_teleporters.size() == 8) {
        // search for the final-boss teleporter capsule and start it
        mapController.activate_final_boss_teleporter();
    }
    mapController.set_scrolling(st_float_position(_player_teleporter.old_map_scroll));
    std::cout << "CHAR::RESET_TO_STAND #Y.5" << std::endl;
    player1.set_animation_type(ANIM_TYPE_STAND);
    if (_player_teleporter.is_object == true) {
        mapController.finish_object_teleporter(_player_teleporter.teleporter_n);
    }
    player1.set_teleporter(-1);
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_stage_music(GameData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).music_filename);
    SoundView::get_instance()->play_music();
}

void GameManager::show_stage(int wait_time, bool move_npcs)
{
    if (_dark_mode == false) {
        mapController.show();
    }
	if (move_npcs == true) {
        mapController.move_enemies();
	}
    if (_dark_mode == false) {
        mapController.show_enemies();
        mapController.show_npcs();
        player1.show();
        mapController.showAbove();
    }
	if (wait_time > 0) {
        TimerView::get_instance()->delay(wait_time);
	}
    Draw::get_instance()->update_screen();
}

bool GameManager::subboss_alive_on_left(short tileX)
{
    return mapController.subboss_alive_on_left(tileX);
}

void GameManager::change_map_scroll(st_float_position pos, bool check_lock)
{
    mapController.changeScrolling(pos, check_lock);
}

void GameManager::init_map_and_player_to_bottom()
{
    /// @TODO: tem que calcular apenas para a área ///
    //int player_initial_x = abs(SharedData::get_instance()->leftmost_room-SharedData::get_instance()->rightmost_room)/2 * GAME_AREA_W * TILESIZE + RES_W/2;
    int player_initial_x = 80; // TEMP //

    mapController.set_scrolling(st_float_position(player_initial_x-RES_W/2, 0));
    mapController.set_scroll_to_bottom();

    int bottom_tile_y = mapController.get_first_lock_on_bottom(player_initial_x, -1, player1.get_size().width, player1.get_hitbox(ANIM_TYPE_STAND).h);



    int bottom_y = bottom_tile_y*TILESIZE-player1.get_size().height+TILESIZE+1;
    //std::cout << "### bottom_y[" << bottom_y << "], bottom_tile.y[" << bottom_tile_y << "], player_h[" << player1.get_hitbox(ANIM_TYPE_STAND).h << "]" << std::endl;


    //std::cout << "@@@@@@@@@@@@@@@@@@@ player_initial_x[" << player_initial_x << "]" << std::endl;
    player1.set_position(st_position(player_initial_x, bottom_y));
    player1.set_animation_type(ANIM_TYPE_STAND);
}

void GameManager::drawSolidPolygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius,
    b2HexColor color, void *context) {
    std::cout << "GameManager::drawSolidPolygon" << std::endl;
}

st_size GameManager::calc_area_tile_size(int area_n)
{
    //file_v6_level_point new_level_data = fio_cmm.load_single_object_from_list<file_v6_level_point>(SharedData::get_instance()->FILEPATH + "/" + FILE_V6_LEVEL_LIST, area_n);

    SharedData::get_instance()->topmost_room = 99999;
    SharedData::get_instance()->bottommost_room = -1;
    SharedData::get_instance()->leftmost_room = 99999;
    SharedData::get_instance()->rightmost_room = -1;
    for (std::map<st_position, file_v6_room>::iterator it = GameData::get_instance()->v6_area_room_list.begin(); it != GameData::get_instance()->v6_area_room_list.end(); ++it) {

        //std::cout << "GameManager::calc_area_tile_size - room.x[" << it->first.x << "], room.y[" << it->first.y << "]" << std::endl;

        if (it->first.x > SharedData::get_instance()->rightmost_room) {
            SharedData::get_instance()->rightmost_room = it->first.x;
        }
        if (it->first.x < SharedData::get_instance()->leftmost_room) {
            SharedData::get_instance()->leftmost_room = it->first.x;
        }
        if (it->first.y > SharedData::get_instance()->bottommost_room) {
            SharedData::get_instance()->bottommost_room = it->first.y;
        }
        if (it->first.y < SharedData::get_instance()->topmost_room) {
            SharedData::get_instance()->topmost_room = it->first.y;
        }
    }
    SharedData::get_instance()->total_editarea_w = (SharedData::get_instance()->rightmost_room - SharedData::get_instance()->leftmost_room) + 1; // plus 1 because we start the count in zero
    SharedData::get_instance()->total_editarea_h = (SharedData::get_instance()->bottommost_room - SharedData::get_instance()->topmost_room) +1;

    //std::cout << "GameManager::calc_area_tile_size - w[" << SharedData::get_instance()->total_editarea_w << "], h[" << SharedData::get_instance()->total_editarea_h << "]";
    //std::cout << ", leftmost[" << SharedData::get_instance()->leftmost_room << "], rightmost[" << SharedData::get_instance()->rightmost_room << "], topmost[" << SharedData::get_instance()->topmost_room << "], bottomost[" << SharedData::get_instance()->bottommost_room << "]" << std::endl;

    return st_size(SharedData::get_instance()->total_editarea_w, SharedData::get_instance()->total_editarea_h);

}

bool GameManager::is_special_boss(std::string name)
{
    if (special_bosses_list.find(name) != special_bosses_list.end()) {
        return true;
    }
    return false;
}

void GameManager::talk_with_npc(int npc_id)
{
    std::string obj_name = "";
    int obj_id = player1.get_current_item_id_from_slot();
    //std::cout << ">>>>>>>>>>> obj_id[" << obj_id << "]" << std::endl;
    if (obj_id != -1) {
        obj_name = GameData::get_instance()->v6_object_list.at(obj_id).name;
    }

    // check if npc wants item or gets item from player

    if (npc_id >= GameData::get_instance()->get_npc_list_size()) {
        return;
    }
    GameNPC* npc = mapController.find_npc_by_id(npc_id);
    file_npc_v3_1_2* main_npc = GameData::get_instance()->get_npc(npc_id);
    short requesting_item_state = npc_dialog_manager.item_request_state(npc_id, obj_id);
    if (requesting_item_state == 0) { // show item request tooltip over npc-head
        int requested_object_id = GameData::get_instance()->get_npc(npc_id)->npc_requested_item_id;
        if (requested_object_id != -1) {
            if (player1.get_current_item_id_from_slot() == requested_object_id) {
                // drop item
                npc_dialog_manager.inc_request_state(npc_id);
                mapController.drop_game_item(main_npc->npc_given_item_id, -1, npc->get_int_position().x, npc->get_int_position().y);
            } else {
                npc->npc_activate_request_item_tooltip();
            }
        } else {
            std::string npc_dialog_str = "DEBUG";
            if (npc->get_dialog(SharedData::get_instance()->current_language).length() > 0) {
                npc_dialog_str = npc->get_dialog(SharedData::get_instance()->current_language);
            }
            std::cout << "TALK_WITH_NPC, dialog_id[" << main_npc->npc_dialog_id << "]" << std::endl;
            zoom_in();
            st_dialog dialog;
            dialog.msgs.push_back(npc_dialog_str);
            dialog.msgs.push_back(npc_dialog_str);
            dialog.msgs.push_back(npc_dialog_str);
            add_queue_dialog(dialog);
            wait_until_dialog_is_consumed();
            zoom_out();
        }
    }


}

void GameManager::morph_player_object(int new_obj_id)
{
    player1.morph_item(new_obj_id);
}

void GameManager::remove_player_object()
{
    player1.remove_game_item_from_slot();
}

bool GameManager::boss_show_intro_sprites(GameEnemy *npc_ref)
{
    if (npc_ref->have_frame_graphic(npc_ref->get_direction(), ANIM_TYPE_INTRO, 0) == false) {
        return true;
    }
    if (npc_ref->get_anim_type() != ANIM_TYPE_INTRO) {
        npc_ref->set_animation_type(ANIM_TYPE_INTRO);
        return false;
    }
    if (!npc_ref->is_on_last_animation_frame()) {
        return false;
    }
    if (npc_ref->get_can_fly() == true) {
        npc_ref->set_animation_type(ANIM_TYPE_WALK_AIR);
    } else {
        npc_ref->set_animation_type(ANIM_TYPE_STAND);
    }
    return true;
}

void GameManager::zoom_in()
{
    for (float i=1.0; i<ZOOM_MAX; i+=ZOOM_STEP) {
        ImageView::get_instance()->inc_scale(0.01);
        show_game(false, false);
        if (ImageView::get_instance()->get_scale() == ZOOM_MAX) { // inc-scale has some issue, this prevent is passing the desired value
            break;
        }
    }
}

void GameManager::zoom_out()
{
    for (float i=ZOOM_MAX; i>=1.0; i-=ZOOM_STEP) {
        ImageView::get_instance()->inc_scale(-0.01);
        show_game(false, false);
        if (ImageView::get_instance()->get_scale() == 1.0) { // inc-scale has some issue, this prevent is passing the desired value
            break;
        }
    }
}

st_dialog_status *GameManager::get_dialog_status()
{
    return &dialog_status;
}

std::vector<st_dialog> *GameManager::get_dialog_queue()
{
    return &dialog_queue;
}

MapController *GameManager::get_current_map_obj()
{
    return &mapController;
}

void GameManager::object_teleport_boss(st_position dest_pos, Uint8 dest_map, Uint8 teleporter_id, bool must_return)
{
    // checa se já foi usado
    if (_last_stage_used_teleporters.find(teleporter_id) != _last_stage_used_teleporters.end()) {
        return;
    }
    std::cout << "############################################ TELEPORT #2" << std::endl;
    if (must_return) {
        set_player_teleporter(teleporter_id, st_position(player1.getPosition().x, player1.getPosition().y), true);
    }
    Draw::get_instance()->fade_screen(0, 0, 0, 500, false);
    Draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(500);

    set_current_map(dest_map);

    int new_scroll_pos = mapController.get_first_lock_on_left(dest_pos.x);
    mapController.set_scrolling(st_float_position(new_scroll_pos, 0));
    classPlayer* test_player = &player1;
    int pos_y = mapController.get_first_lock_on_bottom(dest_pos.x*TILESIZE, -1, test_player->get_size().width, test_player->get_size().height);
    if (pos_y < 0 || pos_y > RES_H/TILESIZE) {
        pos_y = 0;
    }
    // adjust to avoid getting stuck into ground
    pos_y--;
    test_player->set_position(st_position(dest_pos.x*TILESIZE, pos_y*TILESIZE));
    test_player->char_update_real_position();

    mapController.reset_scrolled();

    Draw::get_instance()->update_screen();
}



bool GameManager::show_config(short finished_stage)
{
    game_menu menu;
    if (menu.show_main_config(finished_stage, true) == 1) {
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(50);
        config_manager.disable_ingame_menu();
        leave_stage();
        return true;
    }
    return false;
}

void GameManager::show_savegame_error()
{
    std::vector<std::string> msgs;
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror1, SharedData::get_instance()->current_language));
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror2, SharedData::get_instance()->current_language));
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror3, SharedData::get_instance()->current_language));
    st_dialog dialog;
    dialog.msgs = msgs;
    Draw::get_instance()->show_ingame_warning(dialog);

    InputController::get_instance()->clean();
    //InputController::get_instance()->wait_keypress();

}

void GameManager::get_drop_item_ids()
{
    for (int i=0; i<DROP_ITEM_COUNT; i++) {
        _drop_item_list[i] = -1;
    }
    for (int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        if (GameData::get_instance()->v6_object_list.at(i).type == OBJ_ENERGY_PILL_SMALL) {
            _drop_item_list[DROP_ITEM_ENERGY_SMALL] = i;
        } else if (GameData::get_instance()->v6_object_list.at(i).type == OBJ_ENERGY_PILL_BIG) {
            _drop_item_list[DROP_ITEM_ENERGY_BIG] = i;
        } else if (GameData::get_instance()->v6_object_list.at(i).type == OBJ_WEAPON_PILL_SMALL) {
            _drop_item_list[DROP_ITEM_WEAPON_SMALL] = i;
        } else if (GameData::get_instance()->v6_object_list.at(i).type == OBJ_WEAPON_PILL_BIG) {
            _drop_item_list[DROP_ITEM_WEAPON_BIG] = i;
        }
    }
}

character *GameManager::get_player()
{
    return &player1;
}

st_float_position GameManager::get_current_stage_scroll()
{
    return mapController.getMapScrolling();
}

void GameManager::reset_scroll()
{
    mapController.reset_scrolling();
}

short GameManager::get_drop_item_id(short type)
{
    return _drop_item_list[type];
}

void GameManager::show_map()
{
    mapController.show();
    mapController.showAbove();
    Draw::get_instance()->update_screen();
}


