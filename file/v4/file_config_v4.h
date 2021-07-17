#ifndef FILE_CONFIG_301_H
#define FILE_CONFIG_301_H

#include <SDL2/SDL.h>

#include "data/st_common.h"



struct st_game_config {
    bool sound_enabled;
    bool video_fullscreen;
    E_INPUT_TYPES input_type;                           // if keyboard or joystick
    E_INPUT_MODES input_mode;                           // inf directional is joypad-digital, analog sick or hat
    E_PLATFORM platform;                                // if changed, must reset config to default
    int keys_codes[BTN_COUNT];                          // number indicator for the keyboard-keys
    st_input_button_config button_codes[BTN_COUNT];     // number indicator for the joystick-button-keys

    bool game_finished;                                 // stores if game was finished, so we can show more options to player
    Uint8 selected_input_device;
    Uint8 selected_language;
    bool turbo_mode;
    bool auto_charge_mode;
    Uint8 volume_sfx;
    Uint8 volume_music;
    bool android_touch_controls_hide;                   // define if touch controls must be hidden
    Uint8 android_touch_controls_size;                  // for android, size of the on-screen controls 0 = small, 1 = normal, 2 = big
    Uint8 graphics_performance_mode;                    // 0 => lowend, 1=> normal, 2 => highend
    bool first_run;
    // FOR FUTURE USAGE
    Sint8 CONFIG_PLACEHOLDER_001 = 0;
    Sint8 CONFIG_PLACEHOLDER_002 = 0;
    Sint8 CONFIG_PLACEHOLDER_003 = 0;
    Sint8 CONFIG_PLACEHOLDER_004 = 0;
    Sint8 CONFIG_PLACEHOLDER_005 = 0;
    Sint8 CONFIG_PLACEHOLDER_006 = 0;
    Sint8 CONFIG_PLACEHOLDER_007 = 0;
    Sint8 CONFIG_PLACEHOLDER_008 = 0;
    Sint8 CONFIG_PLACEHOLDER_009 = 0;
    Sint8 CONFIG_PLACEHOLDER_010 = 0;

    void get_default_keys(int (&keys_codes_copy)[BTN_COUNT]) {
    keys_codes_copy[BTN_ATTACK] = SDLK_a;
    keys_codes_copy[BTN_JUMP] = SDLK_x;
    keys_codes_copy[BTN_DASH] = SDLK_c;
    keys_codes_copy[BTN_ITEM] = SDLK_z;
    keys_codes_copy[BTN_L] = SDLK_q;
    keys_codes_copy[BTN_R] = SDLK_w;
    keys_codes_copy[BTN_QUIT] = SDLK_ESCAPE;
    keys_codes_copy[BTN_START] = SDLK_RETURN;
    keys_codes_copy[BTN_LEFT] = SDLK_LEFT;
    keys_codes_copy[BTN_RIGHT] = SDLK_RIGHT;
    keys_codes_copy[BTN_UP] = SDLK_UP;
    keys_codes_copy[BTN_DOWN] = SDLK_DOWN;
    }

    void get_default_buttons(st_input_button_config (&button_codes_copy)[BTN_COUNT]) {
        // reset values
        for (int i=0; i<BTN_COUNT; i++) {
            button_codes_copy[i].type = JOYSTICK_INPUT_TYPE_BUTTON;
            button_codes_copy[i].value = -1;
            button_codes_copy[i].axis_type = 0;
        }

        // COMMON INPUT FOR JOYSTICK AXIS //

        //std::cout << "#1 BTN_DOWN[" << BTN_DOWN << "].axis_type[" << button_codes_copy[BTN_DOWN].axis_type << "].value[" << button_codes_copy[BTN_DOWN].value << "]" << std::endl;

        button_codes_copy[BTN_DOWN].axis_type = 1;
        button_codes_copy[BTN_DOWN].type = JOYSTICK_INPUT_TYPE_AXIS;
        button_codes_copy[BTN_DOWN].value = 1;

        //std::cout << "#2 BTN_DOWN[" << BTN_DOWN << "].axis_type[" << button_codes_copy[BTN_DOWN].axis_type << "].value[" << button_codes_copy[BTN_DOWN].value << "]" << std::endl;

        button_codes_copy[BTN_UP].axis_type = -1;
        button_codes_copy[BTN_UP].type = JOYSTICK_INPUT_TYPE_AXIS;
        button_codes_copy[BTN_UP].value = 1;

        button_codes_copy[BTN_RIGHT].axis_type = 1;
        button_codes_copy[BTN_RIGHT].type = JOYSTICK_INPUT_TYPE_AXIS;
        button_codes_copy[BTN_RIGHT].value = 0;

        button_codes_copy[BTN_LEFT].axis_type = -1;
        button_codes_copy[BTN_LEFT].type = JOYSTICK_INPUT_TYPE_AXIS;
        button_codes_copy[BTN_LEFT].value = 0;

#ifdef ANDROID
        for (int i=0; i<BTN_COUNT; i++) {
            button_codes_copy[i].type = JOYSTICK_INPUT_TYPE_BUTTON;
            button_codes_copy[i].value = -1;
            button_codes_copy[i].axis_type = 0;
        }
#else
        button_codes_copy[BTN_ATTACK].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_ATTACK].value = 2;
        button_codes_copy[BTN_JUMP].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_JUMP].value = 1;
        button_codes_copy[BTN_DASH].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_DASH].value = 0;
        button_codes_copy[BTN_ITEM].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_ITEM].value = 3;
        button_codes_copy[BTN_L].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_L].value = 6;
        button_codes_copy[BTN_R].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_R].value = 7;
        button_codes_copy[BTN_QUIT].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_QUIT].value = 8;
        button_codes_copy[BTN_START].type = JOYSTICK_INPUT_TYPE_BUTTON;
        button_codes_copy[BTN_START].value = 9;
#endif
    }

    void set_default_keys() {
        input_type = INPUT_TYPE_DOUBLE;
        get_default_keys(keys_codes);
        get_default_buttons(button_codes);
    }

    E_PLATFORM get_current_platform() const {
#if LINUX
        return PLATFORM_LINUX;
#elif ANDROID
        return PLATFORM_ANDROID;
#else
        return PLATFORM_WINDOWS;
#endif
    }

    void reset() {
        sound_enabled = true;
        video_fullscreen = false;
        // SET KEYS/TYPES - triangle=shield; circle=dash; x=jump; square=attack; start=start; select=quit
        set_default_keys();
        platform = get_current_platform();
        // Note: do not change game_finished when resetting
        selected_input_device = 0;
        selected_language = LANGUAGE_AUTODETECT;
        turbo_mode = false;
        auto_charge_mode = false;
        volume_sfx = 90;
        volume_music = 128;

        android_touch_controls_hide = false;
        android_touch_controls_size = 1;
        graphics_performance_mode = PERFORMANCE_MODE_HIGH;
        CONFIG_PLACEHOLDER_001 = 0;
        CONFIG_PLACEHOLDER_002 = 0;
        CONFIG_PLACEHOLDER_003 = 0;
        CONFIG_PLACEHOLDER_004 = 0;
        CONFIG_PLACEHOLDER_005 = 0;
        CONFIG_PLACEHOLDER_006 = 0;
        CONFIG_PLACEHOLDER_007 = 0;
        CONFIG_PLACEHOLDER_008 = 0;
        CONFIG_PLACEHOLDER_008 = 0;
        CONFIG_PLACEHOLDER_010 = 0;
    }


    st_game_config() {
        game_finished = false;
        first_run = true;
        reset();
    }
};



#endif // FILE_CONFIG_H
