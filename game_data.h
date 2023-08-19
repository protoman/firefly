#ifndef GAMEMEDIATOR_H
#define GAMEMEDIATOR_H

#include <cstdio>
#include <string>
#include <vector>
#include <map>

#include "file/format.h"
#include "file/file_io.h"
#include "file/fio_common.h"
#include "file/fio_strings.h"
#include "file/v5/struct_file_map.h"
#include "file/v6/file_level_v6.h"
#include "file/v6/file_area_v6.h"
#include "file/v6/file_npc_state_v6.h"
#include "file/v6/file_object_v6.h"

// this class contains elements related to the game structure loaded from files, or mapped from it
class GameData
{
public:
    static GameData* get_instance();

    file_projectilev3 get_projectile(int n);
    int get_projectile_list_size();

    file_enemy_v3_1_2* get_enemy(int n);
    int get_enemy_list_size();
    void add_enemy(file_enemy_v3_1_2 enemy);

    file_npc_v3_1_2* get_npc(unsigned int n);
    int get_npc_list_size();
    void add_npc(file_npc_v3_1_2 npc);

    void load_data();
    void loadStageRooms(int area_n);

private:
    GameData();
    GameData(GameData const&){};             // copy constructor is private
    GameData& operator=(GameData const&){ return *this; };  // assignment operator is private


    // data-load methods
    void load_enemy_list();
    void load_npc_list();
    void load_ai_list();
    void load_projectile_list();
    void load_slope_list();
    void load_style_list();
    void loadGameData();
    void loadNpcStateData();
    void loadMapData();
    void loadAreaListSize();
    void load_anim_tile_list();
    void load_player_list();

public:
    file_game game_data;
    std::vector<file_artificial_inteligence> ai_list;
    std::vector<file_anim_block> anim_tile_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    std::vector<file_projectilev3> projectile_list_v3;
    std::vector<file_v6_stage> v6_stage_list;                                           // list of stages
    std::map<int, std::vector<file_v6_area>> v6_area_map;                               // map of areas, that are a sub-parts of the stage rooms that share same style
    std::map<st_position, file_v6_room> v6_area_room_list;                              // list of rooms for a given area mapped by their position on world-map
    std::map<unsigned int, std::vector<file_v5_map_npc>>  file_v5_stage_npc_map;        // map NPCs
    std::map<unsigned int, std::vector<file_v5_map_npc>>  file_v5_stage_enemy_map;      // map enemies
    std::vector<file_v5_slope_tile> slope_list;
    std::vector<file_v6_style> v6_style_list;                                           // list of styles
    std::map<int, short> npc_state_map;
    std::map<unsigned int, std::vector<v6_stage_object>>  file_v6_stage_objects_map;    // map objects
    std::vector<v6_file_object> v6_object_list;                                         // game objects
    std::vector<file_enemy_v3_1_2> enemy_list;
    std::vector<file_npc_v3_1_2> npc_list;


    /*
    std::vector<file_enemy_v3_1_2> enemy_list;
    std::vector<file_scene_list> scene_list;
    std::vector<file_anim_block> anim_block_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    std::vector<file_artificial_inteligence> ai_list;
    std::vector<file_npc_v3_1_2> npc_list;
    // FILE-V6 //

    */

private:
    static GameData* _instance;
    fio_common fio_cmm;
    file_io fio;




};

#endif // GAMEMEDIATOR_H
