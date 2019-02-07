#include "mediator.h"
#include <stdio.h>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include "../file/format.h"
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "../file/file_io.h"

#define EDIT_MODE_NEW 0
#define EDIT_MODE_EDIT 1


// Global static pointer used to ensure a single instance of the class.
Mediator* Mediator::_instance = nullptr;

Mediator::Mediator()  {
	palleteX=0;
	palleteY=0;
    selectedTileset = "/images/tilesets/default.png";
	editMode = EDITMODE_NORMAL;
	editTool = EDITMODE_NORMAL;
	npcGraphicSize_w = 16;
	npcGraphicSize_h = 16;
	// projectile
	projectileGraphicSize_w = 16;
	projectileGraphicSize_h = 16;

	NpcAddNumber=0;
	layerLevel=1;
	selectedNPC = -1;
    selectedAnimTileset = 0;
	terrainType = 1;
	editModeNPC = EDIT_MODE_NEW;
	currentMap = 0;
	object_type = 3;
	npc_direction = 0;
    object_direction = 0;

	zoom = 1;
	currentStage = 1;
    currentDifficulty = DIFFICULTY_EASY;
    currentDifficultyMode = DIFFICULTY_MODE_GREATER;
	sprintf(addNpcFilename, "%c", '\0');
    sprintf(addProjectileFilename, "%c", '\0');

	link_bidi = true;
	link_is_door = false;
	picked_color_n = 0;


	current_weapon = 0;
    current_projectile = 0;
	current_player = 0;
	show_background_color = true;
	show_bg1 = true;
	show_fg_layer = true;
    playing_sprites = false;
    current_sprite_type = 0;
    current_sprite_selection = 0;
    current_npc_n = 0;
	current_ai = 0;

    show_objects_flag = true;
    show_npcs_flag = true;
    show_teleporters_flag = true;
    show_grid = true;

    combobox_select_string = "-Select-";

    selectedTileset = SharedData::get_instance()->FILEPATH + "/images/tilesets/default.png";

    if (enemy_list.size() == 0) { // add one first item to avoid errors
        enemy_list.push_back(file_npc_v3_1_2());
    }
    if (object_list.size() == 0) { // add one first item to avoid errors
        object_list.push_back(file_object());
    }
    if (ai_list.size() == 0) { // add one first item to avoid errors
        ai_list.push_back(file_artificial_inteligence());
    }

    if (scene_list.size() == 0) {
        scene_list.push_back(file_scene_list());
    }

    if (player_list_v3_1.size() == 0) {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            player_list_v3_1.push_back(file_player_v3_1_1(i));
        }
    }

    stage_dialog_list.clear();

    /*
    file_stage temp_stage;
    for (int i=0; i<FS_MAX_STAGES; i++) {
        stage_data.stages[i].dialog_face_graphics_filename[0] = '\03';
    }
    */


    stage_select_edit_mode = STAGE_SELECT_EDIT_MODE_PATH;




}



Mediator *Mediator::get_instance()
{
    if (!_instance) {
        _instance = new Mediator();
    }
    return _instance;
}


int Mediator::getPalleteX() {
	return palleteX;
}

int Mediator::getPalleteY() {
	return palleteY;
}

void Mediator::setPalleteX(int value) {
	palleteX = value;
}

void Mediator::setPalleteY(int value) {
	palleteY = value;
}

std::string Mediator::getPallete() {
    return selectedTileset;
}

void Mediator::setPallete(std::string filename)
{
    std::cout << "@@@@@@@@@@@@@@@@ Mediator::setPallete[" << filename << "]" << std::endl;
    selectedTileset =  filename;
}

void Mediator::setPallete(char *value) {
	selectedTileset = value;
}


void Mediator::load_game() {
    fio.read_game(game_data);
    std::string stages_extra_data_filename = "data/stages_extra_data" + fio.get_sufix() + ".dat";

    // convert enemy-ist to 3.1.2
    enemy_list = fio_cmm.load_from_disk<file_npc_v3_1_2>("game_enemy_list_3_1_2.dat");
    if (enemy_list.size() == 0) {
        enemy_list.push_back(file_npc_v3_1_2());
    }

    object_list = fio_cmm.load_from_disk<file_object>("game_object_list.dat");
    if (object_list.size() == 0) { // add one first item to avoid errors
        object_list.push_back(file_object());
    }

    ai_list = fio_cmm.load_from_disk<file_artificial_inteligence>("game_ai_list.dat");
    //std::cout << "MEDIATOR::load_game::ai_list.size(): " << ai_list.size() << std::endl;
    if (ai_list.size() == 0) { // add one first item to avoid errors
        for (int i=0; i<enemy_list.size(); i++) {
            ai_list.push_back(file_artificial_inteligence());
        }
    }


    projectile_list_v3 = fio_cmm.load_from_disk<file_projectilev3>(PROJECTILE_FILE_V3);
    if (projectile_list_v3.size() == 0) {
        projectile_list_v3.push_back(file_projectilev3());
    }
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@ projectile_list_v3.size[" << projectile_list_v3.size() << "]" << std::endl;


    scene_list = fio_scenes.load_scenes();
    if (scene_list.size() == 0) {
        scene_list.push_back(file_scene_list());
    }

    anim_block_list = fio_cmm.load_from_disk<file_anim_block>("anim_block_list.dat");

    player_list_v3_1 = fio_cmm.load_from_disk<file_player_v3_1_1>("player_list_v3_1_1.dat");
    if (player_list_v3_1.size() == 0) {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            player_list_v3_1.push_back(file_player_v3_1_1(i));
        }
    }

    ScenesMediator::get_instance()->load_game_scenes();


    // FILE V5 //
    SharedData::get_instance()->file_v5_map_header_list = fio_cmm.load_from_disk<file_v5_map_header>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_HEADER_LIST);
    SharedData::get_instance()->file_v5_map_link_list = fio_cmm.load_from_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST);
    SharedData::get_instance()->slope_list = fio_cmm.load_from_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST);
    SharedData::get_instance()->area_list = fio_cmm.load_from_disk<struct_file_v5_area>(SharedData::get_instance()->FILEPATH + FILE_V5_AREA_LIST);

    for (int i=0; i<SharedData::get_instance()->file_v5_map_header_list.size(); i++) {
        SharedData::get_instance()->file_v5_map_tile_map.insert(std::pair<int, std::vector<file_v5_map_tile>>(i, std::vector<file_v5_map_tile>()));
        QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");
        SharedData::get_instance()->file_v5_map_tile_map.at(i) = fio_cmm.load_from_disk<file_v5_map_tile>(filename.toStdString());
    }
    if (SharedData::get_instance()->file_v5_map_header_list.size() == 0) {
        SharedData::get_instance()->file_v5_map_header_list.push_back(file_v5_map_header());
    }


}



void Mediator::save_game()
{
    clean_data();
    //temp_fix_player_colors_order();

    Mediator::get_instance()->fio.write_game(game_data);

    std::string stages_extra_data_filename = "data/stages_extra_data" + fio.get_sufix() + ".dat";


    fio_cmm.save_data_to_disk<file_npc_v3_1_2>("game_enemy_list_3_1_2.dat", enemy_list);
    fio_cmm.save_data_to_disk<file_object>("game_object_list.dat", object_list);
    fio_cmm.save_data_to_disk<file_artificial_inteligence>("game_ai_list.dat", ai_list);


    //convert_ai_list_to_v3();
    //fio_cmm.save_data_to_disk<file_artificial_inteligence_v3>("game_ai_list_v3.dat", ai_list);

    //convertProjectileListToV2();
    fio_cmm.save_data_to_disk<file_projectilev3>("data/game_projectile_list_v3.dat", projectile_list_v3);

    fio_cmm.save_data_to_disk<file_anim_block>("anim_block_list.dat", anim_block_list);

    fio_cmm.save_data_to_disk<file_player_v3_1_1>("player_list_v3_1_1.dat", player_list_v3_1);


    ScenesMediator::get_instance()->save_game_scenes();


    // FILE V5 //
    fio_cmm.save_data_to_disk<file_v5_map_header>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_HEADER_LIST, SharedData::get_instance()->file_v5_map_header_list);
    fio_cmm.save_data_to_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST, SharedData::get_instance()->file_v5_map_link_list);
    fio_cmm.save_data_to_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST, SharedData::get_instance()->slope_list);
    fio_cmm.save_data_to_disk<struct_file_v5_area>(SharedData::get_instance()->FILEPATH + FILE_V5_AREA_LIST, SharedData::get_instance()->area_list);

    std::cout << "Mediator::save - saving map-tiles for maps[" << SharedData::get_instance()->file_v5_map_header_list.size() << "]" << std::endl;

    for (int i=0; i<SharedData::get_instance()->file_v5_map_header_list.size(); i++) {
        if (SharedData::get_instance()->file_v5_map_tile_map.find(i) != SharedData::get_instance()->file_v5_map_tile_map.end()) {
            QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");
            std::cout << "Mediator::save - saving map-tiles, map[" << i << "] has data, saving with filename[" << filename.toStdString() << "]" << std::endl;
            fio_cmm.save_data_to_disk<file_v5_map_tile>(filename.toStdString(), SharedData::get_instance()->file_v5_map_tile_map.at(i));
        }
    }



}





void Mediator::clean_data()
{

    QString filename;
    if (Mediator::get_instance()->getPallete().length() < 1) {
         filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/") + QString("default.png");
    } else {
         filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/") + QString(Mediator::get_instance()->getPallete().c_str());
    }

    QPixmap *image = new QPixmap(filename);
    if (image->isNull()) {
        printf("DEBUG.Tile - Could not load image file '%s'\n", qPrintable(filename));
    }

    int tileset_w = image->width();
    int tileset_h = image->height();

}



void Mediator::centNumberFormat(int n) {
	if (n >= 100) {
		sprintf(centNumber, "%d\0", n);
	} else if (n >= 10) {
		sprintf(centNumber, "0%d\0", n);
	} else {
		sprintf(centNumber, "00%d\0", n);
	}
	//printf("DEBUG.Mediator::centNumberFormat - centNumber: %s\n", centNumber);
}





int Mediator::get_stage_n(const int map_n) {
	if (map_n < 10) {
		return map_n;
	}
	if (map_n < 19) {
		return map_n-9;
	}
    return map_n-18;
}

void Mediator::reload_game_scenes()
{
    scene_list = fio_scenes.load_scenes();
    if (scene_list.size() == 0) {
        scene_list.push_back(file_scene_list());
    }
}





