#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#ifdef ANDROID
#include <android/log.h>
#endif

#define RAIN_DELAY 160
#define FLASH_DELAY 260
#define FLASH_IMG_SIZE 8
#define SNOW_DELAY 40
#define LIGHTINGBOLT_DELAY1 4600
#define LIGHTINGBOLT_DELAY2 80
#define TRAIN_DELAY 2000
#define TRAIN_EFFECT_DELAY 80
#define TRAIN_EFFECT_SCREEN_MOVE 1
#define INFERNO_ALPHA_STEP 3
#define STARS_DELAY 50
#define CASTLE_PATH_DURATION 1000

//#include "strings_map.h"

#define FADE_INC 2

#include "file/file_io.h"

#include "GameManager.h"
#include "view/timerview.h"
#include "view/textview.h"
#include "view/ingame_presentation.h"
#include "options/pausemenu.h"

#include "game_data.h"
#include "file/v5/struct_file_game_area_map.h"

Draw* Draw::_instance = nullptr;

Draw::Draw()
{
    for (int i=0; i<FLASH_POINTS_N; i++) {
        flash_points[i].x = rand() % RES_W;
        flash_points[i].y = rand() % AREA_H;
    }
    _weapon_tooltip_timer = 0;
    _weapon_tooltip_n = 0;
    _weapon_tooltip_pos_ref = nullptr;
    _weapon_tooltip_direction_ref = nullptr;
    _train_effect_timer = 0;
    _train_effect_state = 0;
    _train_sfx = nullptr;
    _lightingbolt_effect_timer = 0;
    _lightingbolt_effect_state = 0;
    // INFERNO EFFECT //
    _inferno_alpha = 0;
    _inferno_alpha_mode = 0;
    _boss_current_hp = -99;
    current_alpha = -1;
    teleport_small_frame_count = 1;
    teleport_small_frame = 0;
    teleport_small_frame_timer = 0;

    // level colors
    level_color_list.push_back(st_color(186, 19, 192));
    level_color_list.push_back(st_color(17, 150, 17));
    level_color_list.push_back(st_color(252, 71, 1));

}

Draw *Draw::get_instance()
{
    if (!_instance) {
        _instance = new Draw();
    }
    return _instance;
}

void Draw::preload()
{
    std::string filename = SharedData::get_instance()->GAMEPATH + std::string("shared/images/teleport_small.png");
    _teleport_small_gfx = ImageView::get_instance()->imageFromFile(filename);
    teleport_small_frame_count = _teleport_small_gfx.surface->w/_teleport_small_gfx.surface->h;
    if (teleport_small_frame_count <1) {
        teleport_small_frame_count = 1;
    }

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/snowflacke.png";
    snow_flacke = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/rain.png";
    rain_obj = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/black_line.png";
    shadow_line = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/boss_intro_bg.png";
    boss_intro_bg = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/dark_effect_mask.png";
    dark_effect_light_source_mask = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/yellow_light_mask.png";
    yellow_light_mask = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/red_light_mask.png";
    red_light_mask = ImageView::get_instance()->imageFromFile(filename);


    ImageView::get_instance()->init_target_image(dark_effect_surface, RES_W, AREA_H);
    ImageView::get_instance()->clear_texture_area(0, 0, RES_W, RES_H, 0, 100, 0, 55, dark_effect_surface);


    // DROPABLE OBJECT GRAPHICS
    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        for (int j=0; j<DROP_ITEM_COUNT; j++) {
            short obj_type_n = GameManager::get_instance()->get_drop_item_id(j);
            if (obj_type_n != -1) {
                get_object_graphic(obj_type_n);
            }
        }
    }

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/hp_ball.png";
    hud_player_hp_ball = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/wpn_ball.png";
    hud_player_wpn_ball = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/boss_hp_ball.png";
    hud_boss_hp_ball = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "/images/1up_icons.png";
    hud_player_1up = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/castle_point.png";
    castle_point = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/d_pad.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_DPAD_LEFTRIGHT, st_imageData()));
    input_images_map.at(INPUT_IMAGES_DPAD_LEFTRIGHT) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/btn_a.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_A, st_imageData()));
    input_images_map.at(INPUT_IMAGES_A) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/btn_b.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_B, st_imageData()));
    input_images_map.at(INPUT_IMAGES_B) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/btn_x.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_X, st_imageData()));
    input_images_map.at(INPUT_IMAGES_X) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/btn_y.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_Y, st_imageData()));
    input_images_map.at(INPUT_IMAGES_Y) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/d_pad_up.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_DPAD_UP, st_imageData()));
    input_images_map.at(INPUT_IMAGES_DPAD_UP) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/d_pad_down.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_DPAD_DOWN, st_imageData()));
    input_images_map.at(INPUT_IMAGES_DPAD_DOWN) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/d_pad_left.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_DPAD_LEFT, st_imageData()));
    input_images_map.at(INPUT_IMAGES_DPAD_LEFT) = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/buttons/d_pad_right.png";
    input_images_map.insert(std::pair<e_INPUT_IMAGES, st_imageData>(INPUT_IMAGES_DPAD_RIGHT, st_imageData()));
    input_images_map.at(INPUT_IMAGES_DPAD_RIGHT) = ImageView::get_instance()->imageFromFile(filename);



    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/map.png";
    interstage_map = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/hud.png";
    hud_image = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/energy_bars.png";
    hud_energy_bar = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/ingame_menu_002.png";
    in_game_menu_bg = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/ingame_menu_002_clean.png";
    in_game_menu_bg_clean = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/hud/door_h.png";
    door_h = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/hud/door_v.png";
    door_v = ImageView::get_instance()->imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/dialog.png";
    dialog_surface = ImageView::get_instance()->imageFromFile(filename);
    ImageView::get_instance()->set_surface_alpha(200, dialog_surface);

    filename = SharedData::get_instance()->GAMEPATH + "/shared/images/water_animation.png";
    water_tile_overlay = ImageView::get_instance()->imageFromFile(filename);
    ImageView::get_instance()->set_surface_alpha(160, water_tile_overlay);

    filename = SharedData::get_instance()->GAMEPATH + "shared/images/death_animation.png";
    _death_animation = ImageView::get_instance()->imageFromFile(filename);
    _death_animation_frames_n = _death_animation.surface->w/_death_animation.surface->h;

}

void Draw::show_gfx()
{
    //std::cout << "screen_gfx[" << (int)screen_gfx << "]" << std::endl;

    //screen_gfx = SCREEN_GFX_DARK;

    if (screen_gfx == SCREEN_GFX_RAIN) {
        show_rain();
    } else if (screen_gfx == SCREEN_GFX_SNOW) {
        show_snow_effect();
    } else if (screen_gfx == SCREEN_GFX_TRAIN) {
        show_train_effect();
    } else if (screen_gfx == SCREEN_GFX_LIGHTINGBOLT) {
        show_lightingbolt_effect();
    } else if (screen_gfx == SCREEN_GFX_SHADOW_TOP) {
        show_shadow_top_effect();
    } else if (screen_gfx == SCREEN_GFX_INFERNO) {
        show_inferno_effect();
    } else if (screen_gfx == SCREEN_GFX_DARK) {
        show_dark_effect();
        //show_tint_screen_effect();
    } else if (screen_gfx != SCREEN_GFX_NONE) {
        std::cout << "UNKNOWN screen_gfx[" << (int)screen_gfx << "] UNKNOWN" << std::endl;
    }

    if (flash_effect_enabled == true || screen_gfx == SCREEN_GFX_FLASH) {
        show_flash();
    }
}

st_imageData *Draw::get_input_surface(e_INPUT_IMAGES input)
{
    return &input_images_map.at(input);
}

// show GFX or things added to pipeline //
void Draw::update_screen()
{

    if (GameManager::get_instance()->is_paused() == false) {
        show_gfx();
    }

    ImageView::get_instance()->change_render_target(RENDER_TARGET_DIRECT_SCREEN);
    // show game-texture
    ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W*ImageView::get_instance()->get_scale(), AREA_H*ImageView::get_instance()->get_scale(), RES_W-(RES_W*ImageView::get_instance()->get_scale()), AREA_H-(AREA_H*ImageView::get_instance()->get_scale()), ImageView::get_instance()->get_game_texture());

    // show hud-texture
    if (GameManager::get_instance()->is_paused() == false) {
        ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, HUD_H, 0, AREA_H, ImageView::get_instance()->get_hud_texture());
    }


    // show overlay effects
    if (current_alpha != -1) {
        if (current_alpha < 254) {
            current_alpha += 2;
        }
        if (current_alpha_surface.surface->w < RES_W) {
            current_alpha_surface = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
            ImageView::get_instance()->clear_surface_area(0, 0, RES_W, RES_H, current_alpha_color.r, current_alpha_color.g, current_alpha_color.b, current_alpha_surface);
        } else {
            ImageView::get_instance()->set_surface_alpha(current_alpha, current_alpha_surface);
            ImageView::get_instance()->renderTexturePortionAt(0, 0, current_alpha_surface.surface->w, current_alpha_surface.surface->h, 0, 0, current_alpha_surface.texture);
        }
    }
    // TODO: make this into a vector and generic with position and image //
    if (draw_game_button_request.x >= 0 && draw_game_button_request.y >= 0) {
        ImageView::get_instance()->renderTexturePortionAt(0, 0, input_images_map.at(draw_game_button_request.button).surface->w, input_images_map.at(draw_game_button_request.button).surface->h, draw_game_button_request.x, draw_game_button_request.y, input_images_map.at(draw_game_button_request.button).texture);
        draw_game_button_request.x = -1;
        draw_game_button_request.y = -1;
    }


    //std::cout << "draw::update_screen - queue_size[" << GameManager::get_instance()->get_dialog_queue()->size() << "]" << std::endl;
    if (GameManager::get_instance()->get_dialog_queue()->size() > 0 && InGamePresentation::get_instance()->is_showing_ready() == false) {
        show_dialogs_from_queue(); // TODO: update screen should not be executed anywere else
    }

    InGamePresentation::get_instance()->execute_ingame_presentation();

    ImageView::get_instance()->updateRender();
}

void Draw::set_gfx(Uint8 gfx, short mode)
{
    screen_gfx = gfx;
    screen_gfx_mode = mode;
    // free train sfx if not using it
    if (_train_sfx != nullptr && screen_gfx != SCREEN_GFX_TRAIN) {
        Mix_FreeChunk(_train_sfx);
        _train_sfx = nullptr;
    }
    if (screen_gfx == SCREEN_GFX_INFERNO) {
        if (_inferno_surface.surface == nullptr) {
            ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
            ImageView::get_instance()->clear_surface_area(0, 0, RES_W, RES_H, 180, 0, 0, _inferno_surface);
        }
    } else {
        free_inferno_surface();
    }
}

Uint8 Draw::get_gfx()
{
    return screen_gfx;
}

void Draw::set_flash_enabled(bool enabled)
{
    flash_effect_enabled = enabled;
}


void Draw::show_rain()
{
    for (int i=0; i<RES_W/TILESIZE; i++) {
        for (int j=0; j<AREA_H/TILESIZE; j++) {
            ImageView::get_instance()->renderTexturePortionAt(_rain_pos*TILESIZE, 0, TILESIZE, TILESIZE, i*TILESIZE, j*TILESIZE, rain_obj.texture);
        }
    }
    if (TimerView::get_instance()->getTimer() > _effect_timer) {
        _rain_pos++;
        if (_rain_pos > 2) {
            _rain_pos = 0;
        }
        _effect_timer = TimerView::get_instance()->getTimer() + RAIN_DELAY;
        std::cout << "## DRAW::SHOW_RAIN::SET-EFFECT-TIMER: " << _effect_timer << std::endl;
    }
}

void Draw::show_flash()
{
    if (flash_obj.surface == nullptr) {
        // load rain
        std::string filename = SharedData::get_instance()->GAMEPATH + "/shared/images/flash.png";
        flash_obj = ImageView::get_instance()->imageFromFile(filename);
    }
    for (int i=0; i<FLASH_POINTS_N; i++) {
        ImageView::get_instance()->renderTexturePortionAt(_flash_pos*FLASH_IMG_SIZE, 0, FLASH_IMG_SIZE, FLASH_IMG_SIZE, flash_points[i].x, flash_points[i].y, flash_obj.texture);
    }
    if (TimerView::get_instance()->getTimer() > _flash_timer) {
        _flash_pos++;
        if (_flash_pos > 2) {
            _flash_pos = 0;
        }
        _flash_timer = TimerView::get_instance()->getTimer() + FLASH_DELAY;
    }
}


void Draw::show_bubble(int x, int y)
{
    if (_bubble_gfx.surface == nullptr) {
        std::string filename = SharedData::get_instance()->GAMEPATH + "shared/images/bubble.png";
        _bubble_gfx = ImageView::get_instance()->imageFromFile(filename);
    }
    ImageView::get_instance()->renderImageAt(x, y, _bubble_gfx);
}


void Draw::show_teleport_small(int x, int y)
{
    if (teleport_small_frame_count > 0) {
        int x_origin = teleport_small_frame * _teleport_small_gfx.surface->h;
        if (TimerView::get_instance()->getTimer() > teleport_small_frame_timer) {
            teleport_small_frame++;
            teleport_small_frame_timer = TimerView::get_instance()->getTimer()+100;
        }
        //std::cout << "timer[" << TimerView::get_instance()->getTimer() << "], frame.timer[" << teleport_small_frame_timer << "], frames[" << teleport_small_frame_count << "], current frame[" << teleport_small_frame << "]" << std::endl;
        if (teleport_small_frame >= teleport_small_frame_count) {
            teleport_small_frame = 0;
        }
        st_rectangle origin_rect(x_origin, 0, _teleport_small_gfx.surface->h, _teleport_small_gfx.surface->h);
        ImageView::get_instance()->renderTexturePortionAt(origin_rect.x, origin_rect.y, origin_rect.w, origin_rect.h, x, y, _teleport_small_gfx.texture);
    } else {
        ImageView::get_instance()->renderImageAt(x, y, _teleport_small_gfx);
    }
}

/// @TODO: make "async" like dialogs
int Draw::show_credits_text(bool can_leave, std::vector<std::string> credit_text)
{
    int line_n=0;
    unsigned int scrolled = 0;
    int posY = RES_H;
    st_rectangle dest;
    float bg1_speed = 0.5;
    float bg2_speed = 3.5;
    float bg1_pos = 0;
    float bg2_pos = 0;
    st_imageData bg1;
    st_imageData bg2;


    bg1 = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->GAMEPATH + "/shared/images/star_field1.png");
    bg2 = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->GAMEPATH + "/shared/images/star_field2.png");

    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);


    std::cout << "draw::show_credits_text::START" << std::endl;

    // add the initial lines to screen
    create_engine_credits_text();

    TimerView::get_instance()->delay(200);
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(200);

    // scroll the lines
    int limit = (credit_text.size()*12)+RES_H/2;
    std::cout << "credit_text.size[" << credit_text.size() << "]" << std::endl;
    while (scrolled < limit) {

        //std::cout << "scrolled[" << scrolled << "], limit[" << limit << "]" << std::endl;

        //@TODO: draw stars fields //
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
        ImageView::get_instance()->renderImageAt(0, bg1_pos, bg1);
        if (bg1_pos > 0) {
            ImageView::get_instance()->renderTexturePortionAt(0, RES_H-bg1_pos, RES_W, bg1_pos, 0, 0, bg1.texture);
        }
        ImageView::get_instance()->renderImageAt(0, bg2_pos, bg2);
        if (bg2_pos > 0) {
            ImageView::get_instance()->renderTexturePortionAt(0, RES_H-bg2_pos, RES_W, bg2_pos, 0, 0, bg2.texture);
        }


        // @TODO: calculate min and max to trestrain loop number
        for (unsigned int i=0; i<credit_text.size(); i++) {
            int text_pos = posY+12*i;

            if (text_pos >= -12 && text_pos <= RES_H+12) {
                std::size_t found_title_blue = credit_text.at(i).find("- ");
                std::size_t found_title_red = credit_text.at(i).find("# ");
                if (credit_text.at(i)[0] == '@') { // section
                    std::string text_out = credit_text.at(i);
                    text_out = text_out.substr(1, text_out.length()-1);
                    TextView::get_instance()->renderText(0, text_pos, st_color(250, 250, 250), true, text_out);
                } else if (found_title_red != std::string::npos) { // main title
                    TextView::get_instance()->renderText(0, text_pos, st_color(250, 250, 250), true, credit_text.at(i));
                } else if (found_title_blue != std::string::npos) { // sub-title
                    TextView::get_instance()->renderText(0, text_pos, st_color(250, 250, 250), true, credit_text.at(i));
                } else {
                    TextView::get_instance()->renderText(0, text_pos, st_color(250, 250, 250), true, credit_text.at(i));
                }
                //std::cout << "text_pos[" << i << "][" << text_pos << "]" << std::endl;
            }
        }


        if (can_leave) {
            InputController::get_instance()->read_input();
            if (InputController::get_instance()->wait_scape_time(STARS_DELAY) == 1 || InputController::get_instance()->p1_input[BTN_START] == 1) {
                return 1;
            }
        } else {
            TimerView::get_instance()->delay(STARS_DELAY);
        }
        posY--;
        scrolled++;
        // need to copy next line and reposition

        bg1_pos += bg1_speed;
        bg2_pos += bg2_speed;

        if (bg1_pos >= RES_H) {
            bg1_pos = 0;
        }
        if (bg2_pos >= RES_H) {
            bg2_pos = 0;
        }
    }
    return 0;
}

int Draw::show_credits(bool can_leave)
{
    SoundView::get_instance()->stop_music();
    SoundView::get_instance()->load_music("rockbot_endcredits.mod");
    SoundView::get_instance()->play_music();
    int res = show_credits_text(can_leave, create_engine_credits_text());
    if (res == 1) {
        SoundView::get_instance()->stop_music();
        SoundView::get_instance()->load_music(GameData::get_instance()->game_data.game_start_screen_music_filename);
        SoundView::get_instance()->play_music();
        return 1;
    }
    ImageView::get_instance()->updateRender();
    TimerView::get_instance()->delay(1000);
    InputController::get_instance()->clean();
    TimerView::get_instance()->delay(100);
    InputController::get_instance()->wait_keypress();
    return 0;
}

void Draw::show_unlocked_charsMsg()
{

}

std::vector<std::string> Draw::create_engine_credits_text()
{

    fio_strings fio_str;
    std::vector<std::string> credits_list = fio_str.get_string_list_from_file(SharedData::get_instance()->FILEPATH + "/game_credits.txt");

    if (credits_list.size() > 0) {
        for (int i=0; i<6; i++) {
            credits_list.push_back("");
        }
    }

    credits_list.push_back("- ROCKBOT/ENGINE CREDITS -");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");


    credits_list.push_back("- PLANNER -");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- HELPER -");
    credits_list.push_back("ARISMEIRE KUMMER SILVA FIEDORUK");
    credits_list.push_back("NELSON ROSENBERG");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");


    credits_list.push_back("- CHARACTER DESIGNER -");
    credits_list.push_back("ARISMEIRE KUMMER SILVA FIEDORUK");
    credits_list.push_back("BOBERATU");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("NELSON ROSENBERG");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- OBJECT DESIGNER -");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- GRAPHICS DESIGNER -");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("BOBERATU");
    credits_list.push_back("HFBN2");
    credits_list.push_back("CAPT. CHRIS AND KB");
    credits_list.push_back("SURT.OPENGAMEART");
    credits_list.push_back("AVERAGE-HANZO.DEVIANTART");
    credits_list.push_back("FUNKY96.DEVIANTART");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- DIALOGS EDITOR -");
    credits_list.push_back("NELSON ROSENBERG");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- TEST PLAYER -");
    credits_list.push_back("NELSON ROSENBERG");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");


    credits_list.push_back("- ILLUSTRATION DESIGNER -");
    credits_list.push_back("ARISMEIRE KUMMER SILVA FIEDORUK");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- MUSIC COMPOSER -");
    credits_list.push_back("MODARCHIVE.ORG");
    credits_list.push_back("FIREAGE");
    credits_list.push_back("SOLARIS");
    credits_list.push_back("LEON/EXCESS");
    credits_list.push_back("DIOMATIC");
    credits_list.push_back("JASON");
    credits_list.push_back("DEADLOCK/EXLSD");
    credits_list.push_back("4MAT");
    credits_list.push_back("ZANAZAC");
    credits_list.push_back("AMEGA INDUSTRIESQ");
    credits_list.push_back("DEATH ADDR");
    credits_list.push_back("TOBBX");
    credits_list.push_back("HYOCHAN");
    credits_list.push_back("DR.BULLY/MAJIC12");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- SFX DESIGNER -");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");


    credits_list.push_back("- PROGRAMMER -");
    credits_list.push_back("IURI FIEDORUK");
    credits_list.push_back("FARLEY KNIGHT");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- PORTING HELPER -");
    credits_list.push_back("DINGUX: SHIN-NIL");
    credits_list.push_back("PANDORA: SIGMA NL");
    credits_list.push_back("PS2: RAGNAROK2040");
    credits_list.push_back("PS2: WOON-YUNG LIU");
    credits_list.push_back("PS2: SP193");
    credits_list.push_back("ANDROID: PELYA");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- REVIEW & TESTING -");
    credits_list.push_back("ARISMEIRE KUMMER SILVA FIEDORUK");
    credits_list.push_back("NELSON ROSENBERG");
    credits_list.push_back("ANDREW PRZELUCKI");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- SPECIAL THANKS -");
    credits_list.push_back("FELIPE ZACANI (FPS)");
    credits_list.push_back("FREE SDK DEVELOPERS");
    credits_list.push_back("DEVIANTART.COM");
    credits_list.push_back("PIXELJOINT.COM");
    credits_list.push_back("OPENGAMEART.ORG");
    credits_list.push_back("VENOM");
    credits_list.push_back("JERONIMO");
    credits_list.push_back("");
    credits_list.push_back("");
    credits_list.push_back("");

    credits_list.push_back("- DEVELOPMENT TOOLS -");
    credits_list.push_back("LIBSDL");
    credits_list.push_back("DIGIA QT");
    credits_list.push_back("GCC G+");
    credits_list.push_back("UBUNTU LINUX");
    credits_list.push_back("MINT LINUX");
    credits_list.push_back("GIMP EDITOR");
    credits_list.push_back("PAINT TOOL SAI");
    credits_list.push_back("COREL DRAW");
    credits_list.push_back("SIMPLE SCREEN RECORD");
    credits_list.push_back("AUDACIOUS PLAYER");
    credits_list.push_back("AUDACITY EDITOR");
    credits_list.push_back("BFXR.NET");

    for (int i=0; i<20; i++) {
        credits_list.push_back("");
    }
    credits_list.push_back("- PRESENTED BY -");
    credits_list.push_back("UPPERLAND STUDIOS");

    return credits_list;
}


void Draw::draw_credit_line(st_imageData &surface, Uint8 initial_line,std::vector<std::string> credit_text)
{
    if (initial_line < credit_text.size()) {
        std::cout << "draw::draw_credit_line - text[" << credit_text.at(initial_line) << "]" << std::endl;
        std::size_t found = credit_text.at(initial_line).find("- ");
        if (found != std::string::npos) {
            TextView::get_instance()->renderText(0, RES_H, st_color(95, 151, 255), true, credit_text.at(initial_line));
        } else {
            TextView::get_instance()->renderText(0, RES_H, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), true, credit_text.at(initial_line));
        }
    } else {
        std::cout << "ERROR draw_credit_line, initial_line[" << initial_line << "], credit_text.size()[" << credit_text.size() << "]" << std::endl;
    }
}






st_imageData *Draw::get_object_graphic(int obj_id)
{
    std::map<unsigned int, st_imageData>::iterator it;
    st_imageData temp_sprite;

    //std::cout << "objects_sprite_list.size[" << objects_sprite_list.size() << "]" << std::endl;

    it = objects_sprite_list.find(obj_id);
    if (it == objects_sprite_list.end()) { // there is no graphic with this key yet, add it
        std::string graphic_filename(GameData::get_instance()->v6_object_list.at(obj_id).graphic_filename);
        if (graphic_filename.length() > 0) {
            std::string complete_filename(SharedData::get_instance()->FILEPATH + "images/sprites/objects/" + graphic_filename);
            temp_sprite = ImageView::get_instance()->imageFromFile(complete_filename);
            objects_sprite_list.insert(std::pair<unsigned int, st_imageData>(obj_id, temp_sprite));
            it = objects_sprite_list.find(obj_id);
        } else {
            std::cout << "ERROR: Invalid object graphic. Object_ID: '" << obj_id << "'" << std::endl;
            return nullptr;
        }
    }
    return &(*it).second;
}

void Draw::show_object_graphic(int x, int y, int obj_id)
{
    st_imageData *obj_graphic = get_object_graphic(obj_id);
    if (obj_graphic != nullptr) {
        int framesize_w = GameData::get_instance()->v6_object_list.at(obj_id).size.width;
        int framesize_h = GameData::get_instance()->v6_object_list.at(obj_id).size.height;
        ImageView::get_instance()->renderTexturePortionAt(0, 0, framesize_w, framesize_h, x, y-framesize_h, Draw::get_instance()->get_object_graphic(obj_id)->texture);
    }
}

void Draw::remove_object_graphic(int obj_id)
{
    std::map<unsigned int, st_imageData>::iterator it;
    it = objects_sprite_list.find(obj_id);
    if (it != objects_sprite_list.end()) {
        objects_sprite_list.erase(obj_id);
    }
}

void Draw::show_ingame_warning(st_dialog dialog)
{
    Draw::get_instance()->show_dialog(1);
    st_position dialog_pos = Draw::get_instance()->get_dialog_pos();
    for (unsigned int i=0; i<dialog.msgs.size(); i++) {
        TextView::get_instance()->renderText(dialog_pos.x+30, dialog_pos.y+56+((FONT_SIZE*1.5)*i), st_color(250, 250, 250), false, dialog.msgs.at(i));
    }
    if (dialog.timer == 0) {
        Draw::get_instance()->show_dialog_button(0);
    }
}


void Draw::fade_screen(int r, int g, int b, int total_delay, bool reverse)
{
    st_imageData transparent_area;
    transparent_area = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
    ImageView::get_instance()->clear_surface_area(0, 0, RES_W, AREA_H, r, g, b, transparent_area);
    ImageView::get_instance()->rebuildTexture(transparent_area);
    SDL_SetTextureBlendMode(transparent_area.texture, SDL_BLENDMODE_BLEND);

    float step = 255.0/20.0;
    float delay = (total_delay / 25)-10;
    float alpha_n = 0;

    for (float i=0; i<=20; i++) {
        if (reverse == false) {
            //std::cout << "LOOP.DIRECT[" << i << "]" << std::endl;
            ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, AREA_H, 0, 0, ImageView::get_instance()->get_game_texture());
            ImageView::get_instance()->set_surface_alpha(alpha_n, transparent_area);
            ImageView::get_instance()->renderImageAt(0, 0, transparent_area);
        } else {
            //std::cout << "LOOP.REVERSE[" << i << "]" << std::endl;
            ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, AREA_H, 0, 0, ImageView::get_instance()->get_game_texture());
            ImageView::get_instance()->set_surface_alpha(255-alpha_n, transparent_area);
            ImageView::get_instance()->renderImageAt(0, 0, transparent_area);
        }
        ImageView::get_instance()->updateRender();
        alpha_n += step;
        if (delay >= 1) {
            TimerView::get_instance()->delay(delay);
        }
    }
}

// executes the fade-out each time update screen is called
void Draw::add_fade_out_effect(int r, int g, int b)
{
    current_alpha = 0;
    current_alpha_color = st_color(r, g, b);
}

void Draw::remove_fade_out_effect()
{
    current_alpha = -1;
    current_alpha_surface.freeGraphic();
}

void Draw::pixelate_screen()
{
    st_imageData res_surface = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
    st_imageData screen_copy = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
    ImageView::get_instance()->copyScreenAreaToImage(0, 0, RES_W, RES_H, 0, 0, screen_copy);

    for (int pixelationAmount=2; pixelationAmount<8; pixelationAmount++) {
        for (int x = 0; x < RES_W; x+= pixelationAmount) { // do the whole image
            for (int y = 0; y < RES_H; y+= pixelationAmount) {

                int avR = 0;
                int avG = 0;
                int avB =0;


                int pointsCount = 0;
                for (int i=0; i<pixelationAmount; i++) {
                    for (int j=0; j<pixelationAmount; j++) {
                        st_color pt_color = screen_copy.get_point_color(x+i, y+j);
                        //res_surface.set_point_color((i+x), (j+y), pt_color.r, pt_color.g, pt_color.b);

                        avR += (int) (pt_color.r);
                        avG+= (int) (pt_color.g);
                        avB += (int) (pt_color.b);
                        if (pt_color.r != 0 && pt_color.g != 0 && pt_color.g != 0) {
                            pointsCount++;
                        }
                    }
                    //std::cout << "x[" << x << "], y[" << y << "], xx[" << (i+x) << "]" << std::endl;
                }
                if (pointsCount != 0) {
                    avR = avR/pointsCount; //divide all by the amount of samples taken to get an average
                    avG = avG/pointsCount;
                    avB = avB/pointsCount;
                }

                for (int i=0; i<pixelationAmount; i++) {
                    for (int j=0; j<pixelationAmount; j++) {
                        res_surface.set_point_color(x+i, y+j, avR, avG, avB);
                    }
                }

            }
        }
        //std::cout << "pixelationAmount[" << pixelationAmount << "]" << std::endl;
        ImageView::get_instance()->renderImageAt(0, 0, res_surface);
        ImageView::get_instance()->updateRender();
        TimerView::get_instance()->delay(20);
    }
    std::cout << "END" << std::endl;
    res_surface.freeGraphic();

}

void Draw::add_weapon_tooltip(short weapon_n, const st_position &player_pos, const Uint8 &direction)
{
    _weapon_tooltip_n = weapon_n;
    _weapon_tooltip_pos_ref = &player_pos;
    _weapon_tooltip_timer = TimerView::get_instance()->getTimer() + 1500;
    _weapon_tooltip_direction_ref = &direction;
}

st_float_position Draw::get_radius_point(st_position center_point, int radius, float angle)
{
    st_float_position res;
    // x = r * cos(x0), y = r * sin(y0)
    res.x = radius * cos(angle) + center_point.x;
    res.y = radius * sin(angle) + center_point.y;
    return res;
}

void Draw::draw_castle_path(bool instant, st_position initial_point, st_position final_point)
{
    if (initial_point.x == 0 && initial_point.y == 0 && final_point.x == 0 && final_point.x == 0) {
        return;
    }
    int dist_x = initial_point.x - final_point.x;
    int dist_y = initial_point.y - final_point.y;
    int duration = CASTLE_PATH_DURATION;
    int step_delay = duration / (abs(dist_x) + abs(dist_y));
    if (instant == true) {
        duration = 0;
        step_delay = 0;
    }

    std::cout << "step_delay[" << step_delay << "]" << std::endl;

    draw_castle_point(initial_point.x, initial_point.y);
    draw_castle_point(final_point.x, final_point.y);
    ImageView::get_instance()->updateRender();

    int pos_y = initial_point.y - 1;
    int pos_x = initial_point.x + 2;

    // first, move Y axis
    if (dist_y < 0) {
        pos_y += castle_point.surface->h + 1;
    }
    std::cout << "ini.y[" << initial_point.y << "], end.y[" << final_point.y << "], dist_y[" << dist_y << "]" << std::endl;
    if (dist_y != 0) {
        for (int i=0; i<abs(dist_y)-2; i++) {
            // border left
            ImageView::get_instance()->clearScreenArea(pos_x-1, pos_y, 1, 1, 19, 19, 19);
            // middle
            ImageView::get_instance()->clearScreenArea(pos_x, pos_y, 4, 1, 220, 220, 220);
            // border right
            ImageView::get_instance()->clearScreenArea(pos_x+4, pos_y, 1, 1, 19, 19, 19);

            if (dist_y > 0) {
                pos_y--;
            } else {
                pos_y++;
            }
            if (step_delay > 0) {
                TimerView::get_instance()->delay(step_delay);
                ImageView::get_instance()->updateRender();
            }
        }
    }
    // remove extra bit
    if (dist_y > 0) {
        pos_y++;
    } else {
        pos_y -= 4;
    }

    int temp_pos_x = pos_x;
    int max_dist_x = abs(dist_x)-2;
    if (dist_x > 0) {
        temp_pos_x -= 1;
        max_dist_x -= 4;
    } else if (dist_x < 0) {
        temp_pos_x += 4;
        max_dist_x -= 4;
    }

    // secondly, move x axis
    if (dist_x > 0) {
        pos_x += 3;
    }
    std::cout << "ini.x[" << initial_point.x << "], end.x[" << final_point.x << "], dist_x[" << dist_x << "]" << std::endl;
    if (dist_x != 0) {
        for (int i=0; i<max_dist_x; i++) {
            // top
            ImageView::get_instance()->clearScreenArea(temp_pos_x, pos_y-1, 1, 1, 19, 19, 19);
            // middle
            ImageView::get_instance()->clearScreenArea(temp_pos_x, pos_y, 1, 4, 220, 220, 220);
            // bottom
            ImageView::get_instance()->clearScreenArea(temp_pos_x, pos_y+4, 1, 1, 19, 19, 19);
            if (dist_x > 0) {
                temp_pos_x--;
            } else {
                temp_pos_x++;
            }
            if (step_delay > 0) {
                TimerView::get_instance()->delay(step_delay);
                ImageView::get_instance()->updateRender();
            }
        }
    }

}

void Draw::draw_castle_point(int x, int y)
{
    ImageView::get_instance()->renderImageAt(x, y, castle_point);
}

void Draw::show_interstage_map_bg(st_position pos)
{
    ImageView::get_instance()->renderImageAt(0, 0, interstage_map);
    ImageView::get_instance()->renderTexturePortionAt(TILESIZE, 0, TILESIZE, TILESIZE, pos.x-4, pos.y-4, hud_player_1up.texture);
    ImageView::get_instance()->updateRender();
    TimerView::get_instance()->delay(5000);
}

void Draw::draw_in_game_menu_bg(int screen)
{
    //ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 20);

    std::string text_next_page = "R-[AREA-MAP]";
    std::string current_page_text = "[CHARACTER]";

    if (screen == 0) {
        ImageView::get_instance()->renderImageAt(10, 10, in_game_menu_bg);
    } else if (screen == 1) { // MAP
        ImageView::get_instance()->renderImageAt(10, 10, in_game_menu_bg_clean);
        current_page_text = "[AREA-MAP]";
        text_next_page = "R-[GAME-OPTIONS]";
    } else { // OPTIONS
        ImageView::get_instance()->renderImageAt(10, 10, in_game_menu_bg_clean);
        current_page_text = "[GAME-OPTIONS]";
        text_next_page = "R-[CHARACTER]";
    }
    int text_w = FONT_ACTUAL_SIZE_W*text_next_page.size();
    //std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>> text_w[" << text_w << "]" << std::endl;
    TextView::get_instance()->renderCenteredText(66, st_color(0, 0, 0), current_page_text);
    TextView::get_instance()->renderText(RES_W-94-text_w, 622, st_color(0, 0, 0), false, text_next_page);

    if (screen == 1) { // MAP
        draw_in_game_menu_map();
    }
}

void Draw::draw_in_game_menu_animation()
{
    // @TODO: set map scroll to current room //
    in_game_menu_map_pos = st_position(0, 0);
    ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 20);

    ImageView::get_instance()->change_render_target(RENDER_TARGET_DIRECT_SCREEN);
    ImageView::get_instance()->updateRender();
    TimerView::get_instance()->delay(100);
    // menu 001 animation //

    for (int i=0; i<424; i+=16) {
        SDL_RenderClear(gRenderer);
        ImageView::get_instance()->renderTexturePortionAt(0, 0, in_game_menu_bg.surface->w, 137+i, 10, 10, in_game_menu_bg.texture);
        ImageView::get_instance()->renderTexturePortionAt(0, 548, in_game_menu_bg.surface->w, 152, 10, 137+i, in_game_menu_bg.texture);
        ImageView::get_instance()->updateRender();
        TimerView::get_instance()->delay(1);
    }
    int final_pos = 420;
    SDL_RenderClear(gRenderer);
    ImageView::get_instance()->renderTexturePortionAt(0, 0, in_game_menu_bg.surface->w, 137+final_pos, 10, 10, in_game_menu_bg.texture);
    ImageView::get_instance()->renderTexturePortionAt(0, 548, in_game_menu_bg.surface->w, 152, 10, 138+final_pos, in_game_menu_bg.texture);
    ImageView::get_instance()->updateRender();
}

void Draw::draw_in_game_menu_map()
{
    //std::cout << "DRAW::draw_in_game_menu_map - visited_level_list.size[" << SharedData::get_instance()->visited_level_list.size() << "]" << std::endl;
    int MAP_ROOM_SIZE_W = 24;
    int MAP_ROOM_SIZE_H = 16;
    int adjust_x = 280;
    int adjust_y = 190;
    /*
    for (int i=0; i<SharedData::get_instance()->visited_level_list.size(); i++) {
        // @TODO: support for multiple areas //
        if (i != 0) {
            break;
        }
        // show visited tiles
        for (int x=0; x<FILE_AREA_W; x++) {
            for (int y=0; y<FILE_AREA_H; y++) {
                if (SharedData::get_instance()->visited_level_list.at(i).visited[x][y] == true) {
                    if (i == SharedData::get_instance()->v6_selected_area && x == SharedData::get_instance()->current_room_pos.x && y == SharedData::get_instance()->current_room_pos.y) {
                        ImageView::get_instance()->clearScreenArea(x*MAP_ROOM_SIZE_W+adjust_x, y*MAP_ROOM_SIZE_H+adjust_y, MAP_ROOM_SIZE_W, MAP_ROOM_SIZE_H, 0, 150, 255);
                    } else {
                        ImageView::get_instance()->clearScreenArea(x*MAP_ROOM_SIZE_W+adjust_x, y*MAP_ROOM_SIZE_H+adjust_y, MAP_ROOM_SIZE_W, MAP_ROOM_SIZE_H, level_color_list.at(i).r, level_color_list.at(i).g, level_color_list.at(i).b);
                    }
                } else {
                    ImageView::get_instance()->clearScreenArea(x*MAP_ROOM_SIZE_W+adjust_x, y*MAP_ROOM_SIZE_H+adjust_y, MAP_ROOM_SIZE_W, MAP_ROOM_SIZE_H, 0, 0, 0);
                }
            }
        }
        // draw grid
        for (int x=1; x<FILE_AREA_W; x++) {
            ImageView::get_instance()->clearScreenArea(x*MAP_ROOM_SIZE_W+adjust_x, adjust_y, 1, FILE_AREA_H*MAP_ROOM_SIZE_H, 255, 255, 255);
        }
        for (int y=1; y<FILE_AREA_H; y++) {
            ImageView::get_instance()->clearScreenArea(adjust_x, y*MAP_ROOM_SIZE_H+adjust_y, FILE_AREA_W*MAP_ROOM_SIZE_W, 1, 255, 255, 255);
        }
    }
    */
}

void Draw::draw_game_button(int x, int y, e_INPUT_IMAGES button)
{
    //std::cout << "draw::draw_game_button - x[" << x << "], y[" << y << "]" << std::endl;
    draw_game_button_request.x = x - input_images_map.at(draw_game_button_request.button).surface->w/2;
    draw_game_button_request.y = y - input_images_map.at(draw_game_button_request.button).surface->h/2;
    draw_game_button_request.button = button;
}

void Draw::show_dialog(Uint8 position)
{
    int posX = (RES_W-dialog_surface.surface->w)*0.5;
    int posY;

    if (position == 0) {
        posY = (RES_H-dialog_surface.surface->h)*0.5;
    } else if (position == 1) {
        posY = 3;
    } else {
        posY = RES_H - dialog_surface.surface->h - 25;
    }

    _dialog_pos.x = posX;
    _dialog_pos.y = posY;

    st_position bg_pos(posX, posY);
    ImageView::get_instance()->renderImageAt(bg_pos.x, bg_pos.y, dialog_surface);

}

void Draw::show_dialog_button(Uint8 position)
{
    int posX = (RES_W-dialog_surface.surface->w)*0.5;
    int posY;

    if (position == 0) {
        posY = (RES_H-dialog_surface.surface->h)*0.5;
    } else if (position == 1) {
        posY = 3;
    } else {
        posY = RES_H - dialog_surface.surface->h - 25;
    }

    int btn_dest_x = posX + dialog_surface.surface->w-input_images_map.at(INPUT_IMAGES_A).surface->w - 10;
    int btn_dest_y = posY+dialog_surface.surface->h-input_images_map.at(INPUT_IMAGES_A).surface->h-TILESIZE/2;
    ImageView::get_instance()->renderTexturePortionAt(0, 0, input_images_map.at(INPUT_IMAGES_A).surface->w, input_images_map.at(INPUT_IMAGES_A).surface->h, btn_dest_x, btn_dest_y, input_images_map.at(INPUT_IMAGES_A).texture);

}

st_position Draw::get_dialog_pos() const
{
    return _dialog_pos;
}

void Draw::show_dialogs_from_queue()
{
    if (GameManager::get_instance()->get_dialog_queue()->size() > 0) {
        Draw::get_instance()->show_ingame_warning(GameManager::get_instance()->get_dialog_queue()->at(0));
        if (GameManager::get_instance()->get_dialog_queue()->at(0).music_filename.length() > 0 && GameManager::get_instance()->get_dialog_status()->started == false) {
            InputController::get_instance()->clean();
            GameManager::get_instance()->get_dialog_status()->started = true;
            SoundView::get_instance()->stop_music();
            SoundView::get_instance()->load_music(GameManager::get_instance()->get_dialog_queue()->at(0).music_filename);
            SoundView::get_instance()->play_music_once();
            GameManager::get_instance()->get_dialog_status()->timer = TimerView::get_instance()->getTimer() + GameManager::get_instance()->get_dialog_queue()->at(0).timer;
        }
    }
}

void Draw::draw_water_tile_overlay(int x, int y)
{
    // TODO: we can add animation to the water tile overlay
    //std::cout << "draw::draw_water_tile_overlay - water_animation_pos[" << water_animation_pos << "]" << std::endl;

    // left part
    ImageView::get_instance()->renderTexturePortionAt(water_animation_pos, 0, TILESIZE-water_animation_pos, TILESIZE, x, y, water_tile_overlay.texture);

    // right part
    ImageView::get_instance()->renderTexturePortionAt(0, 0, water_animation_pos, TILESIZE, x+TILESIZE-water_animation_pos, y, water_tile_overlay.texture);


    if (TimerView::get_instance()->getTimer() > water_animation_timer) {
        water_animation_timer = TimerView::get_instance()->getTimer()+WATER_ANIMATION_FRAME_TIME;
        water_animation_pos++;
        if (water_animation_pos >= TILESIZE) {
            water_animation_pos = 0;
        }
    }
}

void Draw::draw_player_death(st_position center_point, int frame_n)
{
    int frame_size = _death_animation.surface->h;
    ImageView::get_instance()->renderTexturePortionAt(frame_n*frame_size, 0, frame_size, frame_size, center_point.x-frame_size/2, center_point.y-frame_size/2, _death_animation.texture);
}

int Draw::get_death_animation_frames_n()
{
    return _death_animation_frames_n;
}

void Draw::draw_explosion(st_position center_point, int radius, int angle_inc)
{
    // 8 initial points
    int points_n = 12;
    st_float_position points[points_n];
    float angle_diff = 360 / points_n;
    int frame = 0;

    std::cout << "DRAW::draw_explosion::START, angle_diff[" << angle_diff << "], center[" << center_point.x << "][" << center_point.y << "]" << std::endl;

    for (int j=0; j<points_n; j++) {
        float angle = (j*angle_diff)+angle_inc;
        float angle_rad = (angle * 3.14)/180;
        points[j] = get_radius_point(center_point, radius, angle_rad);
        std::cout << "DRAW::draw_explosion - angle[" << angle_rad << "], point[" << j << "][" << points[j].x << "][" << points[j].y << "]" << std::endl;
        ImageView::get_instance()->renderTexturePortionAt(frame*32, 0, 32, 32, points[j].x, points[j].y, ImageView::get_instance()->get_preloaded_image(PRELOADED_IMAGES_EXPLOSION_BUBBLE)->texture);
    }
}


st_imageData *Draw::get_dynamic_background(std::string filename)
{
    std::map<std::string, st_imageData>::iterator it;

    it = maps_dynamic_background_list.find(filename);
    if (it == maps_dynamic_background_list.end()) {
        return nullptr;
    }

    return &maps_dynamic_background_list.find(filename)->second;
}

st_imageData *Draw::get_dynamic_foreground(std::string filename)
{
    std::map<std::string, st_imageData>::iterator it;

    it = maps_dynamic_background_list.find(filename);
    if (it == maps_dynamic_background_list.end()) {
        return nullptr;
    }

    return &maps_dynamic_background_list.find(filename)->second;
}

void Draw::set_dynamic_bg_alpha(std::string filename, int alpha)
{
    std::map<std::string, st_imageData>::iterator it;

    it = maps_dynamic_background_list.find(filename);
    if (it == maps_dynamic_background_list.end()) {
        return;
    }
    ImageView::get_instance()->set_surface_alpha(alpha, maps_dynamic_background_list.find(filename)->second);
}


void Draw::show_hud(int hp, int player_n, int selected_weapon, int selected_weapon_value, int room_n_x, int room_n_y)
{
    // TODO::IURI - usar imagem de fundo //
    ImageView::get_instance()->clearScreenArea(0, AREA_H, RES_W, HUD_H, 0, 0, 0);
    ImageView::get_instance()->renderImageAt(0, 0, hud_image);

    // player HP
    int hp_percent = (100 * hp) / fio.get_heart_pieces_number(SharedData::get_instance()->game_save);


    draw_enery_bars(hp_percent, 113, AREA_H+20, 0); // HP
    draw_enery_bars(hp_percent, 113, AREA_H+43, 1); // MP


    /// @TODO: show map ///


    show_hud_items();
    if (timer_hud_center_show == true) {
        ImageView::get_instance()->clearScreenArea(HUD_GRID_CENTER_X+4, HUD_GRID_CENTER_Y+7, 13, 12, 227, 179, 2); // TODO: blink
    }
    unsigned long now_timer = TimerView::get_instance()->getTimer();
    if (timer_hud_center < now_timer) {
        timer_hud_center = now_timer + HUD_CENTER_BLINK_TIMER;
        timer_hud_center_show = !timer_hud_center_show;
    }


    //ImageView::get_instance()->clearScreenArea(100, 10, RES_W, AREA_H, 227, 2, 2);
    //renderTexturePortionAt(0, 0, RES_W, HUD_H, 0, AREA_H-50, hud_texture_render_target);
    ImageView::get_instance()->renderTexturePortionAt(0, 0, RES_W, HUD_H, 0, AREA_H, ImageView::get_instance()->get_hud_texture());
}

void Draw::show_hud_items()
{
    for (int i=0; i<GAME_ITEM_SLOTS; i++) {
        if (SharedData::get_instance()->game_save.game_item_list[i].uuid != -1) {
            //std::cout << "picked item in slot[" << i << "]" << std::endl;
            //SharedData::get_instance()->game_save.game_item_list[i].obj_id = obj_info.get_id();
            //SharedData::get_instance()->game_save.game_item_list[i].uuid = obj_info.get_uuid();
            ImageView::get_instance()->show_item_tooltip(st_position(580+(i*TILESIZE)+i*6, 72), SharedData::get_instance()->game_save.game_item_list[i].obj_id);
        }
    }
}

void Draw::draw_enery_bars(int value, int x_pos, int y_pos, int type)
{
    int type_y = 0;
    if (type == 1) {
        type_y = 16;
    }
    // 5 balls, each have 4 possible stages
    // so each slice of energy is 100 / (5*4) = 5%
    for (int i=0; i<10; i++) {
        int full_part = (i+1)*10;
        if (value >= full_part) {
            ImageView::get_instance()->renderTexturePortionAt(0, type_y, 8, 16, x_pos+(8*i), y_pos, hud_energy_bar.texture);
        } else {
            ImageView::get_instance()->renderTexturePortionAt(8, type_y, 8, 16, x_pos+(8*i), y_pos, hud_energy_bar.texture);
        }




    }
}

void Draw::set_boss_hp(int hp)
{
    _boss_current_hp = hp;
}

void Draw::show_boss_intro_bg()
{
    ImageView::get_instance()->renderImageAt(0, 0, boss_intro_bg);
    ImageView::get_instance()->updateRender();
}

void Draw::clear_maps_dynamic_background_list()
{
    maps_dynamic_background_list.clear();
}

void Draw::add_dynamic_background(std::string filename, int auto_scroll_mode, st_color bg_color)
{
    // only add if not existing in map
    if (maps_dynamic_background_list.find(filename) == maps_dynamic_background_list.end()) {


        //std::cout << "DRAW::add_dynamic_background::ADD[" << filename << "]" << std::endl;

        maps_dynamic_background_list.insert(std::pair<std::string,st_imageData>(filename, st_imageData()));
        std::string bg1_filename(SharedData::get_instance()->FILEPATH+"images/map_backgrounds/" + filename);


        st_imageData temp_surface;
        temp_surface = ImageView::get_instance()->imageFromFile(bg1_filename);

        //std::cout << "MAP::add_bg[" << bg1_filename << "], w[" << (int)temp_surface.width << "], h[" << (int)temp_surface.height << "]" << std::endl;


        maps_dynamic_background_list.find(filename)->second = ImageView::get_instance()->initSurface(st_size(temp_surface.surface->w, temp_surface.surface->h));
        ImageView::get_instance()->clear_surface_area(0, 0, temp_surface.surface->w, temp_surface.surface->h, bg_color.r, bg_color.g, bg_color.b, maps_dynamic_background_list.find(filename)->second);
        ImageView::get_instance()->copyArea(st_position(0, 0), temp_surface, maps_dynamic_background_list.find(filename)->second);
        //maps_dynamic_background_list.find(filename)->second
    }
}



void Draw::generate_snow_particles()
{
    for (int i=0; i<SNOW_PARTICLES_NUMBER; i++) {
        int rand_x = rand() % RES_W;
        int rand_y = rand() % RES_H;
        int rand_speed = rand() % 5;
        if (rand_speed < 1) {
            rand_speed = 1;
        }
        _snow_particles.push_back(st_snow_particle(st_float_position(rand_x, rand_y), rand_speed));
    }
}

void Draw::show_snow_effect()
{
    if (_snow_particles.size() == 0) {                  // generate snow particles, if needed
        generate_snow_particles();
    }
    std::vector<st_snow_particle>::iterator it;


    //std::cout << "## DRAW::SHOW_SNOW - TimerView::get_instance()->getTimer(): " << TimerView::get_instance()->getTimer() << ", _effect_timer: " << _effect_timer << std::endl;

    if (TimerView::get_instance()->getTimer() > _effect_timer) {

        for (it=_snow_particles.begin(); it!=_snow_particles.end(); it++) {
            st_snow_particle *temp_particle = &(*it);
            temp_particle->position.y += temp_particle->speed;
            if (temp_particle->direction == ANIM_DIRECTION_LEFT) {
                temp_particle->position.x--;
            } else {
                temp_particle->position.x++;
            }
            temp_particle->x_dist++;
            if (temp_particle->x_dist > 8) {
                temp_particle->x_dist = 0;
                temp_particle->direction = !temp_particle->direction;
            }
            if (temp_particle->position.y > AREA_H) {
                temp_particle->position.x = rand() % RES_W;
                temp_particle->speed = rand() % 5;
                if (temp_particle->speed < 1) {
                    temp_particle->speed = 1;
                }
                temp_particle->position.y = 0;

            }
            ImageView::get_instance()->renderTexturePortionAt(0, 0, snow_flacke.surface->w, snow_flacke.surface->h, temp_particle->position.x, temp_particle->position.y, snow_flacke.texture);
        }
        _effect_timer = TimerView::get_instance()->getTimer() + SNOW_DELAY;
    } else {
        for (it=_snow_particles.begin(); it!=_snow_particles.end(); it++) {
            st_snow_particle *temp_particle = &(*it);
            ImageView::get_instance()->renderTexturePortionAt(0, 0, snow_flacke.surface->w, snow_flacke.surface->h, temp_particle->position.x, temp_particle->position.y, snow_flacke.texture);
        }
    }
}

void Draw::show_train_effect()
{
    if (_train_effect_timer == 0) {
        std::cout << "TRAIN_EFFECT-RESET" << std::endl;
        _train_effect_timer = TimerView::get_instance()->getTimer() + TRAIN_DELAY;
        _train_effect_state = 0;
        if (_train_sfx == nullptr) {
            _train_sfx = SoundView::get_instance()->sfx_from_file("train.wav");
        }
    } else {
        if (_train_effect_timer < TimerView::get_instance()->getTimer()) {
            if (_train_effect_state == 0) {
                _train_effect_timer = TimerView::get_instance()->getTimer() + TRAIN_EFFECT_DELAY;
                _train_effect_state++;
                SoundView::get_instance()->play_sfx_from_chunk(_train_sfx, 1);
            } else {
                _train_effect_timer = TimerView::get_instance()->getTimer() + TRAIN_DELAY;
                _train_effect_state = 0;
                SoundView::get_instance()->play_sfx_from_chunk(_train_sfx, 1);
            }
        }
    }
}

void Draw::show_lightingbolt_effect()
{
    if (_lightingbolt_effect_timer == 0) {
        _lightingbolt_effect_timer = TimerView::get_instance()->getTimer() + LIGHTINGBOLT_DELAY1;
        return;
    }
    if (_lightingbolt_effect_timer < TimerView::get_instance()->getTimer()) {
        if (_lightingbolt_effect_state <= 5) {
            _lightingbolt_effect_timer = TimerView::get_instance()->getTimer() + LIGHTINGBOLT_DELAY2;
            _lightingbolt_effect_state++;
        } else {
            _lightingbolt_effect_timer = TimerView::get_instance()->getTimer() + LIGHTINGBOLT_DELAY1;
            _lightingbolt_effect_state = 0;
        }
    } else {
        if (_lightingbolt_effect_state % 2 != 0) {
            if (_lightingbolt_effect_state == 1) {
                SoundView::get_instance()->play_shared_sfx("thunder.wav");
            }
            st_imageData transparent_area;
            transparent_area = ImageView::get_instance()->initSurface(st_size(RES_W, AREA_H));
            ImageView::get_instance()->clear_surface_area(0, 0, RES_W, AREA_H, 250, 250, 158, transparent_area);
            ImageView::get_instance()->set_surface_alpha(80, transparent_area);
            ImageView::get_instance()->renderImageAt(0, 0, transparent_area);

            //ImageView::get_instance()->clearScreenArea(0, 0, RES_W, AREA_H, 250, 250, 158);
        }
    }
}

void Draw::show_shadow_top_effect()
{

    int max = 100;
    int alpha = 255;
    int alpha_step = alpha/12;
    for (int i=0; i<max; i+=10) {
        ImageView::get_instance()->set_surface_alpha(alpha, shadow_line);
        //std::cout << "shadow.y[" << i << "], alpha[" << alpha << "]" << std::endl;
        ImageView::get_instance()->renderTexturePortionAt(0, 0, shadow_line.surface->w, shadow_line.surface->h, 0, i, shadow_line.texture);
        alpha -= alpha_step;
    }
}

void Draw::show_inferno_effect()
{
    if (_inferno_surface.is_null()) {
        _inferno_surface = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));
        ImageView::get_instance()->clear_surface_area(0, 0, RES_W, RES_H, 180, 0, 0, _inferno_surface);
    }
    ImageView::get_instance()->set_surface_alpha(_inferno_alpha, _inferno_surface);
    ImageView::get_instance()->renderImageAt(0, 0, _inferno_surface);
    if (_inferno_alpha_mode == 0) {
        _inferno_alpha += INFERNO_ALPHA_STEP;
        if (_inferno_alpha >= 180) {
            _inferno_alpha_mode = 1;
        }
    } else {
        _inferno_alpha -= INFERNO_ALPHA_STEP;
        if (_inferno_alpha <= 50) {
            _inferno_alpha_mode = 0;
        }
    }
}



void Draw::show_dark_effect()
{
    int alpha = 180;

    // create a white surface that adds to the dark, so we can simulare an alpha effect
    st_imageData alpha_mask_dark_effect;
    ImageView::get_instance()->init_target_image(alpha_mask_dark_effect, RES_W, AREA_H);
    ImageView::get_instance()->clear_texture_area(0, 0, RES_W, RES_H, 255, 255, 255, 100, alpha_mask_dark_effect);


    SDL_SetTextureBlendMode(dark_effect_surface.texture, SDL_BLENDMODE_BLEND);
    ImageView::get_instance()->clear_texture_area(0, 0, RES_W, RES_H, 0, 0, 80, 255, dark_effect_surface);
    SDL_SetTextureBlendMode(dark_effect_surface.texture, SDL_BLENDMODE_MOD);

    st_position player_center_pos = GameManager::get_instance()->get_player_relative_center_position();


    ImageView::get_instance()->blend_images(alpha_mask_dark_effect, dark_effect_surface, 0, 0);
    ImageView::get_instance()->blend_images(dark_effect_light_source_mask, dark_effect_surface, player_center_pos.x-512, player_center_pos.y-512);
    std::cout << "WHITE x[" << (player_center_pos.x-dark_effect_light_source_mask.surface->w/2) << "], y[" << (player_center_pos.y-dark_effect_light_source_mask.surface->h/2) << "]" << std::endl;
    //WHITE x[209], y[-148]
    // TODO: must to this to all light sources
    ImageView::get_instance()->blend_images(dark_effect_light_source_mask, dark_effect_surface, player_center_pos.x-dark_effect_light_source_mask.surface->w/2, player_center_pos.y-dark_effect_light_source_mask.surface->h/2);

    int limit_flame_effect = 20;
    if (flame_light_timer < TimerView::get_instance()->getTimer()) {
        if (flame_light_expanding) {
            flame_light_state += 2;
            if (flame_light_state > limit_flame_effect) {
                flame_light_state = limit_flame_effect;
                flame_light_expanding = !flame_light_expanding;
            }
        } else {
            flame_light_state -= 2;
            if (flame_light_state < 0) {
                flame_light_state = 0;
                flame_light_expanding = !flame_light_expanding;
            }
        }
        flame_light_timer = TimerView::get_instance()->getTimer() + 10;
    }

    ImageView::get_instance()->blend_images(yellow_light_mask, dark_effect_surface, -100+flame_light_state/2, -100+flame_light_state/2, yellow_light_mask.surface->w-flame_light_state, yellow_light_mask.surface->h-flame_light_state);

    //ImageView::get_instance()->blend_images(yellow_light_mask, dark_effect_surface, player_center_pos.x-128, player_center_pos.y-128);
    //ImageView::get_instance()->set_surface_alpha(50, light_points_layer);

/*
    for (int i=0; i<SharedData::get_instance()->lightpoint_list.size(); i++) {
        if (SharedData::get_instance()->lightpoint_list.at(i).color == LIGHT_POINT_COLOR_WHITE) {
            ImageView::get_instance()->blend_images(dark_effect_light_source_mask, dark_effect_surface,  SharedData::get_instance()->lightpoint_list.at(i).x-dark_effect_light_source_mask.surface->w/2, SharedData::get_instance()->lightpoint_list.at(i).y-dark_effect_mask.surface->h/2);
        } else if (SharedData::get_instance()->lightpoint_list.at(i).color == LIGHT_POINT_COLOR_YELLOW) {
            ImageView::get_instance()->blend_images(yellow_light_mask, dark_effect_surface, SharedData::get_instance()->lightpoint_list.at(i).x-yellow_light_mask.surface->w/2, SharedData::get_instance()->lightpoint_list.at(i).y-yellow_light_mask.surface->h/2);
        } else if (SharedData::get_instance()->lightpoint_list.at(i).color == LIGHT_POINT_COLOR_RED) {
            ImageView::get_instance()->blend_images(red_light_mask, dark_effect_surface, SharedData::get_instance()->lightpoint_list.at(i).x-red_light_mask.surface->w/2, SharedData::get_instance()->lightpoint_list.at(i).y-red_light_mask.surface->h/2);
        }
    }
*/

    SDL_SetTextureAlphaMod(dark_effect_surface.texture, 55);
    ImageView::get_instance()->renderImageAt(0, 0, dark_effect_surface);



    //ImageView::get_instance()->clear_texture_area(0, 0, RES_W, RES_H, 0, 0, 0, 55, dark_effect_surface);
    //ImageView::get_instance()->blend_images(dark_effect_mask, dark_effect_surface, player_center_pos.x, player_center_pos.y);
    //SDL_SetTextureBlendMode(dark_effect_surface.texture, SDL_BLENDMODE_BLEND);
    //SDL_SetTextureAlphaMod(dark_effect_surface.texture, alpha);
    //ImageView::get_instance()->renderImageAt(0, 0, dark_effect_surface);


}

void Draw::show_tint_screen_effect()
{
    st_imageData tint_image;
    ImageView::get_instance()->init_target_image(tint_image, RES_W, AREA_H);
    //SDL_SetTextureBlendMode(tint_image.texture, SDL_BLENDMODE_MUL);

    ImageView::get_instance()->clear_texture_area(0, 0, RES_W, RES_H, 255, 0, 0, 55, tint_image);

    ImageView::get_instance()->set_surface_alpha(100, tint_image);

    SDL_SetTextureBlendMode(tint_image.texture, SDL_BLENDMODE_BLEND);
    ImageView::get_instance()->renderImageAt(0, 0, tint_image);


    Uint8 alpha = 0x7F;
    //SDL_SetTextureAlphaMod(tint_image.texture, alpha);
    //SDL_SetTextureColorMod(tint_image.texture, 255, 0, 0);
}


void Draw::free_inferno_surface()
{
    _inferno_surface.freeGraphic();
}



