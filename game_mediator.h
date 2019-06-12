#ifndef GAMEMEDIATOR_H
#define GAMEMEDIATOR_H

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL_mixer.h>

#include "file/format.h"
#include "file/fio_common.h"
#include "file/fio_strings.h"
#include "file/v6/file_level_v6.h"

class GameMediator
{
public:
    static GameMediator* get_instance();
    Mix_Chunk *get_sfx(std::string filename);
    file_projectilev3 get_projectile(int n);
    int get_projectile_list_size();

    file_npc_v3_1_2* get_enemy(int n);
    int get_enemy_list_size();

private:
    GameMediator();
    GameMediator(GameMediator const&){};             // copy constructor is private
    GameMediator& operator=(GameMediator const&){};  // assignment operator is private

public:
    std::vector<file_object> object_list;
    std::vector<file_artificial_inteligence> ai_list;
    std::vector<file_anim_block> anim_tile_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    std::vector<file_projectilev3> projectile_list;
    std::map<std::string, st_imageData> character_graphic_map;      // character graphics

private:
    static GameMediator* _instance;
    std::map<std::string, Mix_Chunk*> sfx_map;                                               // map of sfx files loaded, used in projectiles to avoid loading from disk every time
    fio_common fio_cmm;

    std::vector<file_npc_v3_1_2> enemy_list;



};

#endif // GAMEMEDIATOR_H
