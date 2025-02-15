#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_haptic.h>
#include <iostream>
#include <vector>

#include "file/v4/file_config_v4.h"
#include "data/shareddata.h"
#include "view/timerview.h"

struct input_sequence {
    bool activated;
    short key_n;

    input_sequence(int key) {
        activated = false;
        key_n = key;
    }

    input_sequence() {
        activated = false;
        key_n = -1;
    }
};


class InputController
{
public:
    static InputController* get_instance();
    void init();
    void change_joystick();
    void close_joystick();
    int wait_scape_time(int);
    int clean_and_wait_scape_time(int);
    void clean();
    void clean_all();     /// removes also the attack state
    void save();
    void wait_keypress(); /// waits intil a key is pressed
    void clean_confirm_button();

    bool pick_key_or_button(st_game_config &game_config_copy, INPUT_COMMANDS key);

    int get_joysticks_number();

    std::string get_key_name(int key);

    void read_input();
    void read_input(bool check_input_reset, bool check_input_cheat);
    void check_cheat_input();
    void reset_cheat_input_sequence();
    void reset_cheat_input();
    bool is_check_input_reset_command_activated();
    bool is_check_input_cheat_command_activated();
    void clean_event_queue();
    void test_erumble();

private:
    InputController();
    InputController(InputController const&){};             // copy constructor is private
    InputController& operator=(InputController const&){ return *this; };  // assignment operator is private


public:
    Uint8 p1_input[BTN_COUNT];
    Uint8 p1_previous_input[BTN_COUNT];
    Uint8 p1_save_input[BTN_COUNT];


private:
    static InputController* _instance;
    SDL_Joystick *joystick1 = nullptr;
    SDL_Haptic *joystick1_haptic = nullptr;
    bool _used_keyboard;
    bool _show_btn_debug;
    int held_button_count;
    bool cheat_input_is_active;
    short cheat_input_count;
    short cheat_input_previous;
    std::vector<input_sequence> cheat_input_sequence;
    long held_button_timer;
    int default_keys_codes[BTN_COUNT];

    bool alt_key = false;
    bool enter_key = false;
    int joystick_number = 0;

};

#endif // INPUTCONTROLLER_H
