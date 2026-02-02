#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <vector>
#include <set>
#include <map>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


#include "file/v4/file_config_v4.h"
#include "file/v4/file_game_v4.h"
#include "file/v4/file_scene_v4.h"
#include "file/v4/file_anim_block.h"
#include "file/v5/struct_file_map.h"
#include "file/v5/struct_file_game_area_map.h"

#include "file/v6/file_level_v6.h"
#include "file/v6/file_area_v6.h"
#include "file/v6/file_room_v6.h"
#include "file/v6/file_object_v6.h"
#include "file/v6/file_game_object_state.h"

#include "text/i18ntext.h"

class SharedData
{
public:
    static SharedData* get_instance();
    std::string get_player_face_file();

private:
    SharedData();
    SharedData(SharedData const&){};             // copy constructor is private
    SharedData& operator=(SharedData const&){ return *this; };  // assignment operator is private

public:



private:
    static SharedData* _instance;

public:
    SDL_Window* window;
    SDL_Surface* screenSurface;
    TTF_Font *lowercase_font;
    TTF_Font *outline_font;

    st_game_config game_config;
    std::string FILEPATH = std::string("./games/FireFly/");
    std::string FILEDATAPATH = std::string("./games/FireFly/data");
    std::string GAMEPATH = std::string("./");
    std::string GAMENAME = std::string("FireFly");
    std::string SAVEPATH = std::string("/home/iuri/.firefly"); // @TODO: use the same as in RockDroid

    SDL_Event event;
    bool leave_game = false;
    bool run_game = true;


    // GAME-DATA //
    st_save game_save;
    st_checkpoint checkpoint;

    unsigned int file_v5_selected_area = 0;
    unsigned int file_v5_selected_layer = 0;
    unsigned int file_v5_selected_slope = 0;
    st_position current_room_pos;


    // those are used to get a "global" position, in order to find witch is the room you are in the area
    long area_scroll_x = 0;
    long area_scroll_y = 0;

    unsigned int v6_selected_stage = 0;
    unsigned int v6_selected_area = 0;
    std::vector<st_position> area_room_list;


    int level_count = 0;

    std::vector<std::vector<std::string>> dialog_list;
    int currentDialog = 0;
    unsigned long get_item_timer = 0;

    bool must_interrupt_character_execution = false;
    bool fullscreen_mode = false;

    std::vector<st_light_point> lightpoint_list;

    std::map<int, v6_file_game_object_state> game_object_state_map; // key x for area-id, y for area-object-n
    int current_player = 1;
    std::set<e_GAME_DIALOG> used_game_dialogs;                       // once added in this list, dialog will return empty, so won't repeat again

    st_size window_size;
    bool window_size_changed = false;

    // resolution related vars //
    //st_size screen_resolution = st_size(1280, 720);
    //int screen_resolution_tilesize = 36;

    int topmost_room = 99999;
    int bottommost_room = -1;
    int leftmost_room = 99999;
    int rightmost_room = -1;
    int total_editarea_w = 0;
    int total_editarea_h = 0;


    int clear_point_x = 0;
    int clear_point_y = 0;
    bool is_showing_boss_intro = false;

    int current_language = LANGUAGE_AUTODETECT;

    unsigned int room_style_selected_style = 0;
    unsigned int room_style_selected_layer = 0;

    unsigned int selected_npc = 0;


};

#endif // SHAREDDATA_H
