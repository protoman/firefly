#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <iostream>

#include "data/st_common.h"
#include "data/shareddata.h"
#include "view/imageview.h"

#include "character/GameEnemy.h"
#include "objects/GameObject.h"
#include "view/animation.h"
#include "view/draw.h"

struct object_collision {
    int _block;
    GameObject* _object;
    object_collision(int set_block, GameObject* set_object) {
        _block = set_block;
        _object = set_object;
    }
    object_collision() {
        _block = 0;
        _object = nullptr;
    }
};

struct st_layer_pos {
    st_float_position pos;
    bool is_fg;

    st_layer_pos(bool isFg) {
        this->is_fg = isFg;
    }
};


struct water_bubble {
    st_position pos;
    float x_adjust;
    int x_adjust_direction;
    float timer;
};



struct st_level3_tile {
    st_position tileset_pos;
    st_position map_position;
    st_level3_tile(st_position set_tileset_pos, st_position set_map_position)
    {
        tileset_pos = set_tileset_pos;
        map_position = set_map_position;
    }
    st_level3_tile()
    {
        tileset_pos = st_position(0, 0);
        map_position = st_position(0, 0);
    }

};

struct anim_tile_desc {
    int anim_tile_id;
    int dest_x;
    int dest_y;

    anim_tile_desc(int id, st_position pos) {
        anim_tile_id = id;
        dest_x = pos.x;
        dest_y = pos.y;
    }
};

class MapController
{
public:
    MapController();
    void loadMap();
    void show();
    void updated_visited_room();
    int get_level_from_room(int x, int y);

    int collision_rect_player_obj(st_rectangle player_rect, GameObject* temp_obj, const short int x_inc, const short int y_inc, const short obj_xinc, const short obj_yinc);
    int getMapPointLock(st_position pos) const;
    st_float_position getMapScrolling() const;
    st_size get_size();
    file_v6_tile_piece get_map_point_tile1(st_position pos);
    short get_map_point_lock(int tile_x, int tile_y);

    int get_first_lock_on_left(int x_pos);
    int get_first_lock_on_right(int x_pos);
    int get_first_lock_on_bottom(int x_pos, int y_pos);
    int get_first_lock_on_bottom(int x_pos, int y_pos, int w, int h);

    int get_first_bottom_lock(int initialY);
    bool isEdgeRowLocked(int incY, bool first);
    bool isEdgeColumnLocked(int incX, bool first);

    void add_bubble_animation(st_position pos);
    GameEnemy* collision_player_npcs(character*, const short int, const short int);
    st_float_position get_last_scrolled() const;

    // LAYERS //
    void addLayer(unsigned int n, bool isFg);
    void clearLayers();
    void drawLayers(bool isFg);
    void render_layer(float x, float y, st_imageData* surface_bg);

    void draw_map_tiles();

    void draw_animated_tiles();

    void init_animated_tiles();

    void showAbove(int scroll_y=0, int temp_scroll_x = -99999, bool show_fg=true);

    bool is_point_solid(st_position pos) const;


    file_v6_room_tile getTileFromPosition(int x, int y);

    bool get_map_point_wall_lock(int x);

    void changeScrolling(st_float_position pos, bool check_lock=true);

    void incScrollValue(float xinc, float yinc);

    void changeLayerScroll(int x_change, int y_change);




    void reset_scrolled();

    void load_map_objects();


    bool is_obj_ignored_by_enemies(Uint8 obj_type);                 // returns true if object if of type that can be got like energy

    void collision_char_object(character*, const float, const short int);

    object_collision get_obj_collision();


    void collision_player_special_attack(character*, const short int, const short int, short int, short int);

    GameEnemy* find_nearest_npc(st_position pos);

    GameEnemy* find_npc_by_id(int npc_id);

    GameEnemy* find_nearest_npc_on_direction(st_position pos, int direction);

    void clean_map_npcs_projectiles();

    void reset_beam_objects();

    void remove_temp_objects();

    void get_map_area_surface(st_imageData &mapSurface);

    void set_scrolling(st_float_position pos);

    void reset_scrolling();

    void move_map(const short int move_x, const short int move_y);

    void reset_map();

    void set_scroll_to_bottom();

    void add_animation(ANIMATION_TYPES pos_type, st_imageData* surface, const st_float_position &pos, st_position adjust_pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize);

    void add_animation(animation anim);

    void clear_animations(); // remove all animations from map

    GameEnemy *spawn_map_npc(short int npc_id, st_position npc_pos, short direction, bool player_friend, bool progressive_span);

    int child_npc_count(int parent_id);

    void move_npcs();

    void show_npcs();
    void show_npcs_to_left(int x);

    void build_screen_area_object_list();

    void move_objects(bool paused);

    void clean_finished_objects();

    std::vector<GameObject*> check_collision_with_objects(st_rectangle collision_area);

    void show_objects(int adjust_y=0, int adjust_x=0);

    void show_above_objects(int adjust_y=0, int adjust_x=0);

    bool boss_hit_ground(GameEnemy *npc_ref);

    GameEnemy* get_near_boss();

    void reset_map_npcs();

    void draw_dynamic_backgrounds_into_surface(st_imageData &surface);

    void add_object(GameObject obj);

    st_position get_first_lock_in_direction(st_position pos, st_size max_dist, int direction);


    void drop_item(GameEnemy *npc_ref);
    void drop_game_item(int obj_id, int uuid, int x, int y);

    void set_bg_scroll(int scrollx);

    int get_bg_scroll() const;

    void reset_map_timers();
    void reset_enemies_timers();
    void reset_objects_timers();
    void reset_objects_anim_timers();

    void reset_objects(); // restore objects to their original position

    void print_objects_number();


    bool have_player_object();

    bool subboss_alive_on_left(short tileX);

    void finish_object_teleporter(int obj_number);

    void activate_final_boss_teleporter();

    Uint8 get_map_gfx();

    Uint8 get_map_gfx_mode();

    st_float_position get_bg_scroll();
    void set_bg_scroll(st_float_position pos);
    st_rectangle get_player_hitbox();
    bool must_show_static_bg();                                 // method used to prevent showing enemies on transition if showing static-bg

    void reset_map_loaded_flag();



private:
    void load_map_npcs();

    void adjust_dynamic_background_position(unsigned int bg_n);
    void adjust_dynamic_backgrounds_position();

    bool value_in_range(int value, int min, int max) const;

    void create_dynamic_background_surfaces();

    st_imageData* get_dynamic_bg(int n);

    void set_map_enemy_static_background(std::string filename, st_position pos);

    void preload_slope_images();
    void draw_slope_tile(int x, int y, int dest_x, int dest_y);



private:
    std::map<unsigned int, st_background> imageLayerMap;

public:
    std::vector<GameEnemy> map_enemy_list;                                        // vector npcs
    std::vector<GameEnemy> map_enemy_spawn_list;                                  // list of enemyes to be spawned, after added into _npc_list
    std::vector<animation> animation_list;
    // vector teleporters
    // vector objects

private:
    struct st_float_position scroll;
    st_float_position scrolled;                                             // stores the value the map scrolled in this cycle. used for character movement control (it should move taking the scroll in account)

    std::map<unsigned int, st_layer_pos> layerScrollMap;
    st_imageData static_bg;
    st_position static_bg_pos;
    short _platform_leave_counter;
    water_bubble _water_bubble;
    st_rectangle _3rd_level_ignore_area;
    object_collision _obj_collision;
    std::vector<st_level3_tile> level3_tiles;
    std::vector<st_position> level3_water_tiles;
    std::vector<GameObject> object_list;
    // DRAW MEMBERS //
    int _show_map_pos_x;                                                    // this is used to compare the position that the map was drawn last time to the current scrolling to check if map needs to be redrawn
    int _show_map_pos_y;                                                    // this is used to compare the position that the map was drawn last time to the current scrolling to check if map needs to be redrawn
    st_imageData map_screen;                                                // use to avoid having to draw the tilesets each time we update screen
    std::vector<anim_tile_desc> anim_tile_list;                             // list of animated tiles, so we don't need to loop through all tiles when drawing only the animated ones

    std::map<int, st_imageData> slope_image_map;


    // list of objects, enemies and projectiles in screen-area
    // stores the position of the object in the "main" list
    std::vector<int> on_screen_area_object_list;


    // FIL-V6 //
    std::map<st_position, file_v6_room_tile> area_tile_map;
    int map_tiles_w = 0;
    int map_tiles_h = 0;
    bool map_was_reloaded = false;
};

#endif // MAPCONTROLLER_H
