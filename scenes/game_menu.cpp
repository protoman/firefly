#include "game_menu.h"

#ifdef ANDROID
#include <android/log.h>
#endif


#include "ports/android/android_game_services.h"

#include "controller/inputcontroller.h"

#include "view/imageview.h"
#include "view/draw.h"
#include "view/textview.h"

#include "scenes/dialogs.h"

#include "gameManager.h"

game_menu::game_menu()
{

}

void game_menu::show_extra_menu()
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


    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_about, SharedData::get_instance()->game_config.selected_language)));
    option_picker cheat_config_picker(false, config_text_pos, options, true);

    while (selected_option != -1) {
        option_picker extra_menu_picker(false, config_text_pos, options, true);
        ImageView::get_instance()->show_config_bg();
        extra_menu_picker.draw();
        selected_option = extra_menu_picker.pick(selected_option+1);
        // ABOUT/CREDITS //
        if (selected_option == 0) {
            // only wait for keypress if user did not interrupted credits
            if (draw::get_instance()->show_credits(true) == 0) {
                InputController::get_instance()->wait_keypress();
            }
        }
    }
}

short game_menu::show_main_config(short stage_finished, bool called_from_game)
{
    short res = 0;
    st_position config_text_pos;
    std::vector<st_menu_option> options;

    ImageView::get_instance()->show_config_bg();
    draw::get_instance()->update_screen();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);

    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_audio, SharedData::get_instance()->game_config.selected_language)));
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_input, SharedData::get_instance()->game_config.selected_language)));
#if defined(PC)
    options.push_back(st_menu_option("PC"));
#elif ANDROID
    options.push_back(st_menu_option("ANDROID"));
#else
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_wii_platformspecific, game_config.selected_language), true));
#endif
    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_language, SharedData::get_instance()->game_config.selected_language)));

    options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_config_graphics_performance, SharedData::get_instance()->game_config.selected_language)));

    options.push_back( strings_map::get_instance()->get_ingame_string(strings_ingame_extras, SharedData::get_instance()->game_config.selected_language));

    if (called_from_game) {
        if (stage_finished) {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_leavestage, SharedData::get_instance()->game_config.selected_language)));
        } else {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_ingame_leavestage, SharedData::get_instance()->game_config.selected_language), true));
        }
        options.push_back( strings_map::get_instance()->get_ingame_string(strings_ingame_config_quitgame, SharedData::get_instance()->game_config.selected_language));
    }



    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 20;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;

    short selected_option = 0;
    bool picked_once = false;
    while (selected_option != -1) {
        option_picker main_config_picker(false, config_text_pos, options, true);
        if (picked_once == false) {
            selected_option = main_config_picker.pick();
            picked_once = true;
        } else {
            selected_option = main_config_picker.pick(selected_option+1);
        }
        if (selected_option == -1) {
            break;
        }
        ImageView::get_instance()->clearScreenArea(config_text_pos.x-1, config_text_pos.y-1, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        draw::get_instance()->update_screen();
        if (selected_option == 0) { // CONFIG AUDIO //
            show_config_audio();
        } else if (selected_option == 1) { // CONFIG INPUT //
            key_map key_mapper;
            key_mapper.config_input();
        } else if (selected_option == 2) { // CONFIG PLATFORM //
#ifdef PC
            show_config_video();
            //show_config_android(); // line used for test/debug
#elif ANDROID
            show_config_android();
#endif
        } else if (selected_option == 3) { // LANGUAGE //
            show_config_language();
        } else if (selected_option == 4) { // PERFORMANCE //
            show_config_performance();
        } else if (selected_option == 5) { // LEAVE STAGE //
            res = 1;
            break;
        } else if (selected_option == 6) { // QUIT GAME //
            dialogs dialogs_obj;
            if (dialogs_obj.show_leave_game_dialog() == true) {
                SDL_Quit();
                exit(0);
            }
        }
        fio.save_config(SharedData::get_instance()->game_config);

        ImageView::get_instance()->clearScreenArea(config_text_pos.x-1, config_text_pos.y-1, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        draw::get_instance()->update_screen();
    }
    return res;
}

void game_menu::show_config_android()
{
#ifdef ANDROID
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    short selected_option = 0;

    std::vector<st_menu_option> options;

    while (selected_option != -1) {
        options.clear();

        // OPTION #0: SHOW/HIDE controls
        if (game_config.android_touch_controls_hide == false) {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_android_hidescreencontrols, game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_disabled, game_config.selected_language)));
        } else {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_android_hidescreencontrols, game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_enabled, game_config.selected_language)));
        }
        // OPTION #1: controls size
        if (game_config.android_touch_controls_size == 0) {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize, game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize_SMALL, game_config.selected_language)));
        } else if (game_config.android_touch_controls_size == 2) {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize, game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize_BIG, game_config.selected_language)));
        } else {
            options.push_back(st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize, game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_android_screencontrolssize_MEDIUM, game_config.selected_language)));
        }

        option_picker main_config_picker(false, config_text_pos, options, true);
        selected_option = main_config_picker.pick(selected_option+1);
        if (selected_option == 0) {
            game_config.android_touch_controls_hide = !game_config.android_touch_controls_hide;
            game_services.set_touch_controls_visible(!game_config.android_touch_controls_hide);
            // @TODO: show warning about emergency reset
        } else if (selected_option == 1) {
            game_config.android_touch_controls_size++;
            if (game_config.android_touch_controls_size > 2) {
                game_config.android_touch_controls_size = 0;
            }
            game_services.set_android_default_buttons_size(game_config.android_touch_controls_size);
        }
        ImageView::get_instance()->clear_area(config_text_pos.x-1, config_text_pos.y-1, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        ImageView::get_instance()->updateScreen();
    }
    fio.save_config(game_config);
#endif
}


void game_menu::show_config_video()
{
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);


    short selected_option = 0;

    while (selected_option != -1) {
        std::vector<std::string> options;
        if (SharedData::get_instance()->game_config.video_fullscreen == false) {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_video_windowed, SharedData::get_instance()->game_config.selected_language));
        } else {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_video_fullscreen, SharedData::get_instance()->game_config.selected_language));
        }

        option_picker main_config_picker(false, config_text_pos, options, true);
        selected_option = main_config_picker.pick(selected_option+1);
        if (selected_option == 0) {
            SharedData::get_instance()->game_config.video_fullscreen = !SharedData::get_instance()->game_config.video_fullscreen;
        }
        if (selected_option != -1) {
            fio.save_config(SharedData::get_instance()->game_config);
            show_config_ask_restart();
            st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);
            ImageView::get_instance()->clearScreenArea(menu_pos.x-14, menu_pos.y, RES_W, 100, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
            main_config_picker.draw();
        }
    }
}


void game_menu::show_config_ask_restart()
{
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);


    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+10, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+20, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+40, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_presstorestart, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    draw::get_instance()->update_screen();
}

void game_menu::show_config_audio()
{
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);

    std::vector<std::string> options;
    if (SharedData::get_instance()->game_config.sound_enabled == true) {
        options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_enabled, SharedData::get_instance()->game_config.selected_language));
    } else {
        options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_disabled, SharedData::get_instance()->game_config.selected_language));
    }
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_audio_volume_music, SharedData::get_instance()->game_config.selected_language));
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_audio_volume_sfx, SharedData::get_instance()->game_config.selected_language));


    short selected_option = 0;
    option_picker main_config_picker(false, config_text_pos, options, true);
    selected_option = 0;
    while (selected_option != -1) {
        main_config_picker.draw();
        selected_option = main_config_picker.pick(selected_option+1);
        if (selected_option == 0) {
            if (SharedData::get_instance()->game_config.sound_enabled == false) {
                SoundView::get_instance()->enable_sound();
            } else {
                SoundView::get_instance()->disable_sound();
            }
        } else if (selected_option == 1) {
            config_int_value(SharedData::get_instance()->game_config.volume_music, 1, 128);
            SoundView::get_instance()->update_volumes();
            fio.save_config(SharedData::get_instance()->game_config);
        } else if (selected_option == 2) {
            config_int_value(SharedData::get_instance()->game_config.volume_sfx, 1, 128);
            SoundView::get_instance()->update_volumes();
            fio.save_config(SharedData::get_instance()->game_config);
        }
    }
}

void game_menu::show_config_language()
{
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    std::vector<st_menu_option> options;
    if (SharedData::get_instance()->game_config.selected_language == LANGUAGE_FRENCH) {           // FRENCH
        options.push_back(st_menu_option("ANGLAIS"));
        options.push_back(st_menu_option("FRANCAIS", true));
        options.push_back(st_menu_option("ESPANOL", true));
        options.push_back(st_menu_option("ITALIEN", true));
        options.push_back(st_menu_option("PORTUGAIS"));
    } else if (SharedData::get_instance()->game_config.selected_language == LANGUAGE_SPANISH) {    // SPANISH
        options.push_back(st_menu_option("INGLES"));
        options.push_back(st_menu_option("FRANCES", true));
        options.push_back(st_menu_option("ESPANOL", true));
        options.push_back(st_menu_option("ITALIANO", true));
        options.push_back(st_menu_option("PORTUGUES"));
    } else if (SharedData::get_instance()->game_config.selected_language == LANGUAGE_ITALIAN) {    // ITALIAN
        options.push_back(st_menu_option("INGLESE"));
        options.push_back(st_menu_option("FRANCESE", true));
        options.push_back(st_menu_option("SPAGNOLO", true));
        options.push_back(st_menu_option("ITALIANO", true));
        options.push_back(st_menu_option("PORTOGHESE"));
    } else if (SharedData::get_instance()->game_config.selected_language == LANGUAGE_PORTUGUESE) {    // PORTUGUESE (BRAZILIAN)
        options.push_back(st_menu_option("INGLES"));
        options.push_back(st_menu_option("FRANCES", true));
        options.push_back(st_menu_option("ESPANHOL", true));
        options.push_back(st_menu_option("ITALIANO", true));
        options.push_back(st_menu_option("PORTUGUES"));
    } else {                                            // ENGLISH
        options.push_back(st_menu_option("ENGLISH"));
        options.push_back(st_menu_option("FRENCH", true));
        options.push_back(st_menu_option("SPANISH", true));
        options.push_back(st_menu_option("ITALIAN", true));
        options.push_back(st_menu_option("PORTUGUESE"));
    }

    short selected_option = 0;
    option_picker main_config_picker(false, config_text_pos, options, true);
    while (selected_option != -1) {
        selected_option = main_config_picker.pick(selected_option+1);
        if (selected_option != -1) {
            SharedData::get_instance()->game_config.selected_language = selected_option;
            fio.save_config(SharedData::get_instance()->game_config);
            show_config_ask_restart();
            main_config_picker.draw();
        }
    }
}

void game_menu::show_config_performance()
{
    short selected_option = 0;
    while (selected_option != -1) {
        st_position config_text_pos;
        config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
        config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(300);
        std::vector<std::string> options;

        if (SharedData::get_instance()->game_config.graphics_performance_mode == PERFORMANCE_MODE_LOW) {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_low, SharedData::get_instance()->game_config.selected_language));
        } else if (SharedData::get_instance()->game_config.graphics_performance_mode == PERFORMANCE_MODE_NORMAL) {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_medium, SharedData::get_instance()->game_config.selected_language));
        } else {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_mode, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_config_high, SharedData::get_instance()->game_config.selected_language));
        }

        if (gameManager::get_instance()->get_show_fps_enabled()) {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_video_show_fps, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_on, SharedData::get_instance()->game_config.selected_language));
        } else {
            options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_video_show_fps, SharedData::get_instance()->game_config.selected_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_off, SharedData::get_instance()->game_config.selected_language));
        }



        option_picker main_config_picker(false, config_text_pos, options, true);

        selected_option = main_config_picker.pick(selected_option+1);
        if (selected_option == 0) {
            SharedData::get_instance()->game_config.graphics_performance_mode++;
            if (SharedData::get_instance()->game_config.graphics_performance_mode > PERFORMANCE_MODE_HIGH) {
                SharedData::get_instance()->game_config.graphics_performance_mode = PERFORMANCE_MODE_LOW;
            }
        } else if (selected_option == 1) {
            gameManager::get_instance()->set_show_fps_enabled(!gameManager::get_instance()->get_show_fps_enabled());
        }
        ImageView::get_instance()->clearScreenArea(config_text_pos.x-1, config_text_pos.y-1, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    }

    fio.save_config(SharedData::get_instance()->game_config);
}

void game_menu::show_config_warning_android_play_services()
{
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);

    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_play_services1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+10, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_play_services2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+20, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_play_services3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+40, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_play_services4, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+60, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_presstorestart, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    draw::get_instance()->update_screen();
}

void game_menu::show_config_warning_android_cloud_save()
{
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_cloud_save1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+10, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_cloud_save2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+20, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_cloud_save3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+40, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_android_cloud_save4, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+60, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_presstorestart, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    draw::get_instance()->update_screen();
}

void game_menu::show_config_warning_android_hide_controls()
{
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);
    st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart1, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+10, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart2, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+20, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_restart3, SharedData::get_instance()->game_config.selected_language));
    TextView::get_instance()->renderText(menu_pos.x, menu_pos.y+40, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, strings_map::get_instance()->get_ingame_string(strings_ingame_config_presstorestart, SharedData::get_instance()->game_config.selected_language));
    draw::get_instance()->update_screen();
    InputController::get_instance()->wait_keypress();
    ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, RES_W,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    draw::get_instance()->update_screen();
}


void game_menu::config_int_value(Uint8 &value_ref, int min, int max)
{
    int config_text_pos_x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    int config_text_pos_y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    ImageView::get_instance()->clearScreenArea(config_text_pos_x-1, config_text_pos_y-1, 300, 100, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(10);
    char value[3]; // for now, we handle only 0-999

    TextView::get_instance()->renderText(config_text_pos_x, config_text_pos_y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, "< ");
    TextView::get_instance()->renderText(config_text_pos_x+34, config_text_pos_y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, " >");

    while (true) {
        InputController::get_instance()->read_input();

        if (value_ref < 10) {
            sprintf(value, "00%d", value_ref);
        } else if (value_ref < 100) {
            sprintf(value, "0%d", value_ref);
        } else {
            sprintf(value, "%d", value_ref);
        }
        ImageView::get_instance()->clearScreenArea(config_text_pos_x+11, config_text_pos_y-1, 30, 12, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
        TextView::get_instance()->renderText(config_text_pos_x+12, config_text_pos_y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, std::string(value));
        if (InputController::get_instance()->p1_input[BTN_ATTACK] == 1 || InputController::get_instance()->p1_input[BTN_START] == 1 || InputController::get_instance()->p1_input[BTN_DOWN]) {
            break;
        } else if (InputController::get_instance()->p1_input[BTN_LEFT] == 1) {
            value_ref--;
        } else if (InputController::get_instance()->p1_input[BTN_RIGHT] == 1) {
            value_ref++;
        }
        if (value_ref < min) {
            value_ref = min;
        }
        if (value_ref > max) {
            value_ref = max;
        }
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(10);
        draw::get_instance()->update_screen();
    }

}

