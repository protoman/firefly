#ifndef GAME_H
#define GAME_H

#include <map>
#include <vector>
#include <sstream>
#include <string>

class classPlayer;
class GameEnemy;


#include "character/classplayer.h"
#include "character/GameEnemy.h"
#include "sceneslib.h"
#include "objects/GameObject.h"
#include "class_config.h"
#include "scenes/dialogs.h"
#include "aux_tools/fps_control.h"
#include "class_config.h"
#include "text/npc_dialog_manager.h"

#ifdef PSP
#include "ports/psp/psp_ram.h"
#endif



/**
 * @brief
 *
 */
class GameManager
{
public:
    static GameManager* get_instance();

    void initHardwareLayer();
    void preloadGameData();

    void introScreen();
    void initGame();

    void start_stage_music();

    bool show_game_intro();
    void show_beta_version_warning();
    void show_free_version_warning();
    void show_notice();
    void quick_load_game();
    void set_player_direction(ANIM_DIRECTION dir);
    void show_player_at(int x, int y);
    void draw_player_death(st_position center);
    void update_stage_scrolling();

    void build_screen_area_lists();

    void show_game(bool can_characters_move, bool can_scroll_stage);


    Uint8 getMapPointLock(struct st_position);
    st_size get_map_size();
    st_float_position checkScrolling();

    void horizontal_screen_move(short direction, bool is_door, short tileX, short tileY);
    void vertical_screen_move(short direction, bool is_door, short tileX);

    void show_door_animation();
    void leave_stage();
    void return_to_intro_screen();
    void game_pause();
    void game_unpause();
    bool is_paused();
    void show_ending();
    void draw_explosion(st_position center, bool show_players);
    void show_player();
    void set_player_position(st_position pos);
    void change_player_position(short xinc, short yinc);
    void set_player_anim_type(ANIM_TYPE anim_type);
    st_position get_player_position();
    st_position get_player_center_position();
    st_size get_player_size();
    void set_player_direction(Uint8 direction);
    void map_present_boss(bool show_dialog, bool is_static_boss);
    character* get_player();
    GameObject* get_player_platform();
    void check_player_return_teleport(); // if player is inside a teleporter, killing a boss makes him teleport out
    bool must_show_boss_hp();
    void remove_all_projectiles();
    void reset_beam_objects();
    void remove_temp_objects();
    void remove_players_slide();
    void show_map();
    void set_current_map(unsigned int);
    st_float_position get_current_stage_scroll();
    void reset_scroll();
    short get_drop_item_id(short type);
    void get_drop_item_ids();
    bool show_config(short finished_stage);
    void show_savegame_error();
    void object_teleport_boss(st_position dest_pos, Uint8 dest_map, Uint8 teleporter_id, bool must_return);
    void remove_current_teleporter_from_list(); // used when player dies
    void select_game_screen();
    std::string get_selected_game();
    MapController *get_current_map_obj();
    bool is_player_on_teleporter();
    void show_ability_item_dialog(int ability_n);

    short get_current_save_slot();
    void set_current_save_slot(short n);
    void save_game();
    void read_save();
    void set_show_fps_enabled(bool enabled);
    bool get_show_fps_enabled();
    void add_autoscroll_delay();

    bool check_map_link(int xinc, int yinc);

    void show_at_texture_renderer();

    void show_hud(bool update_room);

    // QUEUE HANDLERS //
    void consume_dialogs_from_queue();
    void add_queue_dialog(st_dialog dialog);

    st_position get_player_relative_center_position();

    st_dialog_status* get_dialog_status();
    std::vector<st_dialog>* get_dialog_queue();
    bool is_special_boss(std::string name);

    void talk_with_npc(int npc_id);
    void morph_player_object(int new_obj_id);
    void remove_player_object();

    bool boss_show_intro_sprites(GameEnemy *npc_ref);


private:
    GameManager();
    ~GameManager();
    GameManager(GameManager const&) : _show_boss_hp(false), player1(0) {};             // copy constructor is private
    GameManager& operator=(GameManager const&){ return *this; };  // assignment operator is private

    void exit_game();
    void start_stage();
    void set_player_position_teleport_in(int initial_pos_x, int initial_pos_y);
    void show_player_teleport(int pos_x, int pos_y);

    void loadGameData();
    void loadAreaRooms(int area_n);
    void loadEnemyStateData();
    void loadMapData();
    void loadAreaListSize();
    int mapNumberFromAreaPosition(int area_n, int x, int y);

    void restart_stage();
    void transition_screen(Uint8 type, Uint8 map_n, short int adjust_x, classPlayer *pObj);
    unsigned int get_current_map();
    int get_current_area();
    void walk_character_to_screen_point_x(character* char_obj, short pos_x); // keeps walking (and jumping obstacles) until reaching a given point in screen (not in map, that should have its own function for that)
    void set_player_teleporter(short set_teleport_n, st_position set_player_pos, bool is_object);

    void finish_player_teleporter();

    void show_stage(int wait_time, bool move_npcs);

    bool subboss_alive_on_left(short tileX);

    void show_mem_debug(int n);
    void change_map_scroll(st_float_position pos, bool check_lock);

    void init_map_and_player_to_bottom();


public:
    st_size calc_area_tile_size(int area_n);



private:
    static GameManager* _instance;
    file_io fio;
    fio_common fio_cmm;


    MapController mapController;
    classPlayer player1;
    scenesLib scenes;
    std::stringstream fps_msg;
    st_position selected_stage;
    class_config config_manager;
    dialogs game_dialogs;
	// framerate timers
    float _frame_duration;
    std::map<short, bool> _last_stage_used_teleporters; // list of used teleportes (they do not work anymore after added to this list)
    used_teleporter _player_teleporter;
    bool _show_boss_hp = false; // after set to true, will keep showing the boss HP bar on screen right side

    short _drop_item_list[DROP_ITEM_COUNT];
    bool invencible_old_value; // used to store flag value in order we don't loose it when setting to true due to temporary "got weapon" invencibility
    bool _dark_mode;                    // on dark mode we only show animation and projectiles

    std::string _selected_game;

    fps_control fps_manager;

    short current_save_slot;

    bool show_fps_enabled = true;

    std::vector<st_position> map_interstage_points;
    long autoscroll_timer = 0;
    int current_area = 0;

    // dialogs queue //
    // @TODO: add into a struct, so you can hold portraits, play music and other options //
    std::vector<st_dialog> dialog_queue;
    st_dialog_status dialog_status;

    bool must_wait_keypress = true;

    e_GAME_INTERRUPT_MODE interrupt_mode = GAME_INTERRUPT_MODE_NONE;

    bool _is_paused = false;

    std::set<std::string> special_bosses_list = {"Rotate Test"};

    npcDialogManager npc_dialog_manager;

#ifdef PSP
    psp_ram _ram_counter;
    //std::cout << "unload_stage::RAM::BF='" << ram_counter.ramAvailable() << "'" << std::endl;
#endif

};

#endif // GAME_H
