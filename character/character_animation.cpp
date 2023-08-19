#include "character_animation.h"
#include "game_data.h"
#include "view/imageview.h"
#include "view/timerview.h"
#include "view/draw.h"

// @TODO: flip on direction //

character_animation::character_animation() : initialized(false)
{
}

void character_animation::init(std::string set_name, std::string filename, st_size size, st_sprite_data data[ANIM_TYPE_COUNT][ANIM_FRAMES_COUNT])
{
    name = set_name;
    for (int i=0; i<ANIM_TYPE_COUNT; i++) {
        for (int j=0; j<ANIM_FRAMES_COUNT; j++) {
            sprite_data[i][j] = data[i][j];
        }
    }
    // if the char does not have image loaded yet, add it
    if (Draw::get_instance()->character_graphic_map.find(name) == Draw::get_instance()->character_graphic_map.end()) {
        st_imageData sprites_img;
        sprites_img = ImageView::get_instance()->imageFromFile(filename);
        std::pair<std::string, st_imageData> sprites_data(name, sprites_img);
        Draw::get_instance()->character_graphic_map.insert(sprites_data);
    }
    sprite_size = size;
    graphic_ref = &Draw::get_instance()->character_graphic_map.find(name)->second;
    frame_n = 0;
    next_frame_timer = 0;
    initialized = true;
}

void character_animation::show_sprite(st_position dest)
{
    if (!initialized) {
        return;
    }
    // by default, use position zero, zero
    st_rectangle sprite_rect = st_rectangle(0, 0, sprite_size.width, sprite_size.height);
    // check if frame exists
    if (sprite_data[type][frame_n].used == true) {
        sprite_rect = st_rectangle(sprite_data[type][frame_n].sprite_graphic_pos_x*sprite_size.width, 0, sprite_size.width, sprite_size.height);
        next_frame_timer = TimerView::get_instance()->getTimer() + sprite_data[type][frame_n].duration;
    // if current frame does not exist, use anim_stand and frame zero, instead
    } else if (sprite_data[ANIM_TYPE_STAND][0].used == true) {
        sprite_rect = st_rectangle(sprite_data[ANIM_TYPE_STAND][frame_n].sprite_graphic_pos_x*sprite_size.width, 0, sprite_size.width, sprite_size.height);
        next_frame_timer = TimerView::get_instance()->getTimer() + sprite_data[ANIM_TYPE_STAND][frame_n].duration;
    } else {
        next_frame_timer = TimerView::get_instance()->getTimer() + 100;
    }
    ImageView::get_instance()->renderTexturePortionAt(sprite_rect.x, sprite_rect.y, sprite_rect.w, sprite_rect.h, dest.x, dest.y, graphic_ref->texture);
    inc_sprite();
}

void character_animation::inc_sprite()
{
    if (TimerView::get_instance()->getTimer() < next_frame_timer) {
        return;
    }
    if (frame_n+1 >= ANIM_FRAMES_COUNT) {
        frame_n = 0;
    } else if (sprite_data[type][frame_n+1].used == false) {
        frame_n = 0;
    } else {
        frame_n++;
    }
}

ANIM_TYPE character_animation::get_type() const
{
    return type;
}

void character_animation::set_type(const ANIM_TYPE &value)
{
    type = value;
}

ANIM_DIRECTION character_animation::get_direction() const
{
    return direction;
}

void character_animation::set_direction(ANIM_DIRECTION value)
{
    direction = value;
}

