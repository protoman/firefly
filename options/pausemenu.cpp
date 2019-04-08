#include "pausemenu.h"

#include "controller/inputcontroller.h"
#include "view/timerview.h"
#include "view/imageview.h"
#include "view/draw.h"

#define GRID_SIZE 32

PauseMenu* PauseMenu::_instance = nullptr;

PauseMenu::PauseMenu()
{

}

PauseMenu *PauseMenu::get_instance()
{
    if (!_instance) {
        _instance = new PauseMenu();
    }
    return _instance;
}

bool PauseMenu::execute()
{
    if (InputController::get_instance()->p1_input[BTN_START] == 1) {
        is_paused = !is_paused;
        TimerView::get_instance()->delay(200);
    }
    if (is_paused == true) {
        draw::get_instance()->draw_in_game_menu_bg();
    }
    return is_paused;
}
