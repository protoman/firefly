#include "game_data.h"

#include <data/shareddata.h>

// Global static pointer used to ensure a single instance of the class.
GameData* GameData::_instance = nullptr;


GameData *GameData::get_instance()
{
    if (!_instance) {
        _instance = new GameData();
    }
    return _instance;
}

GameData::GameData()
{
    //load_data();
}

file_projectile_v0 GameData::get_projectile(unsigned int n)
{
    if (n < 0 || n >= projectile_list_v0.size()) {
        return projectile_list_v0.at(0);
    }
    return projectile_list_v0.at(n);
}

int GameData::get_projectile_list_size()
{
    return projectile_list_v0.size();
}

file_enemy_v3_1_2* GameData::get_enemy(unsigned int n)
{
    // boss not yet set
    if (n < 0 || n >= enemy_list.size()) {
        std::cout << "ERROR: GameMediator::get_enemy - invalid enemy position[" << n << "], list-size: " << enemy_list.size() << std::endl;
        // return first NPC to avoid errors
        n = 0;
    }
    return &enemy_list.at(n);
}

int GameData::get_enemy_list_size()
{
    return enemy_list.size();
}

void GameData::add_enemy(file_enemy_v3_1_2 enemy)
{
    enemy_list.push_back(enemy);
}


file_npc_v3_1_2 *GameData::get_npc(unsigned int n)
{
    if (n < 0 || n >= npc_list.size()) {
        std::cout << "ERROR: GameMediator::get_npc - invalid enemy position[" << n << "], list-size: " << npc_list.size() << std::endl;
        // return first NPC to avoid errors
        n = 0;
    }
    return &npc_list.at(n);
}

int GameData::get_npc_list_size()
{
    return npc_list.size();
}

void GameData::add_npc(file_npc_v3_1_2 npc)
{
    npc_list.push_back(npc);
}

void GameData::load_data()
{
    loadGameData();
    load_style_list();
    load_enemy_list();
    load_npc_list();
    load_ai_list();
    load_projectile_list();
    load_slope_list();
    loadNpcStateData();
    load_anim_tile_list();
    loadMapData();
    load_player_list();
}

void GameData::loadStageRooms(int area_n)
{
    char area_rooms_filename[512];
    sprintf(area_rooms_filename, "%s/data/v6_area_rooms_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), area_n);
    if (fio.file_exists(area_rooms_filename)) {
        std::vector<file_v6_room> room_list = fio_cmm.load_from_disk<file_v6_room>(area_rooms_filename);
        // convert to a map based upon the world-map position, so we can get all rooms easily
        v6_area_room_list.clear();
        for (unsigned int i=0; i<room_list.size(); i++) {
           v6_area_room_list.insert(std::pair<st_position, file_v6_room>(room_list.at(i).position, room_list.at(i)));
        }
    }
}

void GameData::load_enemy_list()
{
    enemy_list = fio_cmm.load_from_disk<file_enemy_v3_1_2>(SharedData::get_instance()->FILEPATH + "/game_enemy_list_3_1_2_b.dat");
    if (enemy_list.size() == 0) {
        enemy_list.push_back(file_enemy_v3_1_2());
    }
}

void GameData::load_npc_list()
{
    npc_list = fio_cmm.load_from_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_npc_list_3_1_2_b.dat");
    if (npc_list.size() == 0) {
        npc_list.push_back(file_npc_v3_1_2());
    }

}

void GameData::load_ai_list()
{
    ai_list_v0 = fio_cmm.load_json_data<file_artificial_intelligence_v0>(SharedData::get_instance()->FILEPATH + "data/game_ai_list_v0.json");

    ai_list = fio_cmm.load_from_disk<file_artificial_intelligence>(SharedData::get_instance()->FILEPATH + "/game_ai_list.dat");
    if (ai_list.size() == 0) { // add one first item to avoid errors
        for (unsigned int i=0; i<enemy_list.size(); i++) {
            ai_list.push_back(file_artificial_intelligence());
        }
    }
    if (ai_list.size() == 0) { // add one first item to avoid errors
        ai_list.push_back(file_artificial_intelligence());
    }
}

void GameData::load_projectile_list()
{
    projectile_list_v0 = fio_cmm.load_json_data<file_projectile_v0>(SharedData::get_instance()->FILEPATH + "data/projectiles_v0.json");
}

void GameData::load_slope_list()
{
    slope_list = fio_cmm.load_from_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH+FILE_V5_MAP_SLOPE_LIST);
}

void GameData::load_style_list()
{
    v6_style_list = fio_cmm.load_from_disk<file_v6_style>(SharedData::get_instance()->FILEPATH + FILE_V6_STYLE_LIST);
    if (v6_style_list.size() == 0) {
        v6_style_list.push_back(file_v6_style());
    }

}

void GameData::loadGameData()
{

    //std::cout << "loadGameData - FILEPATH[" << SharedData::get_instance()->FILEPATH << "]" << std::endl;

    fio.read_game(game_data);

    // CURRENT AREA DATA //
    char level_filename[512];
    sprintf(level_filename, "%s/data/v6_level_list_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), SharedData::get_instance()->v6_selected_stage);
    std::vector<file_v6_level_point> point_list = fio_cmm.load_from_disk<file_v6_level_point>(level_filename);
    // CURRENT AREA-ROOMS
    loadStageRooms(SharedData::get_instance()->v6_selected_stage);

    // ENEMIES LIST
    loadNpcStateData();

    v6_object_list = fio_cmm.load_from_disk<v6_file_object>(SharedData::get_instance()->FILEPATH + "/game_object_list_v6.dat");
    if (v6_object_list.size() == 0) { // add one first item to avoid errors
        v6_object_list.push_back(v6_file_object());
    }
}

void GameData::loadNpcStateData()
{
    // NPCS STATE LIST
    std::vector<file_npc_state> npc_state_list;
    if (fio.file_exists(SharedData::get_instance()->FILEPATH + "/game_npc_state_list_3_1_2.dat")) { // load list from disk
        npc_state_list = fio_cmm.load_from_disk<file_npc_state>(SharedData::get_instance()->FILEPATH + "/game_npc_state_list_3_1_3.dat");
    } else { // first-time list generation
        for (int i=0; i<get_npc_list_size(); i++) {
            //file_npc_v3_1_2 npc = SharedData::get_instance()->'npc_list'.at(i);
            npc_state_list.push_back(file_npc_state(i, false));
        }
    }
    for (unsigned int i=0; i<npc_state_list.size(); i++) { // convert to a map
        npc_state_map.insert(std::pair<int, short>(npc_state_list.at(i).npc_id, npc_state_list.at(i).state));
    }
    for (unsigned int i=0; i<enemy_list.size(); i++) { // fill missing npcs
        if (npc_state_map.find(i) == npc_state_map.end()) {
            npc_state_map.insert(std::pair<int, short>(i, 0));
        }
    }
}

void GameData::loadMapData()
{
    // FILE V5 //
    stage_list_v0 = fio_cmm.load_json_data<file_stage_v0>(SharedData::get_instance()->FILEPATH + "data/stages_v0.json");
    v6_stage_list = fio_cmm.load_from_disk<file_v6_stage>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST);

    for (unsigned int i=0; i<v6_stage_list.size(); i++) {
        // load map objects
        char map_objects_name[FS_CHAR_FILENAME_SIZE];
        sprintf(map_objects_name, "/data/v6_map_%d_objects.dat", i);
        file_v6_stage_objects_map.insert(std::pair<unsigned int, std::vector<v6_stage_object>>(i, std::vector<v6_stage_object>()));
        if (fio.file_exists(SharedData::get_instance()->FILEPATH+map_objects_name)) {
            file_v6_stage_objects_map.at(i) = fio_cmm.load_from_disk<v6_stage_object>(SharedData::get_instance()->FILEPATH+map_objects_name);
            //std::cout << ">>>> gameManager::loadMapData - LOAD-MAP-LINK-FILE[" << map_link_name << "], SIZE[" << SharedData::get_instance()->file_v6_map_object_map.at(i).size() << "]" << std::endl;
        }

        // map enemies //
        char map_enemies_name[FS_CHAR_FILENAME_SIZE];
        sprintf(map_enemies_name, "/data/v5_map_%d_enemies.dat", i);
        file_v5_stage_enemy_map.insert(std::pair<unsigned int, std::vector<file_v5_map_npc>>(i, std::vector<file_v5_map_npc>()));
        if (fio.file_exists(SharedData::get_instance()->FILEPATH+map_enemies_name)) {
            file_v5_stage_enemy_map.at(i) = fio_cmm.load_from_disk<file_v5_map_npc>(SharedData::get_instance()->FILEPATH+map_enemies_name);
        }

        // map NPCs //
        char map_npcs_name[FS_CHAR_FILENAME_SIZE];
        sprintf(map_npcs_name, "/data/v5_map_%d_npcs.dat", i);
        file_v5_stage_npc_map.insert(std::pair<unsigned int, std::vector<file_v5_map_npc>>(i, std::vector<file_v5_map_npc>()));
        if (fio.file_exists(SharedData::get_instance()->FILEPATH+map_npcs_name)) {
            file_v5_stage_npc_map.at(i) = fio_cmm.load_from_disk<file_v5_map_npc>(SharedData::get_instance()->FILEPATH+map_npcs_name);
        }
    }
    v6_area_map.clear();
    for (unsigned int i=0; i<v6_stage_list.size(); i++) {
        char area_filename[512];
        sprintf(area_filename, "%s/%s%d.dat", SharedData::get_instance()->FILEPATH.c_str(), FILE_V6_AREA_LIST_PREFIX, i);
        std::vector<file_v6_area> stage_area_list = fio_cmm.load_from_disk<file_v6_area>(area_filename);
        if (stage_area_list.size() == 0) {
            stage_area_list.push_back(file_v6_area());
        }
        std::pair<int, std::vector<file_v6_area>> new_area_pair(i, stage_area_list);
        v6_area_map.insert(new_area_pair);
    }
    load_map_data_v0();
}

void GameData::load_map_data_v0()
{
    // FILE V5 //
    stage_list_v0 = fio_cmm.load_json_data<file_stage_v0>(SharedData::get_instance()->FILEPATH + "data/stages_v0.json");

    for (unsigned int i=0; i<stage_list_v0.size(); i++) {
        // load map objects
        file_stage_objects_map_v0.clear();
        std::string map_objects_filename = SharedData::get_instance()->FILEPATH + "/data/map_" + std::to_string(i) + "_objects_v0.json";
        file_stage_objects_map_v0.insert(std::pair<unsigned int, std::vector<stage_object_v0>>(i, std::vector<stage_object_v0>()));
        if (fio.file_exists(map_objects_filename)) {
            file_stage_objects_map_v0.at(i) = fio_cmm.load_json_data<stage_object_v0>(map_objects_filename);
        }
        // map enemies //
        std::string map_enemies_filename = SharedData::get_instance()->FILEPATH + "/data/map_" + std::to_string(i) + "_enemies_v0.json";
        file_stage_enemy_map_v0.insert(std::pair<unsigned int, std::vector<file_map_npc_v0>>(i, std::vector<file_map_npc_v0>()));
        if (fio.file_exists(map_enemies_filename)) {
            file_stage_enemy_map_v0.at(i) = fio_cmm.load_json_data<file_map_npc_v0>(map_enemies_filename);
        }
        // map NPCs //
        std::string map_npcs_filename = SharedData::get_instance()->FILEPATH + "/data/map_" + std::to_string(i) + "_npcs_v0.json";
        file_stage_npc_map_v0.insert(std::pair<unsigned int, std::vector<file_map_npc_v0>>(i, std::vector<file_map_npc_v0>()));
        if (fio.file_exists(map_npcs_filename)) {
            file_stage_npc_map_v0.at(i) = fio_cmm.load_json_data<file_map_npc_v0>(map_npcs_filename);
        }
    }
    // AREAS //
    area_map_v0.clear();
    for (unsigned int i=0; i<stage_list_v0.size(); i++) {
        std::string area_filename = SharedData::get_instance()->FILEPATH + "data/v6_area_list_" + std::to_string(0) + ".json";
        std::vector<file_area_v0> stage_area_list = fio_cmm.load_json_data<file_area_v0>(area_filename);
        if (stage_area_list.size() == 0) {
            stage_area_list.push_back(file_area_v0());
        }
        std::pair<int, std::vector<file_area_v0>> new_area_pair(i, stage_area_list);
        area_map_v0.insert(new_area_pair);
    }
}

void GameData::load_anim_tile_list()
{
    anim_tile_list_v0 = fio_cmm.load_json_data<file_anim_block_v0>(SharedData::get_instance()->FILEPATH + "data/anim_tile_list_v0.json");
    anim_tile_list = fio_cmm.load_from_disk<file_anim_block>(SharedData::get_instance()->FILEPATH + "/anim_block_list.dat");
}

void GameData::load_player_list()
{
    player_list_v0 = fio_cmm.load_json_data<file_player_v0>(SharedData::get_instance()->FILEPATH + "data/players_v0.json");

    player_list_v3_1 = fio_cmm.load_from_disk<file_player_v3_1_1>(SharedData::get_instance()->FILEPATH + "player_list_v3_1_1.dat");
    if (player_list_v3_1.size() == 0) {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            player_list_v3_1.push_back(file_player_v3_1_1(i));
        }
    }
}




