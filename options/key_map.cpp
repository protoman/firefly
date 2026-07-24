#include "key_map.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#include "controller/inputcontroller.h"

#include "view/option_picker.h"
#include "view/imageview.h"
#include "view/textview.h"
#include "view/timerview.h"
#include "view/draw.h"
#include "data/shareddata.h"

#include "strings_map.h"

#include "defines.h"


key_map::key_map()
{
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_jump, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_fire, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_dash, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_shield, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_keyl, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_keyr, SharedData::get_instance()->current_language) + std::string(": "));
    _keys_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_start, SharedData::get_instance()->current_language) + std::string(": "));
}


void key_map::draw_screen()
{
    bool finished = false;
    st_position config_text_pos;
    st_position cursor_pos;
    short _pick_pos = 0;

    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    cursor_pos = config_text_pos;

    ImageView::get_instance()->show_config_bg();
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(300);

    for (unsigned int i=0; i<_keys_list.size(); i++) {
        std::cout << "_keys_list[i]: " << _keys_list[i] << std::endl;
        TextView::get_instance()->renderText(config_text_pos.x, config_text_pos.y + i*CURSOR_SPACING, _keys_list[i].c_str());
        redraw_line(i);
    }
    TextView::get_instance()->renderText(config_text_pos.x, config_text_pos.y + _keys_list.size()*CURSOR_SPACING, strings_map::get_instance()->get_ingame_string(strings_config_return, SharedData::get_instance()->current_language));
    Draw::get_instance()->update_screen();

    ImageView::get_instance()->drawCursor(st_position(cursor_pos.x-CURSOR_SPACING, cursor_pos.y+(_pick_pos*CURSOR_SPACING)));

    while (finished == false) {
        InputController::get_instance()->read_input();
        if (InputController::get_instance()->p1_input[BTN_START]) {
            if (_pick_pos == (short)_keys_list.size()) {
                finished = true;
            } else {
                TextView::get_instance()->renderText(config_text_pos.x, config_text_pos.y + _keys_list.size()*CURSOR_SPACING+CURSOR_SPACING*2, strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_pressnew, SharedData::get_instance()->current_language)); //input code (number)
                Draw::get_instance()->update_screen();
                ImageView::get_instance()->clearScreenArea(config_text_pos.x, config_text_pos.y + _keys_list.size()*CURSOR_SPACING+CURSOR_SPACING*2-1, 180,  CURSOR_SPACING+1, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
                ///@TODO - key_config[_pick_pos].key_type = new_key.key_type;
                ///@TODO - key_config[_pick_pos].key_number = new_key.key_number;
                redraw_line(_pick_pos);
                Draw::get_instance()->update_screen();
            }
        }
        if (InputController::get_instance()->p1_input[BTN_DOWN]) {
                SoundView::get_instance()->play_sfx(SFX_CURSOR);
                ImageView::get_instance()->eraseCursor(st_position(cursor_pos.x-CURSOR_SPACING, cursor_pos.y+(_pick_pos*CURSOR_SPACING)));
                _pick_pos++;
                if (_pick_pos >= (short)_keys_list.size()+1) {
                    _pick_pos = 0;
                }
                ImageView::get_instance()->drawCursor(st_position(cursor_pos.x-CURSOR_SPACING, cursor_pos.y+(_pick_pos*CURSOR_SPACING)));
        }
        if (InputController::get_instance()->p1_input[BTN_UP]) {
                SoundView::get_instance()->play_sfx(SFX_CURSOR);
                ImageView::get_instance()->eraseCursor(st_position(cursor_pos.x-CURSOR_SPACING, cursor_pos.y+(_pick_pos*CURSOR_SPACING)));
                _pick_pos--;
                if (_pick_pos < 0) {
                    _pick_pos = _keys_list.size();
                }
                ImageView::get_instance()->drawCursor(st_position(cursor_pos.x-CURSOR_SPACING, cursor_pos.y+(_pick_pos*CURSOR_SPACING)));
        }
        if (InputController::get_instance()->p1_input[BTN_QUIT]) {
            finished = true;
        }
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(10);
        Draw::get_instance()->update_screen();
    }
}

void key_map::redraw_line(short line) const
{
    std::cout << "******* key_map::redraw_line - line: " << line << std::endl;
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    ImageView::get_instance()->clearScreenArea(config_text_pos.x+70-1, config_text_pos.y-1 + line*CURSOR_SPACING, 110,  CURSOR_SPACING+1, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);

    std::stringstream key_n_ss;
    ///@TODO - key_n_ss << key_config[line].key_number;

    TextView::get_instance()->renderText(config_text_pos.x + 140, config_text_pos.y + line*CURSOR_SPACING, key_n_ss.str()); //input code (number)
}




Sint8 key_map::draw_config_input(short current_selection) const
{
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    ImageView::get_instance()->show_config_bg();
    std::vector<std::string> options;
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_buttons, SharedData::get_instance()->current_language));
    if (SharedData::get_instance()->game_config.input_mode == INPUT_MODE_DIGITAL) {
        options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_directional, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_directional_digital, SharedData::get_instance()->current_language));
    } else {
        options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_directional, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_directional_analog, SharedData::get_instance()->current_language));
    }

    std::string selected_joystick_str = std::to_string(SharedData::get_instance()->game_config.selected_input_device + 1);
    std::string max_joystick_str = std::to_string(InputController::get_instance()->get_joysticks_number());

    // turbo mode //
    std::string turbo_mode_string = strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_turbo_mode, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_off, SharedData::get_instance()->current_language);
    if (SharedData::get_instance()->game_config.turbo_mode == true) {
        turbo_mode_string = strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_turbo_mode, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_on, SharedData::get_instance()->current_language);
    }
    options.push_back(turbo_mode_string);
    // auto-charge mode //
    std::string autocharge_mode_string = strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_autocharge_mode, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_off, SharedData::get_instance()->current_language);
    if (SharedData::get_instance()->game_config.auto_charge_mode == true) {
        autocharge_mode_string = strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_autocharge_mode, SharedData::get_instance()->current_language) + std::string(": ") + strings_map::get_instance()->get_ingame_string(strings_ingame_config_on, SharedData::get_instance()->current_language);
    }
    options.push_back(autocharge_mode_string);

    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_input_selected_joystick, SharedData::get_instance()->current_language) + std::string(": ") + selected_joystick_str + std::string("/") + max_joystick_str);

    Sint8 selected_option = -1;
    option_picker main_config_picker(false, st_position(CONFIG_MENU_LEFT_SPACING, CONFIG_MENU_TOP_SPACING), options, true);
    selected_option = main_config_picker.pick(current_selection+1);
    return selected_option;
}

Sint8 key_map::pick_joystick()
{
    ImageView::get_instance()->show_config_bg();
    std::vector<std::string> options;

    for (int i=0; i<InputController::get_instance()->get_joysticks_number(); i++) {
        std::string prefix = "";
        if (i < 10) {
            prefix = "0";
        }
        std::string joynumber = prefix + std::to_string(i+1);
        options.push_back(joynumber + std::string(": "));
    }

    Sint8 selected_option = -1;
    option_picker main_config_picker(false, st_position(CONFIG_MENU_LEFT_SPACING, CONFIG_MENU_TOP_SPACING), options, true);
    selected_option = main_config_picker.pick();
    return selected_option;
}

std::string key_map::build_button_config_line(std::string prefix, std::string sufix)
{
    int total_space = 24;
    prefix = strings_map::get_instance()->get_ingame_string(strings_ingame_config_set, SharedData::get_instance()->current_language) + std::string(" ") + prefix;
    int spaces_to_add = total_space - prefix.length() - sufix.length();
    std::string spaces = "";
    for (int i=0; i<spaces_to_add; i++) {
        spaces += " ";
    }
    std::string res = prefix + spaces + sufix;
    return res;
}

Sint8 key_map::draw_config_buttons(st_game_config& game_config_copy)
{
    st_position config_text_pos;
    config_text_pos.x = ImageView::get_instance()->get_config_menu_pos().x + 24;
    config_text_pos.y = ImageView::get_instance()->get_config_menu_pos().y + 40;
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);

    ImageView::get_instance()->show_config_bg();
    std::vector<std::string> options;

    std::vector<std::string> btn_codes(BTN_COUNT);
// android Pelya's SDL use keyboard events for joystick buttons
#ifdef ANDROID
    btn_codes[BTN_JUMP] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_JUMP]) + "]";
    btn_codes[BTN_ATTACK] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_ATTACK]) + "]";
    btn_codes[BTN_DASH] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_DASH]) + "]";
    btn_codes[BTN_SHIELD] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_SHIELD]) + "]";
    btn_codes[BTN_L] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_L]) + "]";
    btn_codes[BTN_R] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_R]) + "]";
    btn_codes[BTN_START] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_START]) + "]";
    btn_codes[BTN_UP] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_UP]) + "]";
    btn_codes[BTN_DOWN] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_DOWN]) + "]";
    btn_codes[BTN_LEFT] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_LEFT]) + "]";
    btn_codes[BTN_RIGHT] = "[" + InputController::get_instance()->get_key_name(SharedData::get_instance()->game_config_copy.keys_codes[BTN_RIGHT]) + "]";
#elif PC
    btn_codes[BTN_JUMP] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_JUMP]) + "/" + std::to_string(game_config_copy.button_codes[BTN_JUMP].value) + "]";
    btn_codes[BTN_ATTACK] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_ATTACK]) + "/" + std::to_string(game_config_copy.button_codes[BTN_ATTACK].value) + "]";
    btn_codes[BTN_DASH] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_DASH]) + "/" + std::to_string(game_config_copy.button_codes[BTN_DASH].value) + "]";
    btn_codes[BTN_ITEM] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_ITEM]) + "/" + std::to_string(game_config_copy.button_codes[BTN_ITEM].value) + "]";
    btn_codes[BTN_L] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_L]) + "/" + std::to_string(game_config_copy.button_codes[BTN_L].value) + "]";
    btn_codes[BTN_R] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_R]) + "/" + std::to_string(game_config_copy.button_codes[BTN_R].value) + "]";
    btn_codes[BTN_START] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_START]) + "/" + std::to_string(game_config_copy.button_codes[BTN_START].value) + "]";
    btn_codes[BTN_UP] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_UP]) + "/" + std::to_string(game_config_copy.button_codes[BTN_UP].value) + "]";
    btn_codes[BTN_DOWN] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_DOWN]) + "/" + std::to_string(game_config_copy.button_codes[BTN_DOWN].value) + "]";
    btn_codes[BTN_LEFT] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_LEFT]) + "/" + std::to_string(game_config_copy.button_codes[BTN_LEFT].value) + "]";
    btn_codes[BTN_RIGHT] = "[" + InputController::get_instance()->get_key_name(game_config_copy.keys_codes[BTN_RIGHT]) + "/" + std::to_string(game_config_copy.button_codes[BTN_RIGHT].value) + "]";
#else
    btn_codes[BTN_JUMP] = "[" + std::to_string(game_config_copy.button_codes[BTN_JUMP].value) + "]";
    btn_codes[BTN_ATTACK] = "[" + std::to_string(game_config_copy.button_codes[BTN_ATTACK].value) + "]";
    btn_codes[BTN_DASH] = "[" + std::to_string(game_config_copy.button_codes[BTN_DASH].value) + "]";
    btn_codes[BTN_SHIELD] = "[" + std::to_string(game_config_copy.button_codes[BTN_SHIELD].value) + "]";
    btn_codes[BTN_L] = "[" + std::to_string(game_config_copy.button_codes[BTN_L].value) + "]";
    btn_codes[BTN_R] = "[" + std::to_string(game_config_copy.button_codes[BTN_R].value) + "]";
    btn_codes[BTN_START] = "[" + std::to_string(game_config_copy.button_codes[BTN_START].value) + "]";
    btn_codes[BTN_UP] = "[" + std::to_string(game_config_copy.button_codes[BTN_UP].value) + "]";
    btn_codes[BTN_DOWN] = "[" + std::to_string(game_config_copy.button_codes[BTN_DOWN].value) + "]";
    btn_codes[BTN_LEFT] = "[" + std::to_string(game_config_copy.button_codes[BTN_LEFT].value) + "]";
    btn_codes[BTN_RIGHT] = "[" + std::to_string(game_config_copy.button_codes[BTN_RIGHT].value) + "]";
#endif
    options.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_reset, SharedData::get_instance()->current_language));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_jump, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_JUMP])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_fire, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_ATTACK])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_dash, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_DASH])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_shield, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_ITEM])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_keyl, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_L])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_keyr, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_R])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_start, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_START])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_up, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_UP])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_down, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_DOWN])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_left, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_LEFT])));
    options.push_back(build_button_config_line(strings_map::get_instance()->get_ingame_string(strings_ingame_config_key_right, SharedData::get_instance()->current_language), std::string(btn_codes[BTN_RIGHT])));

    Sint8 selected_option = 0;
    option_picker main_config_picker(false, config_text_pos, options, true);
    selected_option = main_config_picker.pick();
    return selected_option;
}





void key_map::config_input()
{
    int selected_option = 0;

    while (selected_option != -1) {
        selected_option = draw_config_input(selected_option);

        if (selected_option == 0) {
            config_buttons();
        } else if (selected_option == 1) {
            if (SharedData::get_instance()->game_config.input_mode == INPUT_MODE_DIGITAL) {
                SharedData::get_instance()->game_config.input_mode = INPUT_MODE_ANALOG;
            } else {
                SharedData::get_instance()->game_config.input_mode = INPUT_MODE_DIGITAL;
            }
        } else if (selected_option == 2) {
            SharedData::get_instance()->game_config.turbo_mode = !SharedData::get_instance()->game_config.turbo_mode;
            if (SharedData::get_instance()->game_config.auto_charge_mode && SharedData::get_instance()->game_config.turbo_mode) {
                SharedData::get_instance()->game_config.auto_charge_mode = false;
            }
        } else if (selected_option == 3) {
            SharedData::get_instance()->game_config.auto_charge_mode = !SharedData::get_instance()->game_config.auto_charge_mode;
            if (SharedData::get_instance()->game_config.auto_charge_mode && SharedData::get_instance()->game_config.turbo_mode) {
                SharedData::get_instance()->game_config.turbo_mode = false;
            }
        } else if (selected_option == 4) {
            int res_pick = pick_joystick();
            if (res_pick != -1) {
                SharedData::get_instance()->game_config.selected_input_device = res_pick;
            }
        }
    }
}

void key_map::config_buttons()
{
    st_game_config game_config_copy = SharedData::get_instance()->game_config;
    int selected_option = 0;
    st_position menu_pos(ImageView::get_instance()->get_config_menu_pos().x + 24, ImageView::get_instance()->get_config_menu_pos().y + 40);

    while (selected_option != -1) {
        selected_option = draw_config_buttons(game_config_copy);

        INPUT_COMMANDS selected_key = BTN_JUMP;
        if (selected_option == 0) {
            SharedData::get_instance()->game_config.set_default_keys();
            return;
        } else if (selected_option != -1) {
            if (selected_option == 1) {
                selected_key = BTN_JUMP;
            } else if (selected_option == 2) {
                selected_key = BTN_ATTACK;
            } else if (selected_option == 3) {
                selected_key = BTN_DASH;
            } else if (selected_option == 4) {
                selected_key = BTN_ITEM;
            } else if (selected_option == 5) {
                selected_key = BTN_L;
            } else if (selected_option == 6) {
                selected_key = BTN_R;
            } else if (selected_option == 7) {
                selected_key = BTN_START;
            } else if (selected_option == 8) {
                selected_key = BTN_UP;
            } else if (selected_option == 9) {
                selected_key = BTN_DOWN;
            } else if (selected_option == 10) {
                selected_key = BTN_LEFT;
            } else if (selected_option == 11) {
                selected_key = BTN_RIGHT;
            }

            ImageView::get_instance()->clearScreenArea(menu_pos.x, menu_pos.y, 195,  180, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
            TextView::get_instance()->renderText(menu_pos.x, menu_pos.y, strings_map::get_instance()->get_ingame_string(strings_ingame_pressanykey, SharedData::get_instance()->current_language));
            Draw::get_instance()->update_screen();
            InputController::get_instance()->clean();
            TimerView::get_instance()->delay(200);
            bool is_joystick = InputController::get_instance()->pick_key_or_button(game_config_copy, selected_key);

            check_key_duplicates(game_config_copy, selected_key, is_joystick);
        }

        // check if jump/attack/start are set, can't leave if one of those is unset
        if (selected_option == -1) {

#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::config_buttons::FINISHED @1 ###");
#else
std::cout << "### INPUT::config_buttons::FINISHED @1 ###" << std::endl;
#endif
            TimerView::get_instance()->delay(200);



            if (is_key_set(BTN_JUMP, game_config_copy) == false) {
                std::string line = strings_map::get_instance()->get_ingame_string(strings_config_keys_unet, SharedData::get_instance()->current_language) + std::string(" JUMP KEY");
                TextView::get_instance()->renderText(menu_pos.x, RES_H-40, st_color(180, 0, 0), false, line);
                Draw::get_instance()->update_screen();
                TimerView::get_instance()->delay(1000);
                InputController::get_instance()->clean_all();
                selected_option = 0;
            } else if (is_key_set(BTN_ATTACK, game_config_copy) == false) {
                std::string line = strings_map::get_instance()->get_ingame_string(strings_config_keys_unet, SharedData::get_instance()->current_language) + std::string(" ATTACK KEY");
                TextView::get_instance()->renderText(menu_pos.x, RES_H-40, st_color(180, 0, 0), false, line);
                Draw::get_instance()->update_screen();
                TimerView::get_instance()->delay(1000);
                InputController::get_instance()->clean_all();
                selected_option = 0;
            } else if (is_key_set(BTN_START, game_config_copy) == false) {
                std::string line = strings_map::get_instance()->get_ingame_string(strings_config_keys_unet, SharedData::get_instance()->current_language) + std::string(" START KEY");
                TextView::get_instance()->renderText(menu_pos.x, RES_H-40, st_color(180, 0, 0), false, line);
                TimerView::get_instance()->delay(1000);
                Draw::get_instance()->update_screen();
                InputController::get_instance()->clean_all();
                selected_option = 0;
            }
        }

#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::config_buttons::FINISHED @2 ###");
#else
std::cout << "### INPUT::config_buttons::FINISHED @2 ###" << std::endl;
#endif
TimerView::get_instance()->delay(200);


    }
    // apply changes to game-config
    apply_key_codes_changes(game_config_copy);

}

bool key_map::is_key_set(INPUT_COMMANDS key, st_game_config game_config_copy)
{

#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::is_key_set @1 ###");
#else
std::cout << "### INPUT::is_key_set @1 ###" << std::endl;
#endif
    TimerView::get_instance()->delay(200);

    bool have_joystick = false;
    if (InputController::get_instance()->get_joysticks_number() > 0 && game_config_copy.selected_input_device < InputController::get_instance()->get_joysticks_number()) {
#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::is_key_set @2 ###");
#else
std::cout << "### INPUT::is_key_set @2 ###" << std::endl;
#endif
        TimerView::get_instance()->delay(200);
        have_joystick = true;
    }
    if (game_config_copy.keys_codes[key] == -1 && (have_joystick == false || (have_joystick == true && game_config_copy.button_codes[key].value == -1))) {
#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::is_key_set @3 ###");
#else
std::cout << "### INPUT::is_key_set @3 ###" << std::endl;
#endif
        TimerView::get_instance()->delay(200);
        return false;
    }
#ifdef ANDROID
__android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::is_key_set @4 ###");
#else
std::cout << "### INPUT::is_key_set @4 ###" << std::endl;
#endif
    TimerView::get_instance()->delay(200);
    return true;
}

// if any key is duplicated in the config, reset it to default value
void key_map::check_key_duplicates(st_game_config& game_config_copy, Uint8 set_key, bool is_joystick)
{
    int default_keys_codes[BTN_COUNT];
    st_input_button_config default_button_codes[BTN_COUNT];

    //std::cout << "KEYMAP::CHECK_DUPLICATES - is_joystick: " << is_joystick << ", set_key: " << (int)set_key << std::endl;

    game_config_copy.get_default_keys(default_keys_codes);
    game_config_copy.get_default_buttons(default_button_codes);

    for (int i=0; i<BTN_COUNT; i++) {
        if (is_joystick == false) {
            if (i != set_key && game_config_copy.keys_codes[i] == game_config_copy.keys_codes[set_key]) { // found duplicate
                game_config_copy.keys_codes[i] = -1; // disable key
                #ifdef ANDROID
                __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::check_key_duplicates RESET[#1]BUTTON[%d] ###", i);
                #else
                std::cout << "### INPUT::check_key_duplicates RESET[KEY-CODE][" << i << "]" << std::endl;
                #endif
            }
        } else {
            if (i != set_key && game_config_copy.button_codes[i].type == game_config_copy.button_codes[set_key].type && game_config_copy.button_codes[i].value == game_config_copy.button_codes[set_key].value && game_config_copy.button_codes[i].axis_type == game_config_copy.button_codes[set_key].axis_type) { // found duplicate
                #ifdef ANDROID
                __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### INPUT::check_key_duplicates RESET[#2]BUTTON[%d] ###", i);
                #else
                //std::cout << "### INPUT::check_key_duplicates RESET[JOY-BUTTON][" << i << "]" << std::endl;
                #endif
                game_config_copy.button_codes[i].value = -1; // disable key
                game_config_copy.button_codes[i].type = JOYSTICK_INPUT_TYPE_NONE;
                game_config_copy.button_codes[i].axis_type = 0;
            }
        }
    }
}

void key_map::apply_key_codes_changes(st_game_config game_config_copy)
{
    #ifdef ANDROID
    __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "#####################################################");
    #else
    //std::cout << "######################## OLD #########################" << std::endl;
    #endif
    for (int i=0; i<BTN_COUNT; i++) {
        #ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### KEYMAP::apply_key_codes_changes button_codes[%d].value[%d].type[%d].axis_type[%d] ###", i, game_config.button_codes[i].value, game_config.button_codes[i].type, game_config.button_codes[i].axis_type);
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### KEYMAP::apply_key_codes_changes keys_codes[%d].value[%d] ###", i, game_config.keys_codes[i]);
        #else
        std::cout << "### KEYMAP::apply_key_codes_changes button_codes[old][" << i << "].value[" << SharedData::get_instance()->game_config.button_codes[i].value << "].type[" << SharedData::get_instance()->game_config.button_codes[i].type << "].axis_type[" << SharedData::get_instance()->game_config.button_codes[i].axis_type << "] ###" << std::endl;
        #endif
    }
    #ifdef ANDROID
    __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "#####################################################");
    #else
    //std::cout << "######################## NEW #########################" << std::endl;
    #endif
    for (int i=0; i<BTN_COUNT; i++) {
        #ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### KEYMAP::apply_key_codes_changes button_codes[%d].value[%d].type[%d].axis_type[%d] ###", i, game_config_copy.button_codes[i].value, game_config_copy.button_codes[i].type, game_config_copy.button_codes[i].axis_type);
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### KEYMAP::apply_key_codes_changes keys_codes[%d].value[%d] ###", i, game_config.keys_codes[i]);
        #else
        std::cout << "### KEYMAP::apply_key_codes_changes button_codes[new][" << i << "].value[" << game_config_copy.button_codes[i].value << "].type[" << game_config_copy.button_codes[i].type << "].axis_type[" << game_config_copy.button_codes[i].axis_type << "] ###" << std::endl;
        #endif
    }


    for (int i=0; i<BTN_COUNT; i++) {
        SharedData::get_instance()->game_config.keys_codes[i] = game_config_copy.keys_codes[i];
        #ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT###", "### KEYMAP::apply_key_codes_changes old_config.btn[%d][%d], new_config_btn[%d]", i, game_config.button_codes[i].value, game_config_copy.button_codes[i].value);
        #else
        //std::cout << "old_config.btn[" << i << "][" << game_config.button_codes[i].value << "], new_config_btn[" << i << "][" << game_config_copy.button_codes[i].value << "]" << std::endl;
        #endif
        SharedData::get_instance()->game_config.button_codes[i].value = game_config_copy.button_codes[i].value;
        SharedData::get_instance()->game_config.button_codes[i].type = game_config_copy.button_codes[i].type;
        SharedData::get_instance()->game_config.button_codes[i].axis_type = game_config_copy.button_codes[i].axis_type;
    }
}





