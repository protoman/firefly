#include <cstring>
#include "dialogs.h"

#include "controller/inputcontroller.h"

#include "view/option_picker.h"
#include "view/imageview.h"
#include "view/textview.h"
#include "strings_map.h"

#include "GameManager.h"

dialogs::dialogs() : is_showing_dialog_bg(false)
{
    /// @TODO - temporary configurations should not be placed in st_config
    //if (game_config.two_players == 2 || game_config.selected_player == 1) {
}




void dialogs::show_dialog(std::string face_file, bool top_side, std::string lines[FS_DIALOG_LINES], bool show_btn=true)
{
	std::string temp_text;
    std::string temp_char;

    if (lines[0].size() < 1) {
        return;
    }

    GameManager::get_instance()->game_pause();

    draw_dialog_bg();
    draw::get_instance()->update_screen();
    st_position dialog_pos = draw::get_instance()->get_dialog_pos();
	ImageView::get_instance()->place_face(face_file, st_position(dialog_pos.x+16, dialog_pos.y+16));
    draw::get_instance()->update_screen();

	/// @TODO: usar show_config_bg e hide_config_bg da graphLib - modificar para aceitar centered (que é o atual) ou top ou bottom
    for (int i=0; i<FS_DIALOG_LINES; i++) {
        std::string line = lines[i];
        TextView::get_instance()->renderText(dialog_pos.x+52, i*11+(dialog_pos.y+16), st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, line);


        draw::get_instance()->update_screen();
        TimerView::get_instance()->delay(50);
    }

    if (show_btn == true) {
        draw::get_instance()->show_dialog_button(1);
        draw::get_instance()->update_screen();
    }


    InputController::get_instance()->clean_confirm_button();
    InputController::get_instance()->wait_keypress();
    GameManager::get_instance()->game_unpause();

}

bool dialogs::show_leave_game_dialog()
{
    std::string lines[3];
    lines[0] = strings_map::get_instance()->get_ingame_string(strings_ingame_quitgame, SharedData::get_instance()->current_language);
    return show_yes_no_dialog(lines);
}

bool dialogs::show_yes_no_dialog(std::string lines[3])
{
    bool res = false;
    bool repeat_menu = true;
    int picked_n = -1;

    Uint8 gfx = draw::get_instance()->get_gfx();
    GameManager::get_instance()->game_pause();

    InputController::get_instance()->clean();

    st_imageData bgCopy;
    bgCopy = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
    ImageView::get_instance()->copyScreenAreaToImage(0, 0, RES_W, RES_H, 0, 0, bgCopy);

    draw::get_instance()->show_dialog(0);
    st_position dialog_pos = draw::get_instance()->get_dialog_pos();

    for (int i=0; i<3; i++) {
        TextView::get_instance()->renderText(dialog_pos.x+30, dialog_pos.y+16+15*i, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, lines[i]);
    }

    std::vector<std::string> item_list;

    item_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_yes, SharedData::get_instance()->current_language));
    item_list.push_back(strings_map::get_instance()->get_ingame_string(strings_ingame_no, SharedData::get_instance()->current_language));
    option_picker main_picker(false, st_position(dialog_pos.x+40, dialog_pos.y+16+11), item_list, false);
    draw::get_instance()->update_screen();
    while (repeat_menu == true) {
        picked_n = main_picker.pick();
        //std::cout << "picked_n: " << picked_n << std::endl;
        if (picked_n == 0) {
            res = true;
            repeat_menu = false;
        } else if (picked_n == 1) {
            res = false;
            repeat_menu = false;
        } else {
            main_picker.draw();
        }
    }
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(200);
    ImageView::get_instance()->renderImageAt(0, 0, bgCopy);
    draw::get_instance()->update_screen();
    GameManager::get_instance()->game_unpause();

    return res;
}

void dialogs::show_timed_dialog(std::string face_file, bool is_left, std::string lines[], short delay, bool show_btn=true)
{
	std::string temp_text;
	char temp_char;

    GameManager::get_instance()->game_pause();

    draw::get_instance()->update_screen();
    st_position dialog_pos = draw::get_instance()->get_dialog_pos();
    ImageView::get_instance()->place_face(face_file, st_position(dialog_pos.x+16, dialog_pos.y+16));
    draw::get_instance()->update_screen();

	/// @TODO: usar show_config_bg e hide_config_bg da graphLib - modificar para aceitar centered (que é o atual) ou top ou bottom
	for (int i=0; i<3; i++) {
        for (unsigned int j=0; j<lines[i].size(); j++) {
			temp_char = lines[i].at(j);
			temp_text = "";
			temp_text += temp_char;

            TextView::get_instance()->renderText(j*9+(dialog_pos.x+52), i*11+(dialog_pos.y+16), st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, temp_text);
            draw::get_instance()->update_screen();
            TimerView::get_instance()->delay(15);
		}
	}

    if (show_btn == true) {
        draw::get_instance()->show_dialog_button(1);
        draw::get_instance()->update_screen();
    }


    TimerView::get_instance()->delay(delay);
    GameManager::get_instance()->game_unpause();
}

void dialogs::show_centered_dialog(std::vector<std::string> lines)
{
    /// TODO ///
}

void dialogs::show_boss_dialog(int stage_n)
{
    /// TODO ///
}





void dialogs::draw_dialog_bg()
{
	if (is_showing_dialog_bg == true) {
		return;
	}
    draw::get_instance()->show_dialog(1);
}


