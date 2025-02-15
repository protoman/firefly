#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <QString>
#include "file/format.h"
#include "file/file_io.h"
#include "file/fio_scenes.h"
#include "file/fio_strings.h"
#include "file/v5/struct_file_map.h"
#include "file/fio_common.h"
#include "scenes/scenesmediator.h"

#include "file/v6/file_level_v6.h"
#include "file/v6/file_area_v6.h"
#include "file/v6/file_room_v6.h"
#include "file/v0/file_game_v0.h"
#include "game_data.h"

#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "aux_tools/dataconverter.h"

#include <sstream>


struct st_player_graphics_data {
	st_size frame_size;
	st_rectangle hit_area;
	std::string graphics_filename;

	st_player_graphics_data()
	{

		frame_size.width = 29;
        frame_size.height = 29;
		hit_area.x = 4;
		hit_area.y = 8;
		hit_area.w = 21;
		hit_area.h = 24;
		graphics_filename = std::string("p1.png");
	}
};

class Mediator {
public:
    static Mediator* get_instance();


public:
	char gameName[50];
	char centNumber[4];
	int currentGame;
    int currentDifficulty;
    int currentDifficultyMode;
	//QString *NpcPreviewFile;
	char addNpcFilename[512];
	int NpcAddNumber;
	int layerLevel;
	int selectedNPC;
	int editMode;
	int editTool;
	int editModeNPC;
	int object_type;
	int terrainType;
    double zoom;
	int npc_direction;
    int object_direction;
	bool show_background_color;
	bool show_bg1;
    bool show_fg_layer;
    // sprites
    bool playing_sprites;
    int current_sprite_type;
    int current_sprite_selection;
    int current_npc_n;
    int selectedAnimTileset;
    bool show_objects_flag;
    bool show_enemies_flag;
    bool show_npcs_flag = true;
    bool show_teleporters_flag;
    short stage_select_edit_mode;
    bool show_grid;
    std::map<int, std::vector<file_v6_level_point>> v6_level_map; // map a room to a position in the world-map and area-number
    std::vector<st_position> selected_rooms;
    std::map<st_position, std::vector<file_v5_map_room>>  file_v5_room_map; // map rooms


	int getPalleteX();
	int getPalleteY();
	void setPalleteX(int value);
	void setPalleteY(int value);
    std::string getPallete();
    void setPallete(std::string filename);
	void setPallete(char *value);
    void load_game();
    void load_area_rooms(int area_n);
    void save_area_rooms(int stage_n, int area_n);
    void save_game();
    void save_game_json();
    void clean_data();                                      // any sanity data cleaning goest here
    int check_area_links(int room_x, int room_y, int tile_x, int tile_y);
    void save_map_check_area_links();
    void add_missing_area_rooms(int stage_n, int area_n);
    void add_missing_areas();
	int get_stage_n(const int map_n);
    void reload_game_scenes();
    void centNumberFormat(int n);


	int npcGraphicSize_w;
	int npcGraphicSize_h;

	// projectile
	int projectileGraphicSize_w;
	int projectileGraphicSize_h;
	char addProjectileFilename[512];

	bool link_bidi;
	bool link_is_door;



	struct st_player_graphics_data player_graphics_data;

	int picked_color_n;
	int current_weapon;
    int current_projectile;
	int current_player;
    int current_ai;

    // old globals


    std::vector<file_projectilev3> projectile_list_v3;

    std::vector<file_scene_list> scene_list;
    std::vector<file_anim_block> anim_block_list;
    std::vector<file_player_v3_1_1> player_list_v3_1;
    //std::vector<file_player_v3_1_1> player_list_v3_1_1;



    file_io fio;
    fio_scenes fio_scenes;
    std::string combobox_select_string;

    // stage dialogs, key is stage_n, vector represent the list for each text line
    std::vector<std::map<int, std::vector<std::string> > > stage_dialog_list;

private:
    Mediator();
    Mediator(Mediator const&){};             // copy constructor is private
    Mediator& operator=(Mediator const&){};  // assignment operator is private

private:
    static Mediator* _instance;
    int palleteX;
    int palleteY;
    std::string selectedTileset;
    fio_common fio_cmm;
    fio_strings fio_str;
    DataConverter data_converter;
};


#endif // MEDIATOR_H
