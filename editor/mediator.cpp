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
        enemy_list.push_back(file_enemy_v3_1_2());
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
    enemy_list = fio_cmm.load_from_disk<file_enemy_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_enemy_list_3_1_2_b.dat");
    if (enemy_list.size() == 0) {
        enemy_list.push_back(file_enemy_v3_1_2());
    }

    SharedData::get_instance()->npc_list = fio_cmm.load_from_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_npc_list_3_1_2_b.dat");
    if (SharedData::get_instance()->npc_list.size() == 0) {
        SharedData::get_instance()->npc_list.push_back(file_npc_v3_1_2());
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
        SharedData::get_instance()->v6_stage_list = fio_cmm.load_from_disk<file_v6_stage>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST);
    }
    SharedData::get_instance()->v6_level_map.clear();
    unsigned int area_size = SharedData::get_instance()->v6_stage_list.size();
    std::cout << "AREAS.SIZE[" << area_size << "]" << std::endl;
    for (unsigned int area_n=0; area_n<area_size; area_n++) {
        char level_filename[512];
        sprintf(level_filename, "%s/data/v6_level_list_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), area_n);
        std::cout << "level_filename[" << level_filename << "]" << std::endl;

        if (fio.file_exists(level_filename)) {
            std::vector<file_v6_level_point> temp_point_list = fio_cmm.load_from_disk<file_v6_level_point>(level_filename);
            std::vector<file_v6_level_point> point_list;
            // avoid invalid points (fix for changing versions)
            for (unsigned int point_n=0; point_n<temp_point_list.size(); point_n++) {
                if (temp_point_list.at(point_n).stage_number == area_n) {
                    point_list.push_back(temp_point_list.at(point_n));
                } else {
                    std::cout << "WARNING: Invalid area_number[" << temp_point_list.at(area_n).stage_number << "] in point for area[" << area_n << "] data file." << std::endl;
                }
            }


            std::cout << "LOADED area[" << area_n << "] with size[" << point_list.size() << "]" << std::endl;

            SharedData::get_instance()->v6_level_map.insert(std::pair<int, std::vector<file_v6_level_point>>(area_n, point_list));
            // Add single point to avoid empty area for first one
            if (area_n == 0 && SharedData::get_instance()->v6_level_map.at(area_n).size() == 0) {
                file_v6_level_point new_point;
                new_point.stage_number = SharedData::get_instance()->v6_selected_stage;
                new_point.area_number = SharedData::get_instance()->v6_selected_area;
                new_point.x = 0;
                new_point.y = 0;
                SharedData::get_instance()->v6_level_map.at(area_n).push_back(new_point);
            }
        } else {
            std::cout << "level_filename[" << level_filename << "] FILE NOT FOUND" << std::endl;
        }
    }

    load_area_rooms(0);

    std::cout << "DEBUG #4" << std::endl;

    if (SharedData::get_instance()->v6_style_list.size() == 0) {
        SharedData::get_instance()->v6_style_list.push_back(file_v6_style());
    }

    //std::cout << "SharedData::get_instance()->v6_level_list.size[" << SharedData::get_instance()->v6_level_map.size() << "]" << std::endl;
    if (SharedData::get_instance()->v6_stage_list.size() == 0) {
        SharedData::get_instance()->v6_stage_list.push_back(file_v6_stage());
    }

    // FILE V5 //
    SharedData::get_instance()->file_v5_map_link_list = fio_cmm.load_from_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST);
    SharedData::get_instance()->slope_list = fio_cmm.load_from_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST);

    for (int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");


        /// @TODO load area links/

        // load stage objects //
        QString filename_area_objects = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v6_map_") + QString::number(i) + QString("_objects.dat");
        SharedData::get_instance()->file_v6_stage_objects_map.insert(std::pair<unsigned int, std::vector<v6_stage_object>>(i, std::vector<v6_stage_object>()));

        if (fio.file_exists(filename_area_objects.toStdString())) {
            SharedData::get_instance()->file_v6_stage_objects_map.at(i) = fio_cmm.load_from_disk<v6_stage_object>(filename_area_objects.toStdString());
        }


        // stage enemies //
        QString filename_area_enemies = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_enemies.dat");
        SharedData::get_instance()->file_v5_stage_enemy_map.insert(std::pair<unsigned int, std::vector<file_v5_map_npc>>(i, std::vector<file_v5_map_npc>()));
        if (fio.file_exists(filename_area_enemies.toStdString())) {
            SharedData::get_instance()->file_v5_stage_enemy_map.at(i) = fio_cmm.load_from_disk<file_v5_map_npc>(filename_area_enemies.toStdString());
            std::cout << ">> FOUND AREA-ENEMIES FILE, size[" <<  SharedData::get_instance()->file_v6_stage_objects_map.at(i).size() << "] <<" << std::endl;
        }

        // stage npcs //
        QString filename_stage_npcs = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_npcs.dat");
        SharedData::get_instance()->file_v5_stage_enemy_map.insert(std::pair<unsigned int, std::vector<file_v5_map_npc>>(i, std::vector<file_v5_map_npc>()));
        if (fio.file_exists(filename_area_enemies.toStdString())) {
            SharedData::get_instance()->file_v5_stage_enemy_map.at(i) = fio_cmm.load_from_disk<file_v5_map_npc>(filename_area_enemies.toStdString());
            std::cout << ">> FOUND AREA-ENEMIES FILE, size[" <<  SharedData::get_instance()->file_v6_stage_objects_map.at(i).size() << "] <<" << std::endl;
        }

    }

    if (SharedData::get_instance()->v6_stage_list.size() == 0) {
        SharedData::get_instance()->v6_stage_list.push_back(file_v6_stage());
    }

    SharedData::get_instance()->v6_area_map.clear();
    SharedData::get_instance()->add_missing_areas();

    SharedData::get_instance()->v6_area_map.clear();
    for (unsigned int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        char area_filename[512];
        sprintf(area_filename, "%s/%s%d.dat", SharedData::get_instance()->FILEPATH.c_str(), FILE_V6_AREA_LIST_PREFIX, i);
        std::vector<file_v6_area> stage_area_list = fio_cmm.load_from_disk<file_v6_area>(area_filename);
        std::cout << "MEDIATOR::load - v6_area_list[" << i << "].size[" << stage_area_list.size() << "]" << std::endl;
        if (stage_area_list.size() == 0) {
            stage_area_list.push_back(file_v6_area());
        }
        std::pair<int, std::vector<file_v6_area>> new_area_pair(i, stage_area_list);
        SharedData::get_instance()->v6_area_map.insert(new_area_pair);
    }
    SharedData::get_instance()->v6_style_list = fio_cmm.load_from_disk<file_v6_style>(SharedData::get_instance()->FILEPATH + FILE_V6_STYLE_LIST);

}


void Mediator::load_area_rooms(int area_n)
{
    char area_rooms_filename[512];
    sprintf(area_rooms_filename, "%s/data/v6_area_rooms_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), area_n);
    SharedData::get_instance()->v6_area_room_list.clear();
    if (fio.file_exists(area_rooms_filename)) {
        std::vector<file_v6_room> room_list = fio_cmm.load_from_disk<file_v6_room>(area_rooms_filename);
        // convert to a map based upon the world-map position, so we can get all rooms easily
        for (unsigned int i=0; i<room_list.size(); i++) {
            //std::cout << "MEDIATOR::load_area_rooms[" << area_n << "], pos[" << room_list.at(i).position.x << "][" << room_list.at(i).position.y << "]" << std::endl;
            SharedData::get_instance()->v6_area_room_list.insert(std::pair<st_position, file_v6_room>(room_list.at(i).position, room_list.at(i)));
            if (room_list.at(i).area_n == SharedData::get_instance()->v6_selected_area) {
                SharedData::get_instance()->selected_rooms.push_back(room_list.at(i).position);
            }
        }
    }
}

void Mediator::save_area_rooms(int stage_n, int area_n)
{
    char area_rooms_filename[512];
    sprintf(area_rooms_filename, "%s/data/v6_area_rooms_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), stage_n);
    std::vector<file_v6_room> room_list;

    SharedData::get_instance()->add_missing_area_rooms(stage_n, area_n);

    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {
        // FIX - set correct stage and area numbers //
        if (SharedData::get_instance()->v6_level_map.find(stage_n) != SharedData::get_instance()->v6_level_map.end()) {
            for (unsigned int i=0; i<SharedData::get_instance()->v6_level_map.at(stage_n).size(); i++) {
                if (SharedData::get_instance()->v6_level_map.at(stage_n).at(i).x == it->second.position.x && SharedData::get_instance()->v6_level_map.at(stage_n).at(i).y == it->second.position.y) {
                    it->second.area_n = SharedData::get_instance()->v6_level_map.at(stage_n).at(i).area_number;
                }
            }
        }
        room_list.push_back(it->second);
    }
    save_map_check_area_links();
    fio_cmm.save_data_to_disk<file_v6_room>(area_rooms_filename, room_list);
}

void Mediator::save_game()
{
    clean_data();
    //temp_fix_player_colors_order();


    //std::cout << "@@@@@@@@@@@@@@@ Mediator::save_game - game_data.obj_uuid[" << SharedData::get_instance()->game_data.obj_uuid << "]" << std::endl;

    Mediator::get_instance()->fio.write_game(SharedData::get_instance()->game_data);

    std::string stages_extra_data_filename = "data/stages_extra_data" + fio.get_sufix() + ".dat";


    fio_cmm.save_data_to_disk<file_enemy_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_enemy_list_3_1_2_b.dat", enemy_list);
    fio_cmm.save_data_to_disk<file_npc_v3_1_2>(SharedData::get_instance()->FILEPATH + "game_npc_list_3_1_2_b.dat", SharedData::get_instance()->npc_list);
    fio_cmm.save_data_to_disk<v6_file_object>(SharedData::get_instance()->FILEPATH + "game_object_list_v6.dat", SharedData::get_instance()->v6_object_list);
    fio_cmm.save_data_to_disk<file_artificial_inteligence>(SharedData::get_instance()->FILEPATH + "game_ai_list.dat", ai_list);


    //fio_cmm.save_data_to_disk<file_artificial_inteligence_v3>("game_ai_list_v3.dat", ai_list);

    //std::cout << "################### save projectile list size[" << projectile_list_v3.size() << "]" << std::endl;
    fio_cmm.save_data_to_disk<file_projectilev3>(SharedData::get_instance()->FILEPATH + PROJECTILE_FILE_V3, projectile_list_v3);

    fio_cmm.save_data_to_disk<file_anim_block>(SharedData::get_instance()->FILEPATH + "anim_block_list.dat", anim_block_list);

    fio_cmm.save_data_to_disk<file_player_v3_1_1>(SharedData::get_instance()->FILEPATH + "player_list_v3_1_1.dat", player_list_v3_1);


    ScenesMediator::get_instance()->save_game_scenes();


    // FILE V5 //
    fio_cmm.save_data_to_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_LINK_LIST, SharedData::get_instance()->file_v5_map_link_list);
    fio_cmm.save_data_to_disk<file_v5_slope_tile>(SharedData::get_instance()->FILEPATH + FILE_V5_MAP_SLOPE_LIST, SharedData::get_instance()->slope_list);


    //std::cout << "Mediator::save - saving map-tiles for maps[" << SharedData::get_instance()->v6_stage_list.size() << "]" << std::endl;

    for (unsigned int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        QString filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_tiles.dat");
        //std::cout << "Mediator::save - saving map-tiles, map[" << i << "], filename[" << filename.toStdString() << "]" << std::endl;

        // @TODO: save area links //

        // map objects //
        QString filename_area_objects = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v6_map_") + QString::number(i) + QString("_objects.dat");
        if (SharedData::get_instance()->file_v6_stage_objects_map.find(i) != SharedData::get_instance()->file_v6_stage_objects_map.end()) {
            //std::cout << "################ save area[" << i << "] map-objects in [" << filename_area_objects.toStdString() << "]" << std::endl;
            fio_cmm.save_data_to_disk<v6_stage_object>(filename_area_objects.toStdString(), SharedData::get_instance()->file_v6_stage_objects_map.at(i));
        }

        // map enemies //
        QString filename_area_enemies = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/data/v5_map_") + QString::number(i) + QString("_enemies.dat");
        /*
        if (SharedData::get_instance()->file_v5_stage_enemy_map.find(i) != SharedData::get_instance()->file_v5_stage_enemy_map.end()) {
            //std::cout << "################ save area[" << i << "] map-enemies in [" << filename_area_enemies.toStdString() << "]" << std::endl;
            fio_cmm.save_data_to_disk<file_v5_map_npc>(filename_area_enemies.toStdString(), SharedData::get_instance()->file_v5_stage_enemy_map.at(i));
        }
        */
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

    save_area_rooms(SharedData::get_instance()->v6_selected_stage, SharedData::get_instance()->v6_selected_area);
    // FILE-V6 AREAS //
    for (unsigned int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        char level_filename_char[512];
        sprintf(level_filename_char, "%s/data/v6_level_list_%d.dat", SharedData::get_instance()->FILEPATH.c_str(), i);
        if (i == 0 && SharedData::get_instance()->v6_level_map.find(i) == SharedData::get_instance()->v6_level_map.end()) {
            std::vector<file_v6_level_point> point_list;
            SharedData::get_instance()->v6_level_map.insert(std::pair<int, std::vector<file_v6_level_point>>(i, point_list));
            file_v6_level_point new_point;
            new_point.stage_number = 0;
            new_point.x = 0;
            new_point.y = 0;
            SharedData::get_instance()->v6_level_map.at(i).push_back(new_point);
        }
        fio_cmm.save_data_to_disk<file_v6_level_point>(level_filename_char, SharedData::get_instance()->v6_level_map.at(i));
        //std::cout << "SAVED area[" <<i << "] with size[" << SharedData::get_instance()->v6_level_map.at(i).size() << "]" << std::endl;
    }

    fio_cmm.save_data_to_disk<file_v6_stage>(SharedData::get_instance()->FILEPATH + FILE_V6_MAP_LIST, SharedData::get_instance()->v6_stage_list);

    SharedData::get_instance()->add_missing_areas();
    for ( auto it = SharedData::get_instance()->v6_area_map.begin(); it != SharedData::get_instance()->v6_area_map.end(); ++it) {
        char area_filename[512];
        sprintf(area_filename, "%s/%s%d.dat", SharedData::get_instance()->FILEPATH.c_str(), FILE_V6_AREA_LIST_PREFIX, it->first);
        //std::cout << "MEDIATOR::save - v6_area_list[" << it->first << "].size[" << it->second.size() << "]" << std::endl;
        fio_cmm.save_data_to_disk<file_v6_area>(area_filename, it->second);
    }
    fio_cmm.save_data_to_disk<file_v6_style>(SharedData::get_instance()->FILEPATH + FILE_V6_STYLE_LIST, SharedData::get_instance()->v6_style_list);
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

int Mediator::check_area_links(int room_x, int room_y, int tile_x, int tile_y)
{
    st_position current_room_pos = st_position(room_x, room_y);
    int area_edge = 0; // 0 no other area, 1 horizontal, 2 vertical
    // check left
    if (room_x > 0 && tile_x == 0) {
        st_position room_left_pos = st_position(room_x-1, room_y);
        if (SharedData::get_instance()->v6_area_room_list.find(room_left_pos) != SharedData::get_instance()->v6_area_room_list.end()) {
            if (SharedData::get_instance()->v6_area_room_list.at(current_room_pos).area_n != SharedData::get_instance()->v6_area_room_list.at(room_left_pos).area_n) {
                area_edge = 1;
            }
        } else {
            std::cout << "$$$$$$$$$$ Area-Left not found" << std::endl;
        }
    } else if (tile_x == AREA_ROOM_TILES_W-1) {
        st_position room_right_pos = st_position(room_x+1, room_y);
        if (SharedData::get_instance()->v6_area_room_list.find(room_right_pos) != SharedData::get_instance()->v6_area_room_list.end()) {
            if (SharedData::get_instance()->v6_area_room_list.at(current_room_pos).area_n != SharedData::get_instance()->v6_area_room_list.at(room_right_pos).area_n) {
                area_edge = 1;
            }
        } else {
            std::cout << "$$$$$$$$$$ Area-Right not found" << std::endl;
        }
    } else if (room_y > 0 && tile_y == 0) {
        st_position room_top_pos = st_position(room_x, room_y-1);
        if (SharedData::get_instance()->v6_area_room_list.find(room_top_pos) != SharedData::get_instance()->v6_area_room_list.end()) {
            if (SharedData::get_instance()->v6_area_room_list.at(current_room_pos).area_n != SharedData::get_instance()->v6_area_room_list.at(room_top_pos).area_n) {
                area_edge = 2;
            }
        } else {
            std::cout << "$$$$$$$$$$ Area-Top not found" << std::endl;
        }
    } else if (tile_y == AREA_ROOM_TILES_H-1) {
        st_position room_bottom_pos = st_position(room_x, room_y+1);
        if (SharedData::get_instance()->v6_area_room_list.find(room_bottom_pos) != SharedData::get_instance()->v6_area_room_list.end()) {
            if (SharedData::get_instance()->v6_area_room_list.at(current_room_pos).area_n != SharedData::get_instance()->v6_area_room_list.at(room_bottom_pos).area_n) {
                area_edge = 2;
            }
        } else {
            std::cout << "$$$$$$$$$$ Area-Bottom not found" << std::endl;
        }
    }
    return area_edge;
}

void Mediator::save_map_check_area_links()
{
    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {
        if (it->second.area_n == -1) {
            it->second.area_n = 0;
        }
        // top
        int room_x = it->second.position.x;
        int room_y = it->second.position.y;
        st_position current_room_pos = st_position(room_x, room_y);
        st_position adjascrent_room_pos = st_position(room_x, room_y-1);
        std::map<st_position, file_v6_room>::iterator adjascent_room_it = SharedData::get_instance()->v6_area_room_list.find(adjascrent_room_pos);
        if (adjascent_room_it != SharedData::get_instance()->v6_area_room_list.end() && it->second.area_n != adjascent_room_it->second.area_n) {
            for (unsigned int x=0; x<AREA_ROOM_TILES_W; x++) {
                int locked = it->second.tiles[x][0].locked;
                if (locked != TERRAIN_SOLID && locked != TERRAIN_AREA_VSCROLL_LOCK) {
                    it->second.tiles[x][0].locked = TERRAIN_AREA_VSCROLL_LOCK;
                }
            }
        }

        // bottom
        adjascrent_room_pos = st_position(room_x, room_y+1);
        adjascent_room_it = SharedData::get_instance()->v6_area_room_list.find(adjascrent_room_pos);
        if (adjascent_room_it != SharedData::get_instance()->v6_area_room_list.end() && it->second.area_n != adjascent_room_it->second.area_n) {
            for (unsigned int x=0; x<AREA_ROOM_TILES_W; x++) {
                int locked = it->second.tiles[x][AREA_ROOM_TILES_H-1].locked;
                if (locked != TERRAIN_SOLID && locked != TERRAIN_AREA_VSCROLL_LOCK) {
                    it->second.tiles[x][AREA_ROOM_TILES_H-1].locked = TERRAIN_AREA_VSCROLL_LOCK;
                }
            }
        }


        // left
        adjascrent_room_pos = st_position(room_x-1, room_y);
        adjascent_room_it = SharedData::get_instance()->v6_area_room_list.find(adjascrent_room_pos);
        if (adjascent_room_it != SharedData::get_instance()->v6_area_room_list.end() && it->second.area_n != adjascent_room_it->second.area_n) {
            for (unsigned int y=0; y<AREA_ROOM_TILES_H; y++) {
                int locked = it->second.tiles[0][y].locked;
                if (locked != TERRAIN_SOLID && locked != TERRAIN_AREA_HSCROLL_LOCK) {
                    it->second.tiles[0][y].locked = TERRAIN_AREA_HSCROLL_LOCK;
                }
            }
        }

        // right
        adjascrent_room_pos = st_position(room_x+1, room_y);
        adjascent_room_it = SharedData::get_instance()->v6_area_room_list.find(adjascrent_room_pos);
        if (adjascent_room_it != SharedData::get_instance()->v6_area_room_list.end() && it->second.area_n != adjascent_room_it->second.area_n) {
            for (unsigned int y=0; y<AREA_ROOM_TILES_H; y++) {
                int locked = it->second.tiles[AREA_ROOM_TILES_W-1][y].locked;
                if (locked != TERRAIN_SOLID && locked != TERRAIN_AREA_HSCROLL_LOCK) {
                    it->second.tiles[AREA_ROOM_TILES_W-1][y].locked = TERRAIN_AREA_HSCROLL_LOCK;
                }
            }
        }
    }
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





