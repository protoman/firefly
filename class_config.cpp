#include "class_config.h"

#include "data/shareddata.h"

#include "controller/inputcontroller.h"

#include "view/imageview.h"
#include "view/soundview.h"
#include "view/textview.h"
#include "view/timerview.h"

class_config::class_config() : ingame_menu_active(false)
{
	ingame_menu_pos.x = 0;
    ingame_menu_pos.y = 0;
    player_ref = nullptr;

}



void class_config::set_player_ref(classPlayer* set_player_ref)
{
	player_ref = set_player_ref;
}





void class_config::draw_ingame_menu()
{
    /*
    ingame_menu_pos = convert_stage_n_to_menu_pos(player_ref->get_selected_weapon());
    ImageView::get_instance()->draw_weapon_menu_bg(player_ref->get_current_hp(), player_ref->get_char_frame(ANIM_DIRECTION_RIGHT, ANIM_TYPE_ATTACK, 0), player_ref->get_max_hp());
    ImageView::get_instance()->draw_weapon_icon(convert_menu_pos_to_weapon_n(ingame_menu_pos), ingame_menu_pos, true);
    ImageView::get_instance()->draw_weapon_cursor(ingame_menu_pos, player_ref->get_weapon_value(convert_menu_pos_to_weapon_n(ingame_menu_pos)), player_ref->get_number(), player_ref->get_max_hp());
    */
    TextView::get_instance()->renderText(0, RES_H/2-8, st_color(255, 255, 255), true, "PAUSED (@TODO)");
}




void class_config::disable_ingame_menu()
{
    ingame_menu_active = false;
}


