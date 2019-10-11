#include "game_mediator.h"

#include "view/soundview.h"

// Global static pointer used to ensure a single instance of the class.
GameMediator* GameMediator::_instance = nullptr;


GameMediator *GameMediator::get_instance()
{
    if (!_instance) {
        _instance = new GameMediator();
    }
    return _instance;

}

Mix_Chunk* GameMediator::get_sfx(std::string filename)
{
    std::map<std::string, Mix_Chunk*>::iterator it = sfx_map.find(filename);
    if (it == sfx_map.end()) {
        Mix_Chunk* sfx = SoundView::get_instance()->sfx_from_file(filename);
        sfx_map.insert(std::pair<std::string, Mix_Chunk*>(filename, sfx));
        return sfx;
    } else {
        return it->second;
    }
}

file_projectilev3 GameMediator::get_projectile(int n)
{
    if (n < 0 || n >= SharedData::get_instance()->projectile_list_v3.size()) {
        return SharedData::get_instance()->projectile_list_v3.at(0);
    }
    return SharedData::get_instance()->projectile_list_v3.at(n);
}

int GameMediator::get_projectile_list_size()
{
    return SharedData::get_instance()->projectile_list_v3.size();
}

file_npc_v3_1_2* GameMediator::get_enemy(int n)
{
    // boss not yet set
    if (n < 0 || n >= enemy_list.size()) {
        std::cout << "ERROR: GameMediator::get_enemy - invalid enemy position[" << n << "], list-size: " << enemy_list.size() << std::endl;
        // return first NPC to avoid errors
        n = 0;
    }
    return &enemy_list.at(n);
}

int GameMediator::get_enemy_list_size()
{
    return enemy_list.size();
}


GameMediator::GameMediator()
{
    enemy_list = fio_cmm.load_from_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "/game_enemy_list_3_1_2.dat");
    SharedData::get_instance()->v6_object_list = fio_cmm.load_from_disk<v6_file_object>(SharedData::get_instance()->FILEPATH + "/game_object_list_v6.dat");
    ai_list = fio_cmm.load_from_disk<file_artificial_inteligence>(SharedData::get_instance()->FILEPATH + "/game_ai_list.dat");

    anim_tile_list = fio_cmm.load_from_disk<file_anim_block>(SharedData::get_instance()->FILEPATH + "/anim_block_list.dat");
    player_list_v3_1 = fio_cmm.load_from_disk<file_player_v3_1_1>(SharedData::get_instance()->FILEPATH + "/player_list_v3_1_1.dat");


    // add some dummy data for game not to crash
    if (player_list_v3_1.size() == 0) {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            player_list_v3_1.push_back(file_player_v3_1_1(i));
        }
    }

    SharedData::get_instance()->v6_area_list = fio_cmm.load_from_disk<file_v6_area>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST);
}


