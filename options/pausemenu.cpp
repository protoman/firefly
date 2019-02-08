#include "pausemenu.h"

#include "controller/inputcontroller.h"
#include "view/timerview.h"
#include "view/imageview.h"

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
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, AREA_H, 0, 0, 20);
        for (int i=GRID_SIZE; i<AREA_H; i+=GRID_SIZE) {
            ImageView::get_instance()->clearScreenArea(0, i, RES_W, 2, 79, 26, 97);
        }
        for (int i=GRID_SIZE; i<RES_W; i+=GRID_SIZE) {
            ImageView::get_instance()->clearScreenArea(i, 0, 2, AREA_H, 79, 26, 97);
        }
    }
    return is_paused;
}
