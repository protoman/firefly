#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#include "file/v4/file_config_v4.h"
#include "file/v4/file_game_v4.h"
#include "file/v4/file_scene_v4.h"
#include "file/v4/file_anim_block.h"
#include "file/v5/struct_file_map.h"
#include "file/v5/struct_file_area.h"
#include "file/v5/struct_file_area_link.h"
#include "file/v5/struct_file_game_area_map.h"
#include "file/v5/struct_file_area_data.h"

#include "file/v6/file_level_v6.h"
#include "file/v6/file_area_v6.h"
#include "file/v6/file_room_v6.h"
#include "file/v6/file_object_v6.h"
#include "file/v6/file_game_object_state.h"


class SharedData
{
public:
    static SharedData* get_instance();

private:
    SharedData();
    SharedData(SharedData const&){};             // copy constructor is private
    SharedData& operator=(SharedData const&){ return *this; };  // assignment operator is private


private:
    static SharedData* _instance;

public:
    SDL_Window* window;
    SDL_Surface* screenSurface;
    TTF_Font *lowercase_font;
    TTF_Font *outline_font;

    st_game_config game_config;
    std::string FILEPATH = std::string("./games/FireFly/");
    std::string GAMEPATH = std::string("./");
    std::string GAMENAME = std::string("FireFly");
    std::string SAVEPATH = std::string(".");

    SDL_Event event;
    bool leave_game = false;
    bool run_game = true;


    // GAME-DATA //
    file_game game_data;
    std::vector<file_npc_v3_1_2> enemy_list;
    std::vector<file_projectilev3> projectile_list_v3;
    std::vector<file_scene_list> scene_list;
    std::vector<file_anim_block> anim_block_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    std::vector<file_artificial_inteligence> ai_list;
    std::vector<file_v5_slope_tile> slope_list;
    //std::vector<struct_file_v5_area> area_list;

    st_save game_save;
    st_checkpoint checkpoint;


    // MAPS DATA //
    std::vector<file_v5_map_link> file_v5_map_link_list;

    std::map<unsigned int, std::vector<v6_map_object>>  file_v6_map_object_map; // map objects


    std::map<unsigned int, std::vector<file_v5_map_npc>>  file_v5_map_npc_map; // map enemies
    std::map<unsigned int, std::vector<struct_file_v5_area_link>> file_v5_area_link_map;
    std::vector<struct_file_v5_area_link> current_area_link_list;
    unsigned int file_v5_selected_area = 0;
    unsigned int file_v5_selected_layer = 0;
    unsigned int file_v5_selected_slope = 0;

    std::map<st_position, std::vector<file_v5_map_room>>  file_v5_room_map; // map rooms

    std::vector<struct_file_level_header> level_header_vector;
    std::vector<std::vector<file_v5_level_screen_data>> level_data_vector;


    // those are used to get a "global" position, in order to find witch is the room you are in the area
    st_file_game_area_room area_map[GAME_AREA_SIZE][GAME_AREA_SIZE];
    long area_scroll_x = 0;
    long area_scroll_y = 0;




    // FILE-V6 //
    std::vector<file_v6_level> v6_level_list;
    std::vector<file_v6_area> v6_area_list;
    std::vector<st_size> v6_area_list_size;
    int v6_selected_area = 0;
    int v6_selected_level = 0;
    file_v6_level v6_current_level_data;
    std::vector<v6_file_object> v6_object_list;

    int leftmost_room = FILE_AREA_W;
    int rightmost_room = 0;
    int topmost_room = FILE_AREA_H;
    int bottommost_room = 0;
    std::vector<st_position> area_room_list;
    std::vector<file_v6_level_visited> visited_level_list;
    st_position current_room_pos;

    int level_count = 0;

    std::vector<std::vector<std::string>> dialog_list;
    int currentDialog = 0;
    unsigned long get_item_timer = 0;

    bool must_interrupt_character_execution = false;
    bool fullscreen_mode = false;

    std::vector<st_light_point> lightpoint_list;

    std::map<int, v6_file_game_object_state> game_object_state_map; // key x for area-id, y for area-object-n

};

#endif // SHAREDDATA_H
