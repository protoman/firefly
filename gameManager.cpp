#include <cstring>
#include <cstdlib>
#include "gameManager.h"
#include <fstream>

#ifdef PSP
	#include <pspkernel.h>
	#include <pspdebug.h>
	#include <pspctrl.h>
	#include <pspdebug.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

#include "data/shareddata.h"

#include "view/option_picker.h"
#include "view/textview.h"
#include "file/format.h"
#include "defines.h"
#include "file/file_io.h"
#include "file/fio_strings.h"
#include "strings_map.h"

#include "controller/inputcontroller.h"

#include "aux_tools/fps_control.h"

gameManager* gameManager::_instance = nullptr;

// ********************************************************************************************** //
// class constructor                                                                              //
// ********************************************************************************************** //
gameManager::gameManager() : _show_boss_hp(false), player1(0)
{
	_frame_duration = 1000/80; // each frame must use this share of time
    invencible_old_value = false;
    _dark_mode = false;
    is_showing_boss_intro = false;
    current_save_slot = 0;
    show_fps_enabled = false;

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
gameManager::~gameManager()
{
}


// ********************************************************************************************** //
// initializar game, can't be on constructor because it needs other objects (circular)            //
// ********************************************************************************************** //
gameManager *gameManager::get_instance()
{
    if (!_instance) {
        _instance = new gameManager();
    }
    return _instance;
}

void gameManager::initHardwareLayer()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }
    SharedData::get_instance()->window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RES_W, RES_H, SDL_WINDOW_SHOWN );
    if (SharedData::get_instance()->window == nullptr) {
        std::cout << "Window could not be created! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    gRenderer = SDL_CreateRenderer(SharedData::get_instance()->window, -1, SDL_RENDERER_ACCELERATED );
    if (gRenderer == nullptr) {
        std::cout << "Renderer could not be created! SDL Error" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }


    ImageView::get_instance()->init();
    TextView::get_instance()->init();
    SoundView::get_instance()->init();

    //Get window surface
    SharedData::get_instance()->screenSurface = SDL_GetWindowSurface(SharedData::get_instance()->window);
    SDL_UpdateWindowSurface(SharedData::get_instance()->window);

}

void gameManager::preloadGameData()
{
    loadGameData();
    loadMapData();
    draw::get_instance()->preload();
}

void gameManager::loadGameData()
{
    fio.read_game(SharedData::get_instance()->game_data);
    SharedData::get_instance()->enemy_list = fio_cmm.load_from_disk<file_npc_v3_1_2>("game_enemy_list_3_1_2.dat");
    if (SharedData::get_instance()->enemy_list.size() == 0) {
        SharedData::get_instance()->enemy_list.push_back(file_npc_v3_1_2());
    }

    SharedData::get_instance()->object_list = fio_cmm.load_from_disk<file_object>("game_object_list.dat");
    if (SharedData::get_instance()->object_list.size() == 0) { // add one first item to avoid errors
        SharedData::get_instance()->object_list.push_back(file_object());
    }

    SharedData::get_instance()->ai_list = fio_cmm.load_from_disk<file_artificial_inteligence>("game_ai_list.dat");
    //std::cout << "MEDIATOR::load_game::ai_list.size(): " << ai_list.size() << std::endl;
    if (SharedData::get_instance()->ai_list.size() == 0) { // add one first item to avoid errors
        for (int i=0; i<SharedData::get_instance()->enemy_list.size(); i++) {
            SharedData::get_instance()->ai_list.push_back(file_artificial_inteligence());
        }
    }


    SharedData::get_instance()->projectile_list_v3 = fio_cmm.load_from_disk<file_projectilev3>(SharedData::get_instance()->FILEPATH+PROJECTILE_FILE_V3);
    if (SharedData::get_instance()->projectile_list_v3.size() == 0) {
        SharedData::get_instance()->projectile_list_v3.push_back(file_projectilev3());
    }
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@ projectile_list_v3.size[" << SharedData::get_instance()->projectile_list_v3.size() << "]" << std::endl;

}

void gameManager::loadMapData()
{
    // FILE V5 //
    SharedData::get_instance()->file_v5_map_header_list = fio_cmm.load_from_disk<file_v5_map_header>(SharedData::get_instance()->FILEPATH+FILE_V5_MAP_HEADER_LIST);

    std::cout << ">>>>>>>>>>>>>>>>>> gameManager::loadMapData file_v5_map_header_list.size[" << SharedData::get_instance()->file_v5_map_header_list.size() << "]" << std::endl;

    SharedData::get_instance()->file_v5_map_link_list = fio_cmm.load_from_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH+FILE_V5_MAP_LINK_LIST);

    for (int i=0; i<SharedData::get_instance()->file_v5_map_header_list.size(); i++) {
        SharedData::get_instance()->file_v5_map_tile_map.insert(std::pair<int, std::vector<file_v5_map_tile>>(i, std::vector<file_v5_map_tile>()));
        char mapName[FS_CHAR_FILENAME_SIZE];
        sprintf(mapName, "/data/v5_map_%d_tiles.dat", i);
        SharedData::get_instance()->file_v5_map_tile_map.at(i) = fio_cmm.load_from_disk<file_v5_map_tile>(SharedData::get_instance()->FILEPATH+std::string(mapName));
    }

}

void gameManager::initGame()
{
    player1.initialize();
    player1.initFrames();
    player1.set_is_player(true);
    player1.reset_hp();
    config_manager.set_player_ref(&player1);

    fps_manager.initialize();
    mapController.loadMap();

}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void gameManager::show_game(bool can_characters_move, bool can_scroll_stage)
{
    if (SharedData::get_instance()->leave_game == true) {
        exit_game();
    }
    if (player1.is_teleporting() == false) { // ignore input while player is teleporting because it caused some issues
        InputController::get_instance()->read_input();
    }

    // TODO::IURI //
    /*
    if (config_manager.execute_ingame_menu()) { // game is paused
        return;
    }
    */

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
        exit(EXIT_SUCCESS);
    }


    if (TimerView::get_instance()->is_paused() == false) {
        if (can_scroll_stage == true) {
            update_stage_scrolling();
        }
        mapController.move_objects(TimerView::get_instance()->is_paused());
    }

    if (_dark_mode == false) {
        mapController.show();
    }

    if (can_characters_move == true) {
        player1.execute();
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
        mapController.show_npcs();
        player1.show();
        mapController.show_above_objects();
        mapController.showAbove();
    } else {
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    }


    //std::cout << "GFX_MODE[" << (int)mapController.get_current_map_gfx_mode() << "]" << std::endl;

    // TODO::IRUI //
    /*
    if (mapController.get_current_map_gfx_mode() == SCREEN_GFX_MODE_OVERLAY) {
        draw::get_instance()->show_gfx();
    }
    */

    // draw HUD
    draw::get_instance()->show_hud(player1.get_current_hp(), 1, 0, 0);

    if (show_fps_enabled == true) {
        fps_manager.fps_count();
    }
    fps_manager.limit();

}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
Uint8 gameManager::getMapPointLock(struct st_position pos)
{
    return mapController.getMapPointLock(pos);
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
st_float_position gameManager::checkScrolling()
{
    st_float_position move;
    st_float_position mapScroll = mapController.getMapScrolling();
    st_float_position p1Pos(player1.getPosition().x,  player1.getPosition().y);

    move.x += (p1Pos.x - mapScroll.x) - RES_W/2;

    if (mapScroll.x + move.x < 0 || mapScroll.x + move.x > mapController.get_size().width*TILESIZE) {
        move.x = 0;
	}

	return move;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void gameManager::start_stage()
{
	_show_boss_hp = false;
    InputController::get_instance()->clean();


    mapController.reset_map();

    /// @TODO - this must be on a single method in soundlib
    player1.set_position(st_position(RES_W/2 - 29/2, -TILESIZE));

	SoundView::get_instance()->stop_music();

    SoundView::get_instance()->load_stage_music(SharedData::get_instance()->file_v5_map_header_list.at(mapController.get_number()).music_filename);

    mapController.loadMap();

    player1.cancel_slide();
    player1.reset_jump();

    player1.clean_projectiles();
    player1.set_direction(ANIM_DIRECTION_RIGHT);
    player1.reset_hp();

    mapController.show();
    mapController.showAbove();
    //draw::get_instance()->update_screen();
    draw::get_instance()->fade_in_screen(0, 0, 0, 1000);

    game_unpause();

    SoundView::get_instance()->play_music();

    for (int i=0; i<AUTOSCROLL_START_DELAY_FRAMES; i++) { // extra delay to show dialogs
        InputController::get_instance()->read_input();
        InputController::get_instance()->clean_confirm_button();
        mapController.show();
        mapController.showAbove();
        draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }
    // TODO::IURI //
    //mapController.add_autoscroll_delay();

    show_player_teleport(PLAYER_INITIAL_X_POS, -1);
    show_game(false, false);
    // reset timers for objects
    mapController.reset_objects_timers();


}

void gameManager::set_player_position_teleport_in(int initial_pos_x, int initial_pos_y)
{
    int first_unlocked_from_bottom = mapController.get_first_lock_on_bottom(initial_pos_x, initial_pos_y, player1.get_size().width, player1.get_size().height);

    //std::cout << ">>>>>>>>>> GAME::set_player_position_teleport_in::first_unlocked_from_bottom[" << first_unlocked_from_bottom << "]" << std::endl;

    player1.set_position(st_position(initial_pos_x, (first_unlocked_from_bottom+1)*TILESIZE-player1.get_size().height));
    player1.char_update_real_position();
    player1.set_animation_type(ANIM_TYPE_TELEPORT);
    player1.set_animation_frame(0);

    //std::cout << ">>>>>>>>>> GAME::set_player_position_teleport_in::DONE" << std::endl;
}

void gameManager::show_player_teleport(int pos_x, int pos_y)
{
    //std::cout << "GAME::show_player_telport #2" << std::endl;

    // find ground for player
    set_player_position_teleport_in(pos_x, pos_y);
    long end_time = TimerView::get_instance()->getTimer() + 1500;

    //std::cout << "GAME::show_player_telport #2" << std::endl;

    while (TimerView::get_instance()->getTimer() < end_time) {
        mapController.show();
        mapController.showAbove();
        if (player1.animation_has_restarted()) {
            player1.set_animation_frame(1);
            player1.set_animation_has_restarted(false);
        }
        player1.show();
        draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }

    //std::cout << "GAME::show_player_telport #3" << std::endl;
    player1.set_animation_frame(2);
    player1.show();
    draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(20);
    //std::cout << "GAME::show_player_telport #4" << std::endl;

    show_ready();
    //std::cout << "GAME::show_player_telport #5" << std::endl;
    // force stand to avoid gravity not doing it for any reason
    player1.set_animation_type(ANIM_TYPE_STAND);
    mapController.show();
    mapController.showAbove();
    player1.show();
    draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(20);

}

void gameManager::show_ready()
{
    //std::cout << "SHOW READY CALL" << std::endl;
    draw::get_instance()->show_ready();
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void gameManager::restart_stage()
{

    InputController::get_instance()->clean_all();

    //std::cout << "### RESTART_STAGE::START ###" << std::endl;

    if (SharedData::get_instance()->checkpoint.x < TILESIZE*4) {
        SharedData::get_instance()->checkpoint.x = TILESIZE*4;
    }
    // remove any used teleporter
    draw::get_instance()->fade_out_screen(0, 0, 0, 500);

    player1.set_teleporter(-1);
    _player_teleporter.active = false;

    remove_all_projectiles();
    remove_players_slide();

	_show_boss_hp = false;
    InputController::get_instance()->clean();
    mapController.loadMap();
	// TODO - this must be on a single method in soundlib

    player1.clean_projectiles();
    player1.set_animation_type(ANIM_TYPE_TELEPORT);

    player1.reset_hp();
    player1.reset_jump();
    player1.cancel_slide();

    game_unpause();

    mapController.show();
    mapController.showAbove();
    draw::get_instance()->update_screen();
    // if was on stage-boss, mneeds to reload music
    if (SoundView::get_instance()->get_is_playing_boss_music() == true) {
        SoundView::get_instance()->load_stage_music(SharedData::get_instance()->file_v5_map_header_list.at(mapController.get_number()).music_filename);
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
        draw::get_instance()->update_screen();
    }
    for (int i=0; i<AUTOSCROLL_START_DELAY_FRAMES; i++) { // extra delay to teleport without moving screen
        InputController::get_instance()->clean_all();
        show_game(false, false);
        draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(20);
    }

}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
bool gameManager::show_game_intro()
{


    show_notice();

#ifdef BETA_VERSION
    show_beta_version_warning();
#endif

    mapController.set_number(0);

    scenes.main_screen();
	initGame();

    SoundView::get_instance()->stop_music();
    start_stage();

    return true;
}

void gameManager::show_beta_version_warning()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    TextView::get_instance()->renderText(0, 30, st_color(255, 130, 0), true, "-- BETA VERSION WARNING --");
    TextView::get_instance()->draw_centered_text(60, "THIS IS A TEST VERSION OF ROCKDROID,");
    TextView::get_instance()->draw_centered_text(75, "IT DOES CONTAIN ERRORS AND IS NOT");
    TextView::get_instance()->draw_centered_text(90, "COMPLETE MISSING SOME FEATURES.");

    TextView::get_instance()->draw_centered_text(120, "SOFTWARE IS PROVIDED \"AS IS\"");
    TextView::get_instance()->draw_centered_text(135, "WITHOUT WARRANTY OF ANY KIND,");
    TextView::get_instance()->draw_centered_text(150, "EXPRESS OR IMPLIED FROM AUTHOR.");

    TextView::get_instance()->draw_centered_text(170, "REPORT ANY FOUND ISSUES TO");
    TextView::get_instance()->draw_centered_text(185, "bugs@upperland.net");
    TextView::get_instance()->draw_centered_text(210, "PRESS A BUTTON OR KEY TO CONTINUE.");
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
}

void gameManager::show_free_version_warning()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    TextView::get_instance()->renderText(0, 10, st_color(255, 130, 0), true, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning_title, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(30, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(45, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(60, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(75, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning4, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(90, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning5, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(105, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning6, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(130, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning7, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(145, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning8, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(160, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning9, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(175, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning10, SharedData::get_instance()->game_config.selected_language));

    TextView::get_instance()->draw_centered_text(205, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning11, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(220, strings_map::get_instance()->get_ingame_string(string_press_key_or_button, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
}

void gameManager::show_notice()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    draw::get_instance()->update_screen();

    st_imageData upperland_surface;
    upperland_surface = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->GAMEPATH + "/shared/images/upperland.png");

    st_position logo_pos(RES_W/2 - (upperland_surface.surface->w/6)/2, RES_H/2 - upperland_surface.surface->h/2);


    TextView::get_instance()->renderText(0, logo_pos.y + upperland_surface.surface->h + 7, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_upperland_studios, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(0, logo_pos.y + upperland_surface.surface->h + 19, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_presents, SharedData::get_instance()->game_config.selected_language));


    //std::cout << ">> logo_pos.x: " << logo_pos.x << ", logo_pos.y: " << logo_pos.y << std::endl;
    ImageView::get_instance()->renderTexturePortionAt(0, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);
    TextView::get_instance()->draw_centered_text(220, "HTTP://ROCKBOT.UPPERLAND.NET");
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean_and_wait_scape_time(400);
    for (int i=1; i<6; i++) {
        ImageView::get_instance()->renderTexturePortionAt((upperland_surface.surface->w/6)*i, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);
        draw::get_instance()->update_screen();
        InputController::get_instance()->wait_scape_time(30);
    }
    ImageView::get_instance()->renderTexturePortionAt(0, 0, upperland_surface.surface->w/6, upperland_surface.surface->h, logo_pos.x, logo_pos.y, upperland_surface.texture);


    draw::get_instance()->update_screen();

    InputController::get_instance()->clean_and_wait_scape_time(1200);


    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    TextView::get_instance()->renderText(0, 10, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_engine1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(30, strings_map::get_instance()->get_ingame_string(string_intro_engine2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(50, strings_map::get_instance()->get_ingame_string(string_intro_engine3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(70, strings_map::get_instance()->get_ingame_string(string_intro_engine4, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(90, strings_map::get_instance()->get_ingame_string(string_intro_engine5, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(110, strings_map::get_instance()->get_ingame_string(string_intro_engine6, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(130, strings_map::get_instance()->get_ingame_string(string_intro_engine7, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(150, strings_map::get_instance()->get_ingame_string(string_intro_engine8, SharedData::get_instance()->game_config.selected_language));

    draw::get_instance()->update_screen();

    TimerView::get_instance()->delay(10000);

    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);

    TextView::get_instance()->renderText(0, 10, st_color(199, 215, 255), true, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning_title, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(30, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(50, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(70, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(90, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning4, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(110, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning5, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(130, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning6, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(150, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning7, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(170, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning8, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(200, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning9, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->draw_centered_text(220, strings_map::get_instance()->get_ingame_string(string_intro_demo_warning10, SharedData::get_instance()->game_config.selected_language));

    draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(10000);
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
}

void gameManager::show_in_memorian()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    draw::get_instance()->update_screen();

    TextView::get_instance()->draw_centered_text(60, "IN MEMORIAN TO MY OLD BROTHER");
    TextView::get_instance()->draw_centered_text(100, "IVAN FIEDORUK");
    TextView::get_instance()->draw_centered_text(120, "AUGUST, 27, 1973 - MAY, 16, 2018");
    TextView::get_instance()->draw_centered_text(160, "CREATOR OF APEBOT");
    TextView::get_instance()->draw_centered_text(180, "REST IN PEACE");
    draw::get_instance()->fade_in_screen(0, 0, 0, 2000);
    draw::get_instance()->update_screen();
    TimerView::get_instance()->delay(4000);
    draw::get_instance()->fade_out_screen(0, 0, 0, 2000);
}







void gameManager::set_current_map(int temp_map_n)
{
    mapController.set_number(temp_map_n);
    mapController.loadMap();
}

Uint8 gameManager::get_current_map()
{
    return mapController.get_number();;
}


void gameManager::map_present_boss(bool show_dialog, bool is_static_boss)
{
	is_showing_boss_intro = true;

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
        draw::get_instance()->update_screen();
	}

	// 2. blink screen
	ImageView::get_instance()->blink_screen(255, 255, 255);

	// 3. move boss from top to ground
    classnpc* boss_ref = mapController.get_near_boss();
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
    }
    show_stage(8, false);


    SoundView::get_instance()->play_boss_music();

    TimerView::get_instance()->delay(100);

	_show_boss_hp = true;
	is_showing_boss_intro = false;

}

GameObject* gameManager::get_player_platform()
{
    return player1.get_platform();
}

void gameManager::check_player_return_teleport()
{
    remove_all_projectiles();
    remove_players_slide();
	if (is_player_on_teleporter() == true) {
        finish_player_teleporter();
    }
}

bool gameManager::must_show_boss_hp()
{
	return _show_boss_hp;
}



// ********************************************************************************************** //
// remove the projectiles from the list of all players and npcs                                   //
// ********************************************************************************************** //
void gameManager::remove_all_projectiles()
{
    player1.clean_projectiles();
    mapController.clean_map_npcs_projectiles();
    player1.remove_freeze_effect();
}

void gameManager::reset_beam_objects()
{
    mapController.reset_beam_objects();
}

void gameManager::remove_temp_objects()
{
    mapController.remove_temp_objects();
}

void gameManager::remove_players_slide()
{
    player1.cancel_slide();
}

// TODO::IURI - refazer, pois não deve desenhar na tela, e sim ter 2 mapas ao mesmo tempo //
//TRANSITION_TOP_TO_BOTTOM, TRANSITION_BOTTOM_TO_TOP
void gameManager::transition_screen(Uint8 type, Uint8 map_n, short int adjust_x, classPlayer *pObj) {
    st_imageData temp_screen;
    short i = 0;
    temp_screen = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H*2));

    mapController.set_bg_scroll(mapController.get_bg_scroll());

    ImageView::get_instance()->renderTexturePortionAt(0, i*TRANSITION_STEP, RES_W, RES_H, 0, 0, temp_screen.texture);

    // if map destiny and map origin are the same, adjust player's X position
    if (mapController.get_number() == map_n) {
        //std::cout << "p.x[" << (int)test_player->getPosition().x << "], p.real.x[" << test_player->get_real_position().x << "]" << std::endl;
        pObj->set_position(st_position(pObj->get_real_position().x+adjust_x, pObj->get_real_position().y));
        //adjust_x += TILESIZE;
    }

    // TODO: adjust player X position when changing from the same map
    // pegar posição relativa do jogador em relação à tela
    // posição nova é o scroll-x novo mais essa diferença

    // draw map in the screen, erasing all players/objects/npcs/GFX
    draw::get_instance()->set_gfx(SCREEN_GFX_NONE, BG_SCROLL_MODE_NONE);
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
        for (i=0; i<(RES_H)/TRANSITION_STEP; i++) {
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
            draw::get_instance()->show_hud(player1.get_current_hp(), 1, 0, 0);


            draw::get_instance()->update_screen();
#if !defined(PLAYSTATION2) && !defined(ANDROID)
            TimerView::get_instance()->delay(6);
            //TimerView::get_instance()->delay(100); // DEBUG //
#endif
		}

        if (type == TRANSITION_TOP_TO_BOTTOM) {
            mapController.changeScrolling(st_float_position(mapController.getMapScrolling().x, 0));
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


void gameManager::horizontal_screen_move(short direction, bool is_door, short tileX)
{
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



    int move_limit = (RES_W/abs((float)scroll_move.x)) - TILESIZE/abs((float)scroll_move.x);
    float player_move_x = (float)(TILESIZE*2.5)/(float)move_limit; // player should move two tilesize, to avoid doors
    if (scroll_move.x < 0) {
        player_move_x = player_move_x * -1;
    }
    int static_scroll_x = mapController.getMapScrolling().x;


    std::cout << "player_move_x[" << player_move_x << "], move_limit[" << move_limit << "]" << std::endl;
    for (int i=0; i<move_limit; i++) {
        change_map_scroll(scroll_move, false, true);
        mapController.show();
        if (mapController.must_show_static_bg() == false) {
            mapController.show_npcs();
        } else {
            mapController.show_npcs_to_left(static_scroll_x+RES_W);
        }
        player1.show();
        mapController.showAbove();
        mapController.show_above_objects();
        // draw HUD
        draw::get_instance()->show_hud(player1.get_current_hp(), 1, 0, 0);
#if defined(PC)
        TimerView::get_instance()->delay(2);
#endif
        draw::get_instance()->update_screen();

        player1.inc_position(player_move_x, 0);
        /*
        if (i%(TILESIZE/4) == 0) {
            player1.set_position(st_position(player1.getPosition().x+scroll_move.x, player1.getPosition().y));
            player1.char_update_real_position();
		}
        */
	}
    if (is_door == true) {
        remove_players_slide();
    }
    TimerView::get_instance()->delay(6);
    game_unpause();
    add_autoscroll_delay();
    mapController.show();
}

void gameManager::show_door_animation()
{
    int steps = 50;
    remove_players_slide();

    TimerView::get_instance()->delay(6);
    game_unpause();
    mapController.show();
}





void gameManager::leave_stage()
{
    save_game();
    draw::get_instance()->set_flash_enabled(false);

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

void gameManager::return_to_intro_screen()
{
    save_game();

    draw::get_instance()->set_flash_enabled(false);

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

void gameManager::game_pause()
{
    TimerView::get_instance()->pause();
    // @TODO - save-player-input
    player1.save_input();
}

void gameManager::game_unpause()
{
    TimerView::get_instance()->unpause();
    InputController::get_instance()->read_input();
    player1.restore_input();
    player1.reset_sprite_animation_timer();
    mapController.reset_map_timers();
}

void gameManager::exit_game()
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


void gameManager::show_ending()
{
    // save the data indicating game was finished, so user can see ending later or get access to more features
    SharedData::get_instance()->game_config.game_finished = true;
    fio.save_config(SharedData::get_instance()->game_config);

    draw::get_instance()->show_credits(false);

    return_to_intro_screen();
}

void gameManager::quick_load_game()
{
    if (fio.save_exists(current_save_slot)) {
        fio.read_save(SharedData::get_instance()->game_save, current_save_slot);
    }

    SharedData::get_instance()->game_save.difficulty = DIFFICULTY_NORMAL;
    SharedData::get_instance()->game_save.selected_player = PLAYER_2;

    scenes.preloadScenes();



    // DEBUG //

    initGame();

    // DEBUG //
    //show_ending();

    //game_save.armor_pieces[ARMOR_TYPE_LEGS] = true;

    start_stage();

    //got_weapon();
}

void gameManager::set_player_direction(ANIM_DIRECTION dir)
{
    player1.set_direction(dir);
}

void gameManager::update_stage_scrolling()
{
    if (TimerView::get_instance()->is_paused() == true) {
        return;
    }
    mapController.changeScrolling(checkScrolling(), true);
    st_position p_pos = player1.get_real_position();
    //std::cout << "p_pos.x: " << p_pos.x << std::endl;
    if (p_pos.x < 0.0) {
        player1.change_position_x(1);
        // out of screen, probably because was pushed out on a autoscroll stage
        if (p_pos.x < -(TILESIZE*2)) {
                player1.damage(999, true);
            }
    }
}



void gameManager::draw_explosion(st_position center, bool show_players) {
    draw::get_instance()->update_screen();
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
        draw::get_instance()->draw_explosion(center, i, angle_inc);
        for (int k=50; k<250; k+=80) {
            if (i > k) {
                draw::get_instance()->draw_explosion(center, i-k, angle_inc);
            }
        }
        draw::get_instance()->update_screen();
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

void gameManager::show_player()
{
    player1.show();
}

void gameManager::set_player_position(st_position pos)
{
    player1.set_position(pos);
    player1.char_update_real_position();
}

void gameManager::change_player_position(short xinc, short yinc)
{
    player1.change_position(xinc, yinc);
    player1.char_update_real_position();
}

void gameManager::set_player_anim_type(ANIM_TYPE anim_type)
{
    player1.set_animation_type(anim_type);
}

void gameManager::show_player_at(int x, int y)
{
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKDROID2###", "### GAME::show_player_at[%d, %d] ###", x, y);
#endif
    //std::cout << "show_player_at[" << x << ", " << y << "]" << std::endl;
    player1.show_at(st_position(x, y));
}

st_position gameManager::get_player_position()
{
    return st_position(player1.getPosition().x, player1.getPosition().y);
}

st_size gameManager::get_player_size()
{
    return player1.get_size();
}

void gameManager::set_player_direction(Uint8 direction)
{
    player1.set_direction(direction);
}

void gameManager::walk_character_to_screen_point_x(character *char_obj, short pos_x)
{
	/// @TODO: jump obstacles
	if (char_obj->get_real_position().x+char_obj->get_size().width/2 > pos_x) {
        char_obj->set_animation_type(ANIM_TYPE_WALK);
		char_obj->set_direction(ANIM_DIRECTION_LEFT);
		while (char_obj->get_real_position().x+char_obj->get_size().width/2 > pos_x) {
			char_obj->set_position(st_position(char_obj->getPosition().x-2, char_obj->getPosition().y));
            mapController.show();
            mapController.showAbove();
            mapController.show_npcs();
            player1.show();
            draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(20);
		}
	} else if (char_obj->get_real_position().x+char_obj->get_size().width/2 < pos_x) {
		char_obj->set_direction(ANIM_DIRECTION_RIGHT);
        char_obj->set_animation_type(ANIM_TYPE_WALK);
		while (char_obj->get_real_position().x+char_obj->get_size().width/2 < pos_x) {
			char_obj->set_position(st_position(char_obj->getPosition().x+2, char_obj->getPosition().y));
            mapController.show();
            mapController.showAbove();
            mapController.show_npcs();
            player1.show();
            draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(20);
		}
	}

}

void gameManager::set_player_teleporter(short set_teleport_n, st_position set_player_pos, bool is_object)
{
    _player_teleporter.is_object = is_object;
	_player_teleporter.teleporter_n = set_teleport_n;
	_player_teleporter.old_player_pos.x = set_player_pos.x;
	_player_teleporter.old_player_pos.y = set_player_pos.y;

    std::cout << "################### SET PLAYER TELEPORTER ###################" << std::endl;

	_player_teleporter.active = true;
	_player_teleporter.finished = false;
    _player_teleporter.old_map_scroll = mapController.getMapScrolling();
    _player_teleporter.old_map_n = mapController.get_number();
}

bool gameManager::is_player_on_teleporter()
{
    std::cout << "######## is_player_on_teleporter[" << _player_teleporter.active << "] ########" << std::endl;
    return _player_teleporter.active;
}


short gameManager::get_current_save_slot()
{
    return current_save_slot;
}

void gameManager::set_current_save_slot(short n)
{
    current_save_slot = n;
}

void gameManager::save_game()
{
    if (fio.write_save(SharedData::get_instance()->game_save, current_save_slot) == false) {
        show_savegame_error();
        return;
    }
}

void gameManager::set_show_fps_enabled(bool enabled)
{
    show_fps_enabled = enabled;
}

bool gameManager::get_show_fps_enabled()
{
    return show_fps_enabled;
}

void gameManager::add_autoscroll_delay()
{
    autoscroll_timer = TimerView::get_instance()->getTimer()+800;
}




void gameManager::remove_current_teleporter_from_list()
{
    if (_player_teleporter.teleporter_n != -1) {
        _last_stage_used_teleporters.erase(_player_teleporter.teleporter_n);
    }
    player1.set_teleporter(-1);
}

void gameManager::select_game_screen()
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
    draw::get_instance()->update_screen();
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

std::string gameManager::get_selected_game()
{
    return _selected_game;
}

void gameManager::finish_player_teleporter()
{
    remove_all_projectiles();
    remove_players_slide();
    player1.recharge(ENERGY_TYPE_HP, ENERGY_ITEM_BIG);
    draw::get_instance()->fade_out_screen(0, 0, 0, 500);
    TimerView::get_instance()->delay(1000);

    std::cout << "################### RESET PLAYER TELEPORTER ###################" << std::endl;
    _player_teleporter.active = false;
    _last_stage_used_teleporters.insert(std::pair<int,bool>(_player_teleporter.teleporter_n, true));
	// teleport out
	SoundView::get_instance()->play_sfx(SFX_TELEPORT);
    draw::get_instance()->fade_out_screen(0, 0, 0, 500);
    _player_teleporter.old_player_pos.y -= 5;
    player1.set_position(_player_teleporter.old_player_pos);
    mapController.set_number(_player_teleporter.old_map_n);
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
    SoundView::get_instance()->load_stage_music(SharedData::get_instance()->file_v5_map_header_list.at(mapController.get_number()).music_filename);
    SoundView::get_instance()->play_music();
}

void gameManager::show_stage(int wait_time, bool move_npcs)
{
    if (_dark_mode == false) {
        mapController.show();
    }
	if (move_npcs == true) {
        mapController.move_npcs();
	}
    if (_dark_mode == false) {
        mapController.show_npcs();
        player1.show();
        mapController.showAbove();
    }
	if (wait_time > 0) {
        TimerView::get_instance()->delay(wait_time);
	}
    draw::get_instance()->update_screen();
}

bool gameManager::subboss_alive_on_left(short tileX)
{
    return mapController.subboss_alive_on_left(tileX);
}

void gameManager::change_map_scroll(st_float_position pos, bool check_lock, bool ignore_auto_scroll)
{
    // debug for autoscrolling test

    bool map_autoscroll = SharedData::get_instance()->file_v5_map_header_list.at(mapController.get_number()).autoscroll;
    // avoid data error (getting 66 as value from data file)
    if (map_autoscroll > 1) {
        map_autoscroll = false;
    }
    if (ignore_auto_scroll == false && map_autoscroll == true) {
        if (TimerView::get_instance()->is_paused() == false && autoscroll_timer < TimerView::get_instance()->getTimer()) {
            autoscroll_timer = TimerView::get_instance()->getTimer()+20;
            pos.x = 1.5;
        } else {
            pos.x = 0;
        }
    }
   mapController.changeScrolling(pos, check_lock);
}

MapController *gameManager::get_current_map_obj()
{
    return &mapController;
}

void gameManager::object_teleport_boss(st_position dest_pos, Uint8 dest_map, Uint8 teleporter_id, bool must_return)
{
    // checa se já foi usado
    if (_last_stage_used_teleporters.find(teleporter_id) != _last_stage_used_teleporters.end()) {
        return;
    }
    std::cout << "############################################ TELEPORT #2" << std::endl;
    if (must_return) {
        set_player_teleporter(teleporter_id, st_position(player1.getPosition().x, player1.getPosition().y), true);
    }
    draw::get_instance()->fade_out_screen(0, 0, 0, 500);
    draw::get_instance()->update_screen();
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

    draw::get_instance()->update_screen();
}



bool gameManager::show_config(short finished_stage)
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

void gameManager::show_savegame_error()
{
    std::vector<std::string> msgs;
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror1, SharedData::get_instance()->game_config.selected_language));
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror2, SharedData::get_instance()->game_config.selected_language));
    msgs.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_savegameerror3, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->show_ingame_warning(msgs);
}

void gameManager::get_drop_item_ids()
{
    for (int i=0; i<DROP_ITEM_COUNT; i++) {
        _drop_item_list[i] = -1;
    }
    for (int i=0; i<GameMediator::get_instance()->object_list.size(); i++) {
        if (GameMediator::get_instance()->object_list.at(i).type == OBJ_LIFE) {
            _drop_item_list[DROP_ITEM_1UP] = i;
        } else if (GameMediator::get_instance()->object_list.at(i).type == OBJ_ENERGY_PILL_SMALL) {
            _drop_item_list[DROP_ITEM_ENERGY_SMALL] = i;
        } else if (GameMediator::get_instance()->object_list.at(i).type == OBJ_ENERGY_PILL_BIG) {
            _drop_item_list[DROP_ITEM_ENERGY_BIG] = i;
        } else if (GameMediator::get_instance()->object_list.at(i).type == OBJ_WEAPON_PILL_SMALL) {
            _drop_item_list[DROP_ITEM_WEAPON_SMALL] = i;
        } else if (GameMediator::get_instance()->object_list.at(i).type == OBJ_WEAPON_PILL_BIG) {
            _drop_item_list[DROP_ITEM_WEAPON_BIG] = i;
        }
    }
}

character *gameManager::get_player()
{
    return &player1;
}

st_float_position gameManager::get_current_stage_scroll()
{
    return mapController.getMapScrolling();
}

void gameManager::reset_scroll()
{
    mapController.reset_scrolling();
}

short gameManager::get_drop_item_id(short type)
{
    return _drop_item_list[type];
}

void gameManager::show_map()
{
    mapController.show();
    mapController.showAbove();
    draw::get_instance()->update_screen();
}


