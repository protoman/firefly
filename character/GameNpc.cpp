#include "GameNpc.h"

#include "game_data.h"
#include "data/shareddata.h"
#include "view/imageview.h"
#include "view/timerview.h"

#define REQUEST_ITEM_TOOLTIP_DURATION 3000


GameNPC::GameNPC(int map_id, int main_id, int id)
{
    is_ghost = false;
    facing = GameData::get_instance()->file_v5_stage_npc_map.at(map_id).at(id).direction;
    start_point.x = GameData::get_instance()->file_v5_stage_npc_map.at(map_id).at(id).start_point.x * TILESIZE;
    start_point.y = GameData::get_instance()->file_v5_stage_npc_map.at(map_id).at(id).start_point.y * TILESIZE;
    position.x = start_point.x;
    position.y = start_point.y;
    status = 0;
    is_ghost = false;
    set_direction(facing);
    build_basic_npc(map_id, main_id);
}

bool GameNPC::have_fly_movement()
{
    return false;
}

void GameNPC::initFrames()
{
    std::cout << "GameNPC::initFrames #1" << std::endl;
    file_npc_v3_1_2 *npc_ref = GameData::get_instance()->get_npc(_number);
    std::string temp_filename = SharedData::get_instance()->FILEPATH + "images/sprites/npcs/" + GameData::get_instance()->get_npc(_number)->graphic_filename;
    st_imageData npc_sprite_surface = ImageView::get_instance()->imageFromFile(temp_filename);
    if (npc_sprite_surface.surface == nullptr) {
        std::cout << "initFrames - Error loading NPC surface from file" << std::endl;
        return;
    }
    graphic_size.width = npc_sprite_surface.surface->w;
    graphic_size.height = npc_sprite_surface.surface->h;
    frameSize.width = npc_ref->frame_width;
    frameSize.height = npc_sprite_surface.surface->h;

    for (int i=0; i<ANIM_TYPE_NPC_COUNT; i++) {
        for (int j=0; j<ANIM_TYPE_NPC_FRAMES_N; j++) {
            if (GameData::get_instance()->get_npc(_number)->sprites[i][j].used == false) {
                //std::cout << "### Adding frame for npc[" << name << "], type[" << i << "], frame[" << j << "] - START" << std::endl;
                //int anim_type, int posX, st_imageData &spritesSurface, int delay
                int pos_x = GameData::get_instance()->get_npc(_number)->sprites[i][j].sprite_graphic_pos_x;
                int duration = GameData::get_instance()->get_npc(_number)->frame_duration;
                //std::cout << "GameNPC::initFrames[" << name << "], sprites[" << i << "], j[" << j << "], pos_x[" << pos_x << "], duration[" << duration << "]" << std::endl;
                addNpcSpriteFrame(i, pos_x, npc_sprite_surface, duration);
                //std::cout << "### Adding frame for npc[" << name << "], type[" << i << "], frame[" << j << "] - DONE" << std::endl;
            }

        }
    }
    std::cout << "GameNPC::initFrames #2" << std::endl;
}

void GameNPC::execute()
{

}

void GameNPC::show()
{
#ifdef SHOW_HITBOXES
    st_rectangle hitbox = get_hitbox();
    hitbox.x -= gameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
    ImageView::get_instance()->draw_rectangle(hitbox, 0, 0, 255, 100);
#endif
    if (npc_request_item_tooltip_timer != 0 && npc_request_item_tooltip_timer > TimerView::get_instance()->getTimer()) {
        st_position tooltip_pos = this->get_real_position();
        tooltip_pos.x += this->get_size().width/2;
        ImageView::get_instance()->show_item_tooltip(tooltip_pos, GameData::get_instance()->get_enemy(_number)->npc_requested_item_id);
    } else if (npc_request_item_tooltip_timer != 0) {
        npc_request_item_tooltip_timer = 0;
    }
    character::show();
}

void GameNPC::npc_activate_request_item_tooltip()
{
    npc_request_item_tooltip_timer = TimerView::get_instance()->getTimer() + REQUEST_ITEM_TOOLTIP_DURATION;
    std::cout << "current-timer[" << TimerView::get_instance()->getTimer() << "], npc_request_item_tooltip_timer[" << npc_request_item_tooltip_timer << "]" << std::endl;
}

int GameNPC::get_id()
{
    return _number;
}

void GameNPC::death()
{

}

void GameNPC::build_basic_npc(int map_id, int main_id)
{
    _number = main_id;
    // TODO - usar operador igual e também para cópia de toda a classe para ela mesma
    st_imageData npc_sprite_surface;

    name = std::string("NPC:") + std::string(GameData::get_instance()->get_npc(_number)->name);
    hitPoints.total = 99;
    hitPoints.current = hitPoints.total;

    if (state.direction > CHAR_ANIM_DIRECTION_COUNT) {
        set_direction(ANIM_DIRECTION_RIGHT);
    }

    move_speed = GameData::get_instance()->get_npc(_number)->speed;
    walk_range = GameData::get_instance()->get_npc(_number)->walk_range;
    if (walk_range < 0 || walk_range > 1000) { // fix data errors by setting value to default
        walk_range = TILESIZE*6;
    }


    add_graphic();
    initFrames();

    std::string graphic_filename = GameData::get_instance()->get_npc(_number)->graphic_filename;


    frameSize.width = GameData::get_instance()->get_npc(_number)->frame_width;
    frameSize.height = graphic_size.height;
    is_ghost = false;
    shield_type = 0;
    _is_boss = false;
    _attack_frame_n = 0;
    relativePosition.x = 0;
    relativePosition.y = 0;
    max_projectiles = 1;			// hardcoded, fix this in editor
    attack_state = 0;
    last_execute_time = 0;
    can_fly = have_fly_movement();
    vulnerable_area_box = GameData::get_instance()->get_enemy(_number)->vulnerable_area;
}

void GameNPC::addNpcSpriteFrame(int anim_type, int posX, st_imageData &spritesSurface, int delay)
{
    struct st_rectangle spriteArea;

    spriteArea.x = posX*frameSize.width;
    spriteArea.y = 0;
    spriteArea.w = frameSize.width;
    spriteArea.h = frameSize.height;

    // ANIM_TYPE_STAIRS_MOVE and ANIM_TYPE_STAIRS_SEMI have an extra frame that is the mirror of the first one

    for (int anim_direction=0; anim_direction<=1; anim_direction++) {
        for (int i=0; i<ANIM_FRAMES_COUNT; i++) { // find the last free frame
            if ((ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i].frameSurface.surface == nullptr) {

                st_spriteFrame *sprite = &(ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i];
                st_imageData gsurface = ImageView::get_instance()->imageFromRegion(spriteArea, spritesSurface);
                //std::cout << "### GameNPC::addNpcSpriteFrame - spritesSurface.w[" << spritesSurface.surface->w << "], spritesSurface.h[" << spritesSurface.surface->h << "], gsurface.w[" << gsurface.surface->w << "], gsurface.h[" << gsurface.surface->h << "]" << std::endl;
                // RIGHT
                if (anim_direction != 0) {
                    ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface);
                // LEFT
                } else {
                    st_imageData gsurface_flip;
                    ImageView::get_instance()->flip_image(gsurface, gsurface_flip, flip_type_horizontal);
                    ImageView::get_instance()->set_spriteframe_surface(sprite, gsurface_flip);
                    //std::cout << "### GameNPC::addNpcSpriteFrame[LEFT] - DONE" << std::endl;
                }
                //(ImageView::get_instance()->character_graphics_list.find(name)->second).frames[anim_direction][anim_type][i].delay = delay;
                break;
            }
        }
    }
}
