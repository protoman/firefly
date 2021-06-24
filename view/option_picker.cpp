#include "option_picker.h"


#include "strings_map.h"
#include "data/shareddata.h"
#include "controller/inputcontroller.h"
#include "view/timerview.h"
#include "view/imageview.h"
#include "view/soundview.h"
#include "view/textview.h"
#include "draw.h"

option_picker::option_picker(bool draw_border, st_position pos, std::vector<st_menu_option> options, bool show_return) : _pick_pos(0)
{

    //std::cout << "#2 option_picker -  pos.x: " << pos.x << ", pos.y: " << pos.y << std::endl;

    _position = pos;
    _draw_border = draw_border;

    if (_draw_border) {
        _position.x += 12 + CURSOR_SPACING;
        _position.y += 12;
    }
    _items = options;
    _show_return = show_return;
    if (_show_return == true) {

        _items.insert(_items.begin(), st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_return, SharedData::get_instance()->current_language)));
    }

    _pick_pos = 0;

    //std::cout << "#3 option_picker -  pos.x: " << _position.x << ", pos.y: " << _position.y << std::endl;

    check_input_reset_command = false;
    check_input_cheat_command = false;

    draw();
}

option_picker::option_picker(bool draw_border, st_position pos, std::vector<std::string> options, bool show_return)
{
    //std::cout << "#1 option_picker -  pos.x: " << pos.x << ", pos.y: " << pos.y << std::endl;
    std::vector<st_menu_option> option_list;
    for (int i=0; i<options.size(); i++) {
        option_list.push_back(st_menu_option(options.at(i)));
    }

    _position = pos;
    _draw_border = draw_border;

    if (_draw_border) {
        _position.x += 12 + CURSOR_SPACING;
        _position.y += 12;
    }
    _items = option_list;
    _show_return = show_return;
    if (_show_return == true) {
        _items.insert(_items.begin(), st_menu_option(strings_map::get_instance()->get_ingame_string(strings_config_return, SharedData::get_instance()->current_language)));
    }

    check_input_reset_command = false;
    check_input_cheat_command = false;


    draw();

}

void option_picker::change_option_label(int n, std::string label)
{
    if (n >= 0 && n < _items.size()) {
        _items.at(n).text = label;
    }
}

Sint8 option_picker::pick(int initial_pick_pos)
{
    bool finished = false;
    InputController::get_instance()->clean_all();
    TimerView::get_instance()->delay(100);
    _pick_pos = initial_pick_pos;

    if (_pick_pos < 0 || _pick_pos >= _items.size()) {
        _pick_pos = 0;
    }

	ImageView::get_instance()->drawCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));

    while (finished == false) {
        InputController::get_instance()->read_input(check_input_reset_command, check_input_cheat_command);
        if (check_input_reset_command == true && InputController::get_instance()->is_check_input_reset_command_activated()) {
            std::cout << "RESET ACTIVE!!" << std::endl;
            show_reset_config_dialog();
        } else if (check_input_cheat_command == true && InputController::get_instance()->is_check_input_cheat_command_activated()) {
            InputController::get_instance()->reset_cheat_input();
            return MAIN_MENU_CHEAT_RETURN;
        }

        if (InputController::get_instance()->p1_input[BTN_START] || InputController::get_instance()->p1_input[BTN_JUMP]) {
            if (_items.at(_pick_pos).disabled == true) {
                SoundView::get_instance()->play_sfx(SFX_NPC_HIT);
            } else {
                //std::cout << "option_picker::option_picker::END #1" << std::endl;
                ImageView::get_instance()->eraseCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
                draw::get_instance()->update_screen();
                if (_show_return == true) {
                    _pick_pos--;
                }
                return _pick_pos;
            }
        }
        if (InputController::get_instance()->p1_input[BTN_DOWN]) {
            //std::cout << "option_picker::option_picker::DOWN" << std::endl;
            SoundView::get_instance()->play_sfx(SFX_CURSOR);
            ImageView::get_instance()->eraseCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
            _pick_pos++;
            if (_pick_pos >= (short)_items.size()) {
                _pick_pos = 0;
            }
            ImageView::get_instance()->drawCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
        }
        if (InputController::get_instance()->p1_input[BTN_UP]) {
            //std::cout << "option_picker::option_picker::UP" << std::endl;
            SoundView::get_instance()->play_sfx(SFX_CURSOR);
            ImageView::get_instance()->eraseCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
            if (_pick_pos == 0) {
                _pick_pos = _items.size()-1;
            } else {
                _pick_pos--;
            }
            ImageView::get_instance()->drawCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
        }
        if (InputController::get_instance()->p1_input[BTN_QUIT] || InputController::get_instance()->p1_input[BTN_ATTACK]) {
            ImageView::get_instance()->eraseCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
            //std::cout << "option_picker::option_picker::END #2" << std::endl;
            return -1;
        }
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(10);
        draw::get_instance()->update_screen();
    }
	ImageView::get_instance()->eraseCursor(st_position(_position.x-CURSOR_SPACING, _position.y+(_pick_pos*CURSOR_SPACING)));
    TimerView::get_instance()->delay(10);
    draw::get_instance()->update_screen();

    if (_show_return == true) {
        _pick_pos--;
    }
    // erase cursor
    return _pick_pos;
}

void option_picker::enable_check_input_reset_command()
{
    check_input_reset_command = true;
}

void option_picker::enable_check_input_cheat_command()
{
    check_input_cheat_command = true;
}

void option_picker::show_reset_config_dialog()
{
    st_imageData screen_copy;
    screen_copy = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
    ImageView::get_instance()->copyScreenAreaToImage(0, 0, RES_W, RES_H, 0, 0, screen_copy);
    wait_release_reset_config();
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 40, 0, 0);
    TextView::get_instance()->renderText(20, 20, st_color(250, 250, 250), false, "NOW PRESS TWO BUTTONS TOGETHER");
    TextView::get_instance()->renderText(20, 32, st_color(250, 250, 250), false, "AND HOLD IT FOR 5 SECONDS");
    TextView::get_instance()->renderText(20, 44, st_color(250, 250, 250), false, "TO RESET CONFIGURATION.");
    TextView::get_instance()->renderText(20, 60, st_color(250, 250, 250), false, "OR WAIT 10 SECONDS TO");
    TextView::get_instance()->renderText(20, 72, st_color(250, 250, 250), false, "RETURN.");
    ImageView::get_instance()->updateRender();
    long init_timer = TimerView::get_instance()->getTimer();
    while (InputController::get_instance()->is_check_input_reset_command_activated() == false) {
        InputController::get_instance()->read_input(true, false);
        TimerView::get_instance()->delay(1);
        if (init_timer+10000 < TimerView::get_instance()->getTimer()) {
            break;
        } else if (InputController::get_instance()->is_check_input_reset_command_activated() == true) {
            std::cout << "RESET CONFIG!!!" << std::endl;
            // reset the configuration file //
            SharedData::get_instance()->game_config.reset();
            fio.save_config(SharedData::get_instance()->game_config);
            break;
        }
    }

    wait_release_reset_config();
    ImageView::get_instance()->renderImageAt(0, 0, screen_copy);
    ImageView::get_instance()->updateRender();
}

void option_picker::wait_release_reset_config()
{
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 40, 0, 0);
    TextView::get_instance()->renderText(20, 20, st_color(250, 250, 250), false, "PLEASE RELEASE BUTTONS");
    ImageView::get_instance()->updateRender();
    while (InputController::get_instance()->is_check_input_reset_command_activated() == true) {
        InputController::get_instance()->read_input(true, false);
        TimerView::get_instance()->delay(1);
    }
}

void option_picker::add_option_item(st_menu_option item)
{
    _items.push_back(item);
}



void option_picker::draw()
{

    text_max_len = 0;
    for (int i=0; i<_items.size(); i++) {
        std::string line = _items.at(i).text;
        int line_len = line.length();
        //std::cout << "line_len[" << i << "]: " << line_len << std::endl;
        if (line_len > text_max_len) {
            text_max_len = line_len;
        }
    }

    //std::cout << "OPTION_PICKER::text_max_len: " << text_max_len << std::endl;

    ImageView::get_instance()->clearScreenArea(_position.x, _position.y, text_max_len*8, _items.size()*12, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
	for (unsigned int i=0; i<_items.size(); i++) {
        st_menu_option menu_item = _items.at(i);

        //std::cout << "menu_item: " << menu_item.text << std::endl;
        if (menu_item.disabled == true) {
            TextView::get_instance()->renderText(_position.x, _position.y + (12 * i), st_color(100, 100, 100), false, menu_item.text);
        } else {
            TextView::get_instance()->renderText(_position.x, _position.y + (12 * i), st_color(250, 250, 250), false, menu_item.text);
        }
	}
}

