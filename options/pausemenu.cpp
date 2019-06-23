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

bool PauseMenu::execute_pause_menu()
{
    if (InputController::get_instance()->p1_input[BTN_START] == 1) {
        is_paused = !is_paused;
        if (is_paused) { // entered pause mode, show animation //
            draw::get_instance()->draw_in_game_menu_animation();
        }
        screen = 0;
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(10);
    }
    if (is_paused == true) {
        if (InputController::get_instance()->p1_input[BTN_L] == 1) {
            screen--;
        } else if (InputController::get_instance()->p1_input[BTN_R] == 1) {
            screen++;
        }
        if (screen < 0) {
            screen = 2;
        } else if (screen > 2) {
            screen = 0;
        }
        InputController::get_instance()->clean();
        TimerView::get_instance()->delay(200);
        draw::get_instance()->draw_in_game_menu_bg(screen);
    }
    return is_paused;
}
