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
	object_type = 3;
	npc_direction = 0;
    object_direction = 0;

    zoom = 0.5;
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
    if (SharedData::get_instance()->v6_object_list.size() == 0) { // add one first item to avoid errors
        SharedData::get_instance()->v6_object_list.push_back(v6_file_object());
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
    selectedTileset =  filename;
}

void Mediator::setPallete(char *value) {
	selectedTileset = value;
}


void Mediator::load_game() {

    std::cout << ">>>>>>>>>>>> Mediator::load_game" << std::endl << std::flush;
    fio.read_game(SharedData::get_instance()->game_data);

    std::string stages_extra_data_filename = "data/stages_extra_data" + fio.get_sufix() + ".dat";

    // convert enemy-ist to 3.1.2
    enemy_list = fio_cmm.load_from_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_enemy_list_3_1_2_b.dat");
    if (enemy_list.size() == 0) {
        enemy_list.push_back(file_npc_v3_1_2());
    }

    SharedData::get_instance()->v6_object_list = fio_cmm.load_from_disk<v6_file_object>(SharedData::get_instance()->FILEPATH + "game_object_list_v6.dat");
    if (SharedData::get_instance()->v6_object_list.size() == 0) { // add one first item to avoid errors
        SharedData::get_instance()->v6_object_list.push_back(v6_file_object());
    }

    ai_list = fio_cmm.load_from_disk<file_artificial_inteligence>(SharedData::get_instance()->FILEPATH + "game_ai_list.dat");
    //std::cout << "MEDIATOR::load_game::ai_list.size(): " << ai_list.size() << std::endl;
    if (ai_list.size() == 0) { // add one first item to avoid errors
        for (int i=0; i<enemy_list.size(); i++) {
            ai_list.push_back(file_artificial_inteligence());
        }
    }


    projectile_list_v3 = fio_cmm.load_from_disk<file_projectilev3>(SharedData::get_instance()->FILEPATH + PROJECTILE_FILE_V3);
    if (projectile_list_v3.size() == 0) {
        projectile_list_v3.push_back(file_projectilev3());
    }

    scene_list = fio_scenes.load_scenes();
    if (scene_list.size() == 0) {
        scene_list.push_back(file_scene_list());
    }

    anim_block_list = fio_cmm.load_from_disk<file_anim_block>(SharedData::get_instance()->FILEPATH + "anim_block_list.dat");

    player_list_v3_1 = fio_cmm.load_from_disk<file_player_v3_1_1>(SharedData::get_instance()->FILEPATH + "player_list_v3_1_1.dat");
    if (player_list_v3_1.size() == 0) {
        for (int i=0; i<FS_MAX_PLAYERS; i++) {
            player_list_v3_1.push_back(file_player_v3_1_1(i));
        }
    }

    ScenesMediator::get_instance()->load_game_scenes();

    if (fio.file_exists(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST)) {
        SharedData::get_instance()->v6_area_list = fio_cmm.load_from_disk<file_v6_area>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST);
    }
    SharedData::get_instance()->v6_level_map.clear();
    std::cout << "AREAS.SIZE[" << SharedData::get_instance()->v6_area_list.size() << "]" << std::endl;
    for (int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        char level_filename[512];
        sprintf(level_filename, "%s/data/v6_level_list_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), i);

        std::cout << "level_filename[" << level_filename << "]" << std::endl;

        if (fio.file_exists(level_filename)) {
            std::vector<file_v6_level_point> point_list = fio_cmm.load_from_disk<file_v6_level_point>(level_filename);
            std::cout << "LOADED area[" << i << "] with size[" << point_list.size() << "]" << std::endl;
            SharedData::get_instance()->v6_level_map.insert(std::pair<int, std::vector<file_v6_level_point>>(i, point_list));
            if (SharedData::get_instance()->v6_level_map.at(i).size() == 0) {
                file_v6_level_point new_point;
                new_point.area_number = 0;
                new_point.x = 0;
                new_point.y = 0;
                SharedData::get_instance()->v6_level_map.at(i).push_back(new_point);
            }
        } else {
            std::cout << "level_filename[" << level_filename << "] FILE NOT FOUND" << std::endl;
        }
    }

    load_area_rooms(0);

    std::cout << "DEBUG #4" << std::endl;

    std::cout << "SharedData::get_instance()->v6_level_list.size[" << SharedData::get_instance()->v6_level_map.size() << "]" << std::endl;
    if (SharedData::get_instance()->v6_area_list.size() == 0) {
        SharedData::get_instance()->v6_area_list.push_back(file_v6_area());
    }

    // FILE V5 //
    SharedData::get_instance()->file_v5_map_link_list = fio_cmm.load_from_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST);
    SharedData::get_instance()->slope_list = fio_cmm.load_from_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST);

    for (int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");


        /// @TODO load area links/

        // load map objects //
        QString filename_area_objects = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v6_map_") + QString::number(i) + QString("_objects.dat");
        SharedData::get_instance()->file_v6_map_object_map.insert(std::pair<unsigned int, std::vector<v6_map_object>>(i, std::vector<v6_map_object>()));

        if (fio.file_exists(filename_area_objects.toStdString())) {
            SharedData::get_instance()->file_v6_map_object_map.at(i) = fio_cmm.load_from_disk<v6_map_object>(filename_area_objects.toStdString());
        }


        // map enemies //
        QString filename_area_enemies = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_enemies.dat");
        SharedData::get_instance()->file_v5_map_npc_map.insert(std::pair<unsigned int, std::vector<file_v5_map_npc>>(i, std::vector<file_v5_map_npc>()));
        if (fio.file_exists(filename_area_enemies.toStdString())) {
            SharedData::get_instance()->file_v5_map_npc_map.at(i) = fio_cmm.load_from_disk<file_v5_map_npc>(filename_area_enemies.toStdString());
            std::cout << ">> FOUND AREA-ENEMIES FILE, size[" <<  SharedData::get_instance()->file_v6_map_object_map.at(i).size() << "] <<" << std::endl;
        }

    }

    if (SharedData::get_instance()->v6_area_list.size() == 0) {
        SharedData::get_instance()->v6_area_list.push_back(file_v6_area());
    }


}

void Mediator::load_area_rooms(int area_n)
{
    char area_rooms_filename[512];
    sprintf(area_rooms_filename, "%s/data/v6_area_rooms_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), area_n);
    if (fio.file_exists(area_rooms_filename)) {
        std::vector<file_v6_room> room_list = fio_cmm.load_from_disk<file_v6_room>(area_rooms_filename);
        // convert to a map based upon the world-map position, so we can get all rooms easily
        SharedData::get_instance()->v6_area_room_list.clear();
        for (int i=0; i<room_list.size(); i++) {
            SharedData::get_instance()->v6_area_room_list.insert(std::pair<st_position, file_v6_room>(room_list.at(i).position, room_list.at(i)));
        }
    }

}

void Mediator::save_area_rooms(int area_n)
{
    char area_rooms_filename[512];
    sprintf(area_rooms_filename, "%s/data/v6_area_rooms_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), area_n);
    std::vector<file_v6_room> room_list;

    SharedData::get_instance()->add_missing_area_rooms(area_n);

    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {
        room_list.push_back(it->second);
    }
    fio_cmm.save_data_to_disk<file_v6_room>(area_rooms_filename, room_list);
}

void Mediator::save_game()
{
    clean_data();
    //temp_fix_player_colors_order();


    std::cout << "@@@@@@@@@@@@@@@ Mediator::save_game - game_data.obj_uuid[" << SharedData::get_instance()->game_data.obj_uuid << "]" << std::endl;

    Mediator::get_instance()->fio.write_game(SharedData::get_instance()->game_data);




    std::string stages_extra_data_filename = "data/stages_extra_data" + fio.get_sufix() + ".dat";


    fio_cmm.save_data_to_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_enemy_list_3_1_2_b.dat", enemy_list);
    fio_cmm.save_data_to_disk<v6_file_object>(SharedData::get_instance()->FILEPATH + "game_object_list_v6.dat", SharedData::get_instance()->v6_object_list);
    fio_cmm.save_data_to_disk<file_artificial_inteligence>(SharedData::get_instance()->FILEPATH + "game_ai_list.dat", ai_list);


    //fio_cmm.save_data_to_disk<file_artificial_inteligence_v3>("game_ai_list_v3.dat", ai_list);

    std::cout << "################### save projectile list size[" << projectile_list_v3.size() << "]" << std::endl;
    fio_cmm.save_data_to_disk<file_projectilev3>(SharedData::get_instance()->FILEPATH + PROJECTILE_FILE_V3, projectile_list_v3);

    fio_cmm.save_data_to_disk<file_anim_block>(SharedData::get_instance()->FILEPATH + "anim_block_list.dat", anim_block_list);

    fio_cmm.save_data_to_disk<file_player_v3_1_1>(SharedData::get_instance()->FILEPATH + "player_list_v3_1_1.dat", player_list_v3_1);


    ScenesMediator::get_instance()->save_game_scenes();


    // FILE V5 //
    fio_cmm.save_data_to_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST, SharedData::get_instance()->file_v5_map_link_list);
    fio_cmm.save_data_to_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST, SharedData::get_instance()->slope_list);


    std::cout << "Mediator::save - saving map-tiles for maps[" << SharedData::get_instance()->v6_area_list.size() << "]" << std::endl;

    for (unsigned int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");
        std::cout << "Mediator::save - saving map-tiles, map[" << i << "], filename[" << filename.toStdString() << "]" << std::endl;

        // @TODO: save area links //

        // map objects //
        QString filename_area_objects = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v6_map_") + QString::number(i) + QString("_objects.dat");
        if (SharedData::get_instance()->file_v6_map_object_map.find(i) != SharedData::get_instance()->file_v6_map_object_map.end()) {
            std::cout << "################ save area[" << i << "] map-objects in [" << filename_area_objects.toStdString() << "]" << std::endl;
            fio_cmm.save_data_to_disk<v6_map_object>(filename_area_objects.toStdString(), SharedData::get_instance()->file_v6_map_object_map.at(i));
        }

        // map enemies //
        QString filename_area_enemies = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_enemies.dat");
        if (SharedData::get_instance()->file_v5_map_npc_map.find(i) != SharedData::get_instance()->file_v5_map_npc_map.end()) {
            std::cout << "################ save area[" << i << "] map-enemies in [" << filename_area_enemies.toStdString() << "]" << std::endl;
            fio_cmm.save_data_to_disk<file_v5_map_npc>(filename_area_enemies.toStdString(), SharedData::get_instance()->file_v5_map_npc_map.at(i));
        }
    }

    // MAP ROOMS - serialize data to save//
    std::vector<file_v5_map_room_data> serialized_room_data;
    std::map<st_position, std::vector<file_v5_map_room>>::iterator it;
    for (it = SharedData::get_instance()->file_v5_room_map.begin(); it != SharedData::get_instance()->file_v5_room_map.end(); it++) {
        for (unsigned int i=0; i<it->second.size(); i++) {
            file_v5_map_room_data item;
            item.area = it->first.x;
            item.map = it->first.y;
            item.area_x = it->second[i].area_x;
            item.area_y = it->second[i].area_y;
            item.room = it->second[i];
            serialized_room_data.push_back(item);
        }
    }
    fio_cmm.save_data_to_disk<file_v5_map_room_data>(SharedData::get_instance()->FILEPATH + FILE_V5_ROOM_LIST, serialized_room_data);

    save_area_rooms(SharedData::get_instance()->v6_selected_area);
    // FILE-V6 //
    for (int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        char level_filename_char[512];
        sprintf(level_filename_char, "%s/data/v6_level_list_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), i);
        if (SharedData::get_instance()->v6_level_map.find(i) == SharedData::get_instance()->v6_level_map.end()) {
            std::vector<file_v6_level_point> point_list;
            SharedData::get_instance()->v6_level_map.insert(std::pair<int, std::vector<file_v6_level_point>>(i, point_list));
            file_v6_level_point new_point;
            new_point.area_number = 0;
            new_point.x = 0;
            new_point.y = 0;
            SharedData::get_instance()->v6_level_map.at(i).push_back(new_point);
        }
        fio_cmm.save_data_to_disk<file_v6_level_point>(level_filename_char, SharedData::get_instance()->v6_level_map.at(i));
        std::cout << "SAVED area[" <<i << "] with size[" << SharedData::get_instance()->v6_level_map.at(i).size() << "]" << std::endl;
    }
    fio_cmm.save_data_to_disk<file_v6_area>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST, SharedData::get_instance()->v6_area_list);

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





