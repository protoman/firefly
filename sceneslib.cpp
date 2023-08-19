#include <cstdlib>
#include "sceneslib.h"
#include "file/fio_scenes.h"
#include "strings_map.h"

#include "game_data.h"

#include "view/animation.h"
#include "view/draw.h"
#include "view/gfx_sin_wave.h"
#include "view/option_picker.h"
#include "view/textview.h"

#include "file/version.h"
#include "file/file_io.h"
#include "file/fio_strings.h"

#include "options/key_map.h"


#include "data/shareddata.h"

#include "GameManager.h"


#ifdef ANDROID
#include <android/log.h>
#include "ports/android/android_game_services.h"
extern android_game_services game_services;
#endif


#include "aux_tools/fps_control.h"

#define TIME_SHORT 120
#define TIME_LONG 300
#define INTRO_DIALOG_DURATION_TIME 4000
#define BOSS_CREDITS_LINES_N 4

// ********************************************************************************************** //
// ScenesLib handles all scinematic like intro and ending                                         //
// ********************************************************************************************** //
scenesLib::scenesLib() : _timer(0), _state(0)
{
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void scenesLib::preloadScenes()
{
    SoundView::get_instance()->load_boss_music(GameData::get_instance()->game_data.boss_music_filename);
}




void scenesLib::draw_main()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    draw::get_instance()->update_screen();

	// PARTE 1 - TITLE SCREEN
    st_imageData intro_screen;
    std::string intro_path = SharedData::get_instance()->FILEPATH + "/images/logo.png";
    intro_screen = ImageView::get_instance()->imageFromFile(intro_path);
    ImageView::get_instance()->zoom_image(st_position(0, 20), intro_screen, false);

    TextView::get_instance()->renderText(8, 8, VERSION_NUMBER);

    TextView::get_instance()->renderText(40, (RES_H-35), strings_map::get_instance()->get_ingame_string(strings_ingame_copyrightline, SharedData::get_instance()->current_language));
    TextView::get_instance()->renderCenteredText(220, "http://rockbot.upperland.net");

}

// ********************************************************************************************** //
// mostra tela de introdução, até alguém apertar start/enter
// a partir daí, mostra tela de seleção de personagem
// ********************************************************************************************** //
void scenesLib::main_screen()
{

    SharedData::get_instance()->leave_game = false;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_music(GameData::get_instance()->game_data.game_start_screen_music_filename);
    SoundView::get_instance()->play_music();
	draw_main();

    std::vector<st_menu_option> options;
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_newgame, SharedData::get_instance()->current_language)));
    if (fio.have_one_save_file() == true) {
        options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_loadgame, SharedData::get_instance()->current_language)));
    } else {
        options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_loadgame, SharedData::get_instance()->current_language), true));
    }
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_config, SharedData::get_instance()->current_language)));
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_about, SharedData::get_instance()->current_language)));

    option_picker main_picker(false, st_position(20, (RES_H*0.5)), options, false);


    main_picker.enable_check_input_reset_command();
    main_picker.enable_check_input_cheat_command();


    bool have_save = fio.have_one_save_file();

    // IF HAVE NO SAVE, TRY TO LOAD IT FROM CLOUD //

    int picked_n = 0;

    if (have_save) {
        picked_n = 1;
    }
	bool repeat_menu = true;
	while (repeat_menu == true) {

        picked_n = main_picker.pick(picked_n);
		if (picked_n == -1) {
            dialogs dialogs_obj;
            if (dialogs_obj.show_leave_game_dialog() == true) {
                SDL_Quit();
                exit(0);
            }
        } else if (picked_n == 0) { // NEW GAME
            short selected_save = select_save(true);
            if (selected_save != -1) {
                repeat_menu = false;
                GameManager::get_instance()->set_current_map(0);
                GameManager::get_instance()->set_current_save_slot(selected_save);
                SharedData::get_instance()->game_save.reset();
            } else {
                InputController::get_instance()->clean_all();
                draw_main();
                main_picker.draw();
            }
        } else if (picked_n == 1) { // LOAD GAME //
            short selected_save = select_save(false);
            if (selected_save != -1) {
                GameManager::get_instance()->set_current_save_slot(selected_save);
                if (have_save == true) {
                    fio.read_save(SharedData::get_instance()->game_save, GameManager::get_instance()->get_current_save_slot());
                    repeat_menu = false;
                }
            } else {
                InputController::get_instance()->clean_all();
                draw_main();
                main_picker.draw();
            }
        } else if (picked_n == 2) { // CONFIG //
            menu.show_main_config(0, false);
			draw_main();
			main_picker.draw();
        } else if (picked_n == 3) { // ABOUT/CREDITS //
            // only wait for keypress if user did not interrupted credits
            if (draw::get_instance()->show_credits(true) == 0) {
                InputController::get_instance()->wait_keypress();
            }
            draw_main();
            main_picker.draw();
        } else if (picked_n == 5) { // CHEATS //
            show_cheats_menu();
            draw_main();
            main_picker.draw();
        } else if (picked_n == MAIN_MENU_CHEAT_RETURN) { // LEAVING CHEATS MENU ITEM //
            picked_n = 5;
            main_picker.add_option_item(st_menu_option("CHEATS"));
            draw_main();
            main_picker.draw();
        }
	}
    draw::get_instance()->update_screen();

    if (picked_n == 0) { // NEW GAME //
        // demo do not have player selection, only rockbot is playable
        SharedData::get_instance()->game_save.selected_player = PLAYER_2;
        GameManager::get_instance()->save_game();
    }
}

void scenesLib::show_cheats_menu()
{
    short res = 0;
    st_position config_text_pos;
    std::vector<st_menu_option> options;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 20;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;

    ImageView::get_instance()->show_config_bg();
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);


    short selected_option = 0;
    int current_player = SharedData::get_instance()->game_save.selected_player;
    while (selected_option != -1) {

        options.clear();



        std::string invencibleStr = "OFF";
        options.push_back(st_menu_option("INVENCIBLE: " + invencibleStr));

        std::string allStagesStr = "OFF";
        options.push_back(st_menu_option("ALL BEATEN: " + allStagesStr));

        char char_n[50];
        sprintf(char_n, "%s", GameData::get_instance()->player_list_v3_1[current_player].name);
        options.push_back(st_menu_option("CHARACTER: " + std::string(char_n)));

        option_picker cheat_config_picker(false, config_text_pos, options, true);
        ImageView::get_instance()->show_config_bg();
        cheat_config_picker.draw();
        selected_option = cheat_config_picker.pick(selected_option+1);

    }


}




Uint8 scenesLib::select_difficulty()
{
    short res = 0;
    st_position config_text_pos;
    std::vector<std::string> options;

    ImageView::get_instance()->show_config_bg();
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);

    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_difficulty_easy, SharedData::get_instance()->current_language));
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_difficulty_normal, SharedData::get_instance()->current_language));
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_difficulty_hard, SharedData::get_instance()->current_language));

    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 60;

    TextView::get_instance()->renderText(config_text_pos.x, ImageView::get_instance()->get_config_menu_pos().y+40, strings_map::get_instance()->get_ingame_string(strings_ingame_difficulty_select, SharedData::get_instance()->current_language).c_str());

    short selected_option = -2;
    while (selected_option == -2) {
        option_picker main_config_picker(false, config_text_pos, options, false);
        selected_option = main_config_picker.pick();
        if (selected_option == -1) {
            dialogs dialogs_obj;
            if (dialogs_obj.show_leave_game_dialog() == true) {
                SDL_Quit();
                exit(0);
            }
        } else {
            res = selected_option;
        }
        std::cout << "############ select_difficulty.selected_option[" << selected_option << "]" << std::endl;
        ImageView::get_instance()->clearScreenArea(config_text_pos.x-1, config_text_pos.y-1, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        draw::get_instance()->update_screen();
    }
    return res;
}








short scenesLib::select_save(bool is_new_game)
{
    int selected = 0;
    bool finished = false;


    std::string filename_selector_enabled = SharedData::get_instance()->FILEPATH + "images/backgrounds/save_selector_enabled.png";
    st_imageData selector_enabled_bg;
    selector_enabled_bg = ImageView::get_instance()->imageFromFile(filename_selector_enabled);

    std::string filename_selector_disabled = SharedData::get_instance()->FILEPATH + "images/backgrounds/save_selector_disabled.png";
    st_imageData selector_disabled_bg;
    selector_disabled_bg = ImageView::get_instance()->imageFromFile(filename_selector_disabled);

    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
    InputController::get_instance()->clean_all();
    TimerView::get_instance()->delay(300);

    st_save save_detail_array[5];
    bool save_slot_exists[5];
    for (int i=0; i<5; i++) {
        if (fio.save_exists(i) == false) {
            save_slot_exists[i] = false;
        } else {
            fio.read_save(save_detail_array[i], i);
            save_slot_exists[i] = true;
        }
    }

    if (is_new_game == true) {
        TextView::get_instance()->renderText(10, 10, "CREATE NEW GAME");
    } else {
        TextView::get_instance()->renderText(10, 10, "LOAD GAME FILE");
    }
    TextView::get_instance()->renderText(10, RES_H-12, "PLEASE SELECT SAVE SLOT");

    while (finished == false) {
        // draw screen
        for (int i=0; i<5; i++) {
            st_imageData* surface_ref = &selector_disabled_bg;
            if (i == selected) {
                surface_ref = &selector_enabled_bg;
            }
            ImageView::get_instance()->renderImageAt(0, (i*surface_ref->surface->h)+22, *surface_ref);
        }
        for (int i=0; i<5; i++) {
            if (save_slot_exists[i] == true) {
                draw_save_details(i, save_detail_array[i]);
            } else {
                TextView::get_instance()->renderText(10, i*40+34, "- NO SAVE FILE -");
            }
        }

        ImageView::get_instance()->updateRender();
        InputController::get_instance()->read_input();
        if (InputController::get_instance()->p1_input[BTN_JUMP] == 1 || InputController::get_instance()->p1_input[BTN_START] == 1) {
            if (is_new_game == false && save_slot_exists[selected] == false) {
                SoundView::get_instance()->play_sfx(SFX_NPC_HIT);
            } else {
                break;
            }
        } else if (InputController::get_instance()->p1_input[BTN_ATTACK] == 1) {
            return -1;
        } else if (InputController::get_instance()->p1_input[BTN_UP] == 1) {
            selected--;
            SoundView::get_instance()->play_sfx(SFX_CURSOR);
            if (selected < 0) {
                selected = SAVE_MAX_SLOT_NUMBER;
            }
        } else if (InputController::get_instance()->p1_input[BTN_DOWN] == 1) {
            selected++;
            SoundView::get_instance()->play_sfx(SFX_CURSOR);
            if (selected > SAVE_MAX_SLOT_NUMBER) {
                selected = 0;
            }
        } else if (InputController::get_instance()->p1_input[BTN_QUIT] == 1) {
            dialogs dialogs_obj;
            if (dialogs_obj.show_leave_game_dialog() == true) {
                SDL_Quit();
                exit(0);
            }
        }
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(10);
    }

    return selected;

}

void scenesLib::draw_save_details(int n, st_save save)
{
    // intro stage is rock buster icon, other are weapons icons
    int y_pos = n*40+34;
    for (int i=0; i<=8; i++) {
        st_position pos((i+1)*18, y_pos);
        // TODO::IURI //
        /*
        if (SharedData::get_instance()->game_save.stages[i] == 1) {
            ImageView::get_instance()->draw_weapon_tooltip_icon(i, pos, true);
        } else {
            // @TODO: draw disabled version
            ImageView::get_instance()->draw_weapon_tooltip_icon(i, pos, false);
        }
        */
        ImageView::get_instance()->draw_weapon_tooltip_icon(i, pos, false);
    }

    // e-tank
    char buffer[3];
    st_position pos_etank(11*18, y_pos);
    ImageView::get_instance()->draw_weapon_tooltip_icon(15, pos_etank, true);
    sprintf(buffer, "x%d", save.items.energy_tanks);
    TextView::get_instance()->renderText(12*18, y_pos+5, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, std::string(buffer));

}

