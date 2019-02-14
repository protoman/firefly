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

class SharedData
{
public:
    static SharedData* get_instance();

private:
    SharedData();
    SharedData(SharedData const&){};             // copy constructor is private
    SharedData& operator=(SharedData const&){};  // assignment operator is private


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
    std::vector<file_object> object_list;
    std::vector<file_projectilev3> projectile_list_v3;
    std::vector<file_scene_list> scene_list;
    std::vector<file_anim_block> anim_block_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    std::vector<file_artificial_inteligence> ai_list;
    std::vector<file_v5_slope_tile> slope_list;
    std::vector<struct_file_v5_area> area_list;

    st_save game_save;
    st_checkpoint checkpoint;


    // MAPS DATA //
    std::vector<file_v5_map_header> file_v5_map_header_list;
    std::vector<file_v5_map_link> file_v5_map_link_list;
    std::map<unsigned int, std::vector<file_v5_map_tile>>  file_v5_map_tile_map; // map tiles
    std::map<unsigned int, std::vector<file_v5_map_object>>  file_v5_map_object_map; // map objects
    std::map<unsigned int, std::vector<file_v5_map_npc>>  file_v5_map_npc_map; // map enemies
    std::map<unsigned int, std::vector<struct_file_v5_area_link>> file_v5_area_link_map;
    unsigned int file_v5_selected_map = 0;
    unsigned int file_v5_selected_layer = 0;
    unsigned int file_v5_selected_slope = 0;

};

#endif // SHAREDDATA_H
