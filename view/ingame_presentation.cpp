#include "ingame_presentation.h"


#include "draw.h"
#include "view/textview.h"

InGamePresentation* InGamePresentation::_instance = nullptr;

InGamePresentation::InGamePresentation()
{

}

InGamePresentation *InGamePresentation::get_instance()
{
    if (!_instance) {
        _instance = new InGamePresentation();
    }
    return _instance;
}

void InGamePresentation::execute_ingame_presentation()
{
    if (must_show_ready == true) {
        show_ready_presentation();
    }
}

void InGamePresentation::start_show_ready()
{
    must_show_ready = true;
    show_ready_state = 0;
    ready_text_stage_number_size = TextView::get_instance()->get_text_size("STAGE-0");
    ready_text_stage_name_size = TextView::get_instance()->get_text_size("\"New Dimension is a... Swamp?!\"");

    limit_x = RES_W/2 - ready_text_stage_name_size.width/2;
    limit_y = AREA_H/2 - ready_text_stage_number_size.height/2;

    speed_x = DEFAULT_SPEED;
    speed_y = (DEFAULT_SPEED*limit_y) / limit_x;
}

void InGamePresentation::show_ready_presentation()
{
    TextView::get_instance()->renderText(limit_x, pos_y, st_color(255, 255, 255), false, "STAGE-0");
    TextView::get_instance()->renderText(pos_x, AREA_H/2+20, st_color(255, 255, 255), false, "\"New Dimension is a... Swamp?!\"");
    if (show_ready_state < limit_x) {
        pos_x += speed_x;
        pos_y += speed_y;
    } else if (show_ready_state > RES_W) {
        pos_x -= speed_x;
        pos_y -= speed_y;
    }

    show_ready_state += DEFAULT_SPEED;
    if (show_ready_state > RES_W*1.5) { // @TDO: add the text width/height
        must_show_ready = false;
    }
}

bool InGamePresentation::is_showing_ready()
{
    return must_show_ready;
}

std::vector<st_dialog> InGamePresentation::get_game_dialog(int number)
{
    std::vector<st_dialog> res;
    std::vector<std::string> dialog_str_list = i18nText::get_instance()->get_dialog(GAME_DIALOG_INTRO_STAGE_SWAMP_ARRIVAL);
    for (int i=0; i<dialog_str_list.size(); i++) {
        st_dialog item;
        item.msgs = i18nText::get_instance()->split(dialog_str_list.at(i), GAME_TEXT_LINE_SEPARATOR_CHAR);
        res.push_back(item);
    }
    return res;
}
