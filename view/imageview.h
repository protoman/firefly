#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <iostream>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "defines.h"
#include "data/st_common.h"
#include "data/shareddata.h"


struct st_background {
    st_position position;
    st_imageData imageData;
};

struct anim_tile_timer {
    unsigned long timer;                        // timer animation was last executed
    Uint8 frame_pos;                            // current frame animation is
    Uint8 max_frames;                           // maximum number of frames this anim-tile have
    int frames_delay[ANIM_TILE_MAX_FRAMES];     // the delay for each animation frame TBD
    anim_tile_timer(int frames_n, unsigned long time_set) {
        timer = time_set;
        frame_pos = 0;
        max_frames = frames_n;

        /// @TODO, set those times
        for (int i=0; i<ANIM_TILE_MAX_FRAMES; i++) {
            frames_delay[i] = 150;
        }
    }
};


struct st_spriteFrame {
    int delay; // time in milisseconds this frame will be shown /**< TODO */
    st_imageData frameSurface;
    st_spriteFrame() {
        //frameSurface.gSurface = nullptr;
        delay = 20;
    }

    // copy constructor //
    st_spriteFrame (const st_spriteFrame& other)
    {
        if (other.frameSurface.surface != nullptr) {

            frameSurface = other.frameSurface;

        }
        delay = other.delay;
    }

    // assign copy constructor //
    st_spriteFrame& operator= (const st_spriteFrame& other)
    {
        setbuf(stdout, nullptr);

        if (other.frameSurface.surface != nullptr) {
            frameSurface = other.frameSurface;
        }
        delay = other.delay;
    }

    void setDelay(int newDelay)
    {
        delay = newDelay;
    }
};

struct st_char_sprite_data {
    st_spriteFrame frames[CHAR_ANIM_DIRECTION_COUNT][ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT];

    st_char_sprite_data() {
        for (int i=0; i<CHAR_ANIM_DIRECTION_COUNT; i++) {
            for (int j=0; j<ANIM_TYPE_COUNT; j++) {
                for (int k=0; k<ANIM_FRAMES_COUNT; k++) {
                    frames[i][j][k].frameSurface.surface = nullptr;
                }
            }
        }
    }

    // copy constructor //
    st_char_sprite_data (const st_char_sprite_data& other)
    {
        fflush(stdout);
        for (int i=0; i<CHAR_ANIM_DIRECTION_COUNT; i++) {
            for (int j=0; j<ANIM_TYPE_COUNT; j++) {
                for (int k=0; k<ANIM_FRAMES_COUNT; k++) {
                    /// @TODO - copy surface
                }
            }
        }
        fflush(stdout);
    }

    // assign copy constructor //
    st_char_sprite_data& operator= (const st_char_sprite_data& other)
    {
        printf(">> DEBUG.st_char_sprite_data.ASSIGN.START <<\n");
        fflush(stdout);
        for (int i=0; i<CHAR_ANIM_DIRECTION_COUNT; i++) {
            for (int j=0; j<ANIM_TYPE_COUNT; j++) {
                for (int k=0; k<ANIM_FRAMES_COUNT; k++) {
                    /// @TODO - copy surface
                }
            }
        }
        printf(">> DEBUG.st_char_sprite_data.ASSIGN.END <<\n");
        fflush(stdout);
    }
};

class ImageView
{
public:
    void init();
    static ImageView *get_instance();

    void copyScreenAreaToImage(int origin_x, int origin_y, int origin_w, int origin_h, int dest_x, int dest_y, st_imageData image);

    void copyArea(st_imageData &origin, st_imageData &dest);

    void copyArea(struct st_rectangle rect, struct st_position dest_pos, st_imageData &origin, st_imageData &dest);
    void copyArea(struct st_rectangle origin_rect, struct st_rectangle dest_rect, st_imageData &origin, st_imageData &dest);
    void copyArea(struct st_position origin_pos, struct st_imageData& origin, struct st_imageData& dest); // overload, use the whole width and height of given image


    void clear_surface(struct st_imageData& surface);
    st_imageData imageFromFile(std::string filename);
    void renderTexturePortionAt(int origin_x, int origin_y, int origin_w, int origin_h, int dest_x, int dest_y, SDL_Texture* texture);
    void renderImageAt(int dest_x, int dest_y, st_imageData& image);
    void clearScreenArea(short int x, short int y, short int w, short int h, short int r, short int g, short int b);
    void updateScreen();
    st_imageData initSurface(struct st_size size);
    void clear_surface_area(short int x, short int y, short int w, short int h, short int r, short int g, short int b, struct st_imageData& image) const;
    void set_surface_alpha(int alpha, st_imageData &image);
    void update_anim_tiles_timers();
    void place_anim_tile(int anim_tile_id, struct st_position pos_destiny);
    void place_easymode_block_tile(st_position destiny, st_imageData &surface);
    void place_hardmode_block_tile(st_position destiny, st_imageData &surface);
    void placeTile(struct st_position origin_pos, struct st_position dest_pos, st_imageData& dest);
    void place_3rd_level_tile(int origin_x, int origin_y, int dest_x, int dest_y);
    void load_icons();
    void flip_image(st_imageData& original, st_imageData &res, e_flip_type flip_mode);
    void blink_screen(int r, int g, int b);
    struct st_imageData imageFromRegion(struct st_rectangle area, struct st_imageData& origin);
    void set_spriteframe_surface(st_spriteFrame *frame_dest, st_imageData& originSurface);
    void drawCursor(st_position pos);
    void eraseCursor(st_position pos);
    void show_dialog(Uint8 position);
    void show_dialog_button(Uint8 position);
    void place_face(std::string face_file, st_position pos);
    st_position get_dialog_pos() const;
    st_position get_config_menu_pos() const;
    st_size get_config_menu_size();
    void show_btn_a(st_position btn_pos);
    void show_config_bg();

    st_imageData* get_preloaded_image(e_PRELOADED_IMAGES image_n);



    void zoom_image(st_position dest, st_imageData& picture, int smooth);
    void rotate_image(st_imageData& picture, double angle);
    void rotated_from_image(st_imageData& picture, st_imageData& dest, double angle);


    void draw_explosion(st_position pos);
    void draw_weapon_tooltip_icon(short weapon_n, st_position position, bool disabled);




private:
    ImageView();
    ImageView(ImageView const&){};             // copy constructor is private
    ImageView& operator=(ImageView const&){};  // assignment operator is private

    // from graphlib
    void copySDLPortion(st_rectangle original_rect, st_rectangle destiny_rect, SDL_Surface *surfaceOrigin, SDL_Surface *surfaceDestiny);



public:
    std::map<std::string, st_char_sprite_data> character_graphics_list;
    std::map<std::string, st_imageData> character_graphics_background_list;
    // IMAGES // /// @TODO: deve ser private e passar todas as imagens para uma lista+enum //
    st_imageData bomb_explosion_surface;
    st_imageData explosion32;
    std::vector<st_surface_with_direction> projectile_surface;
    st_imageData dash_dust;
    st_imageData water_splash;


private:
    static ImageView* _instance;

    std::vector<struct st_imageData> weapon_icons;		// weapon icons, used in menu and energy bars
    std::map<std::string, st_imageData> FACES_SURFACES;
    std::vector<struct anim_tile_timer> ANIM_TILES_TIMERS;
    std::vector<struct st_imageData> ANIM_TILES_SURFACES;   // hold animated-tiles surface
    // IMAGES //
    st_imageData ingame_menu;
    st_imageData config_menu;
    st_imageData dialog_surface;
    st_imageData _btn_a_surface;
    st_imageData _easymode_block;
    st_imageData _hardmode_block;
    st_imageData tileset;										// we do not put this into a st_imageData because this is meant to be used only internally
    st_imageData water_tile;                                    // transparent blue surface used for water effect
    st_imageData explosion16;
    st_imageData explosion_player_death;
    st_imageData preloaded_images[PRELOADED_IMAGES_COUNT];
    st_imageData small_explosion;


    st_position _dialog_pos;
    st_position _config_menu_pos;
    unsigned int _explosion_animation_timer;
    int _explosion_animation_pos;

};

#endif // IMAGEVIEW_H
