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
#include "game_mediator.h"
#include "file/file_io.h"

#define FLASH_POINTS_N 10
#define SNOW_PARTICLES_NUMBER 20



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
    void show_ready();
    void show_bubble(int x, int y);
    void show_teleport_small(int x, int y);
    int show_credits_text(bool can_leave, std::vector<std::string> credit_text);
    int show_credits(bool can_leave);
    void show_unlocked_charsMsg();
    std::vector<std::string> create_engine_credits_text();
    st_imageData* get_object_graphic(int obj_id);
    void remove_object_graphic(int obj_id);
    void show_ingame_warning(std::vector<std::string> message);
    void fade_in_screen(int r, int g, int b, int total_delay);
    void fade_out_screen(int r, int g, int b, int total_delay);
    void add_fade_out_effect(int r, int g, int b);
    void remove_fade_out_effect();
    void pixelate_screen();
    void add_weapon_tooltip(short weapon_n, const st_position &player_pos, const Uint8 &direction);
    void clear_maps_dynamic_background_list();
    void add_dynamic_background(std::string filename, int auto_scroll_mode, st_color bg_color);
    void set_dynamic_bg_alpha(std::string filename, int alpha);
    st_imageData* get_dynamic_background(std::string filename);
    st_imageData* get_dynamic_foreground(std::string filename);
    void show_hud(int hp, int player_n, int selected_weapon, int selected_weapon_value);
    void draw_enery_ball(int value, int x_pos, st_imageData &ball_surface);
    void set_boss_hp(int hp);
    void show_boss_intro_bg();
    void draw_explosion(st_position center_point, int radius, int angle_inc);
    void draw_castle_path(bool instant, st_position initial_point, st_position final_point);
    void draw_castle_point(int x, int y);
    void show_interstage_map_bg(st_position pos);

private:
    draw();
    draw(draw const&){};             // copy constructor is private
    draw& operator=(draw const&){};  // assignment operator is private

    void draw_credit_line(st_imageData& surface, Uint8 initial_line, std::vector<std::string> credit_text);
    void show_rain();
    void show_flash();
    void generate_snow_particles();
    void show_snow_effect();
    void show_train_effect();
    void show_lightingbolt_effect();
    void show_shadow_top_effect();
    void show_inferno_effect();
    void free_inferno_surface();
    st_float_position get_radius_point(st_position center_point, int radius, float angle);
    //void create_dynamic_background_surface(st_imageData& dest_surface, st_imageData& image_surface, int auto_scroll_mode);


private:
    file_io fio;
    static draw* _instance;
    st_imageData boss_intro_bg;
    st_imageData rain_obj;
    unsigned int _effect_timer;
    short int _rain_pos;

    st_imageData flash_obj;
    short int _flash_pos;
    unsigned int _flash_timer;
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


    // USED IN TRAIN EFFECT
    int _train_effect_timer;
    int _train_effect_state;
    Mix_Chunk* _train_sfx;

    // USED IN LIGHTINGBOLT
    int _lightingbolt_effect_timer;
    int _lightingbolt_effect_state;

    Uint8 screen_gfx;
    Uint8 screen_gfx_mode;
    bool flash_effect_enabled;

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


    // used to avoid having multiple copies of same background for all 3 maps in same stage
    std::map<std::string, st_imageData> maps_dynamic_background_list;

    std::map<e_INPUT_IMAGES, st_imageData> input_images_map;

};

#endif // DRAW_H
