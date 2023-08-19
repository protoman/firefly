#ifndef DRAW_H
#define DRAW_H

#include <iostream>
#include <string>

#include "view/imageview.h"
#include "view/soundview.h"
#include "view/timerview.h"
#include "controller/inputcontroller.h"
#include "data/st_common.h"
#include "data/shareddata.h"
#include "game_data.h"
#include "file/file_io.h"

#define FLASH_POINTS_N 10
#define SNOW_PARTICLES_NUMBER 20

#define HUD_CENTER_BLINK_TIMER 400
#define WATER_ANIMATION_FRAME_TIME 20

struct st_snow_particle {
    st_float_position position;
    float speed;                    // vertical speed. horizontal speed is the same for all
    float x_dist;                   // used to check how many pixels were moved in balance to change direction
    short direction;                // if balancing left or right
    st_snow_particle(st_float_position pos, float spd)
    {
        position = pos;
        speed = spd;
        x_dist = 0;
        if ((int)position.x % 2 == 0) {
            direction = ANIM_DIRECTION_LEFT;
        } else {
            direction = ANIM_DIRECTION_RIGHT;
        }
    }
};


struct st_draw_game_button {
    float x = 0;
    float y = 0;
    e_INPUT_IMAGES button = INPUT_IMAGES_Y;
};

class draw
{
public:
    static draw* get_instance();

    void preload();
    void show_gfx();
    st_imageData* get_input_surface(e_INPUT_IMAGES input);
    void update_screen(); // replaces external calls to ImageView::get_instance()->updateScreen
    void set_gfx(Uint8 gfx, short mode);
    Uint8 get_gfx();
    void set_flash_enabled(bool enabled);
    void show_bubble(int x, int y);
    void show_teleport_small(int x, int y);
    int show_credits_text(bool can_leave, std::vector<std::string> credit_text);
    int show_credits(bool can_leave);
    void show_unlocked_charsMsg();
    std::vector<std::string> create_engine_credits_text();
    st_imageData* get_object_graphic(int obj_id);
    void show_object_graphic(int x, int y, int obj_id);
    void remove_object_graphic(int obj_id);
    void show_ingame_warning(st_dialog dialog);
    void fade_in_screen(int r, int g, int b, int total_delay);
    void fade_screen(int r, int g, int b, int total_delay, bool reverse);
    void add_fade_out_effect(int r, int g, int b);
    void remove_fade_out_effect();
    void pixelate_screen();
    void add_weapon_tooltip(short weapon_n, const st_position &player_pos, const Uint8 &direction);
    void clear_maps_dynamic_background_list();
    void add_dynamic_background(std::string filename, int auto_scroll_mode, st_color bg_color);
    void set_dynamic_bg_alpha(std::string filename, int alpha);
    st_imageData* get_dynamic_background(std::string filename);
    st_imageData* get_dynamic_foreground(std::string filename);

    void show_hud(int hp, int player_n, int selected_weapon, int selected_weapon_value, int room_n_x, int room_n_y);
    void show_hud_items();


    void draw_enery_bars(int value, int x_pos, int y_pos, int type);
    void set_boss_hp(int hp);
    void show_boss_intro_bg();
    void draw_explosion(st_position center_point, int radius, int angle_inc);
    void draw_castle_path(bool instant, st_position initial_point, st_position final_point);
    void draw_castle_point(int x, int y);
    void show_interstage_map_bg(st_position pos);
    void draw_in_game_menu_bg(int screen);

    void draw_in_game_menu_animation();

    void draw_in_game_menu_map();
    void draw_game_button(int x, int y, e_INPUT_IMAGES button);

    void show_dialog(Uint8 position);
    void show_dialog_button(Uint8 position);
    st_position get_dialog_pos() const;
    void show_dialogs_from_queue();

    void draw_water_tile_overlay(int x, int y);

    void draw_player_death(st_position center_point, int frame_n);
    int get_death_animation_frames_n();




private:
    draw();
    draw(draw const&){};             // copy constructor is private
    draw& operator=(draw const&){ return *this; };  // assignment operator is private

    void draw_credit_line(st_imageData& surface, Uint8 initial_line, std::vector<std::string> credit_text);
    void show_rain();
    void show_flash();
    void generate_snow_particles();
    void show_snow_effect();
    void show_train_effect();
    void show_lightingbolt_effect();
    void show_shadow_top_effect();
    void show_inferno_effect();
    void show_dark_effect();
    void show_tint_screen_effect();
    void free_inferno_surface();
    st_float_position get_radius_point(st_position center_point, int radius, float angle);
    //void create_dynamic_background_surface(st_imageData& dest_surface, st_imageData& image_surface, int auto_scroll_mode);


private:
    file_io fio;
    static draw* _instance;
    st_imageData boss_intro_bg;
    st_imageData rain_obj;
    st_imageData dark_effect_light_source_mask;
    st_imageData yellow_light_mask;
    st_imageData red_light_mask;
    unsigned int _effect_timer = 0;
    short int _rain_pos = 0;

    st_imageData flash_obj;
    short int _flash_pos = 0;
    unsigned int _flash_timer = 0;
    st_position flash_points[FLASH_POINTS_N];

    st_imageData _bubble_gfx;
    // USED IN CREDITS
    st_imageData _teleport_small_gfx;
    int teleport_small_frame_count;
    int teleport_small_frame;
    long teleport_small_frame_timer;

    // GRAPHICS LISTS
    std::map<unsigned int, st_imageData> objects_sprite_list; // object_id, graphic

    // USED IN SNOW EFFECT
    std::vector<st_snow_particle> _snow_particles;
    st_imageData snow_flacke;
    st_imageData shadow_line;
    st_imageData _inferno_surface;
    int _inferno_alpha;
    short _inferno_alpha_mode; // 0 increasing, 1 decreasing
    st_imageData dark_effect_surface;

    st_imageData dark_effect_surface2;


    // USED IN TRAIN EFFECT
    int _train_effect_timer;
    int _train_effect_state;
    Mix_Chunk* _train_sfx;

    // USED IN LIGHTINGBOLT
    int _lightingbolt_effect_timer;
    int _lightingbolt_effect_state;

    Uint8 screen_gfx = SCREEN_GFX_NONE;
    Uint8 screen_gfx_mode;
    bool flash_effect_enabled = false;

    // used in HUD
    st_imageData hud_player_hp_ball;
    st_imageData hud_player_wpn_ball;
    st_imageData hud_player_1up;
    st_imageData hud_boss_hp_ball;
    int _boss_current_hp;

    // WEAPON ICON TOOLTIP
    const st_position* _weapon_tooltip_pos_ref;         // holds a pointer to the position it must follow in dynamic animation type
    int _weapon_tooltip_timer;                          // timer, if less than current time, will show the tooltip
    short _weapon_tooltip_n;                            // weapon number to show
    const Uint8* _weapon_tooltip_direction_ref;         // reference to player direction, so we can draw the icon in the correct position
    st_float_position *_map_scroll_ref;                 // reference to map scroll, so we can adjust position
    int current_alpha;                                  // used for fade effect that runs each time update-screen is called
    st_color current_alpha_color;
    st_imageData current_alpha_surface;
    st_imageData castle_point;

    st_imageData interstage_map;

    st_imageData hud_image;
    st_imageData hud_energy_bar;

    st_imageData water_tile_overlay;


    // used to avoid having multiple copies of same background for all 3 maps in same stage
    std::map<std::string, st_imageData> maps_dynamic_background_list;

    std::map<e_INPUT_IMAGES, st_imageData> input_images_map;
    st_imageData in_game_menu_bg;
    st_imageData in_game_menu_bg_clean;

    st_imageData door_h, door_v;
    long timer_hud_center = 0;
    bool timer_hud_center_show = false;
    st_position in_game_menu_map_pos;

    std::vector<st_color> level_color_list;

    st_draw_game_button draw_game_button_request;

    // DIALOG //
    st_imageData dialog_surface;
    st_position _dialog_pos;

    int water_animation_pos = 0;
    long water_animation_timer = 0;

    st_imageData _death_animation;
    int _death_animation_frames_n = 0;

    unsigned long flame_light_timer = 0;
    int flame_light_state = 0;
    bool flame_light_expanding = true;
};

#endif // DRAW_H
