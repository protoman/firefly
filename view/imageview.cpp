#include "imageview.h"

extern SDL_Renderer* gRenderer;

ImageView* ImageView::_instance = nullptr;

#include <SDL3/SDL_surface.h>
#include "data/st_common.h"

#include "view/timerview.h"
#include "view/textview.h"
#include "game_data.h"

ImageView::ImageView()
{
    screen_scale_adjust.x = 0;
    screen_scale_adjust.y = 0;
    screen_scale_adjust.w = RES_W;
    screen_scale_adjust.h = RES_H;
    game_render_target = RENDER_TARGET_DIRECT_SCREEN;
}



void ImageView::copySDLPortion(st_rectangle original_rect, st_rectangle destiny_rect, SDL_Surface *surfaceOrigin, SDL_Surface *surfaceDestiny)
{
    SDL_Rect src, dest;
    src.x = original_rect.x;
    src.y = original_rect.y;
    src.w = original_rect.w;
    src.h = original_rect.h;
    dest.x = destiny_rect.x;
    dest.y = destiny_rect.y;
    dest.w = destiny_rect.w;
    dest.h = destiny_rect.h;

    if (!surfaceOrigin) {
        std::cout << "copySDLArea - ERROR surfaceOrigin is nullptr\n";
        return;
    }
    if (!surfaceDestiny) {
        std::cout << "FAIL: copySDLPortion - ERROR surfaceDestiny is nullptr - ignoring..." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (src.x >= surfaceOrigin->w || (src.x+src.w) > surfaceOrigin->w) {
        //printf(">> Invalid X portion src.x[%d], src.w[%d] for image.w[%d] <<\n", src.x, src.w, surfaceOrigin->w);
        fflush(stdout);
        return;
    }
    if (src.y >= surfaceOrigin->h || (src.y+src.h) > surfaceOrigin->h) {
        printf(">> Invalid Y portion - w[%d], h[%d] for image.w[%d] and image.h[%d] <<\n", src.y, src.h, surfaceOrigin->w, surfaceOrigin->h);
        fflush(stdout);
        return;
    }

    SDL_BlitSurface(surfaceOrigin, &src, surfaceDestiny, &dest);
}

void ImageView::draw_weapon_tooltip_icon(short weapon_n, st_position position, bool disabled)
{
    if (weapon_n >= weapon_icons.size()) {
        return;
    }
    int icon_size = weapon_icons.at(weapon_n).surface->w;
    if (disabled == true) {
        renderTexturePortionAt(0, 0, icon_size, icon_size, position.x, position.y, weapon_icons.at(weapon_n).texture);
    } else {
        renderTexturePortionAt(0, icon_size, icon_size, icon_size, position.x, position.y, weapon_icons.at(weapon_n).texture);
    }

}

void ImageView::change_render_target(e_RENDER_TARGET target)
{
    if (target == RENDER_TARGET_DIRECT_SCREEN) {
        SDL_SetRenderTarget(gRenderer, nullptr);
    } else if (target == RENDER_TARGET_GAME_TEXTURE) {
        SDL_SetRenderTarget(gRenderer, texture_render_target);
    } else if (target == RENDER_TARGET_HUD_TEXTURE) {
        SDL_SetRenderTarget(gRenderer, hud_texture_render_target);
    }
    game_render_target = target;
}

void ImageView::set_render_target(SDL_Texture* target)
{
    SDL_SetRenderTarget(gRenderer, target);
}

void ImageView::restore_render_target()
{
    if (game_render_target == RENDER_TARGET_DIRECT_SCREEN) {
        SDL_SetRenderTarget(gRenderer, nullptr);
    } else if (game_render_target == RENDER_TARGET_GAME_TEXTURE) {
        SDL_SetRenderTarget(gRenderer, texture_render_target);
    } else if (game_render_target == RENDER_TARGET_HUD_TEXTURE) {
        SDL_SetRenderTarget(gRenderer, hud_texture_render_target);
    }

}

SDL_Texture *ImageView::get_game_texture()
{
    return texture_render_target;
}

SDL_Texture *ImageView::get_hud_texture()
{
    return hud_texture_render_target;
}

e_RENDER_TARGET ImageView::get_current_target()
{
    return game_render_target;
}

void ImageView::set_fullscreen(bool mode)
{
    std::cout << "ImageView::set_fullscreen::START" << std::endl;
    if (mode == true) {
        SDL_SetWindowFullscreen(SharedData::get_instance()->window, SDL_WINDOW_FULLSCREEN);
    } else {
        SDL_SetWindowFullscreen(SharedData::get_instance()->window, 0);
    }

}

void ImageView::blend_images(st_imageData &source, st_imageData &dest, int x, int y)
{
    blend_images(source, dest, x, y, source.surface->w, source.surface->h);
}

void ImageView::blend_images(st_imageData &source, st_imageData &dest, int x, int y, int w, int h)
{

    SDL_SetRenderTarget(gRenderer, dest.texture);

    SDL_SetTextureBlendMode(source.texture, SDL_BLENDMODE_ADD);
    SDL_FRect origin = {0.0f, 0.0f, (float)source.surface->w, (float)source.surface->h};
    SDL_FRect destiny  = {(float)x, (float)y, (float)w, (float)h};
    SDL_RenderTexture(gRenderer, source.texture, &origin, &destiny);

    restore_render_target();
}


void ImageView::copyArea(st_imageData &origin, st_imageData &dest)
{
    copyArea(st_position(0, 0), origin, dest);
}


void ImageView::copyArea(st_position origin_pos, st_imageData &origin, st_imageData &dest)
{
    copyArea(st_rectangle(origin_pos.x, origin_pos.y, origin.surface->w, origin.surface->h), st_rectangle(0, 0, origin.surface->w, origin.surface->h), origin, dest);
}

void ImageView::copyAreaNoTexture(st_rectangle rect, st_position dest_pos, st_imageData &origin, st_imageData &dest)
{
    st_rectangle dest_rect(dest_pos.x, dest_pos.y, origin.surface->w, origin.surface->h);
    copySDLPortion(rect, dest_rect, origin.surface, dest.surface);
}

void ImageView::rebuildTexture(st_imageData &origin)
{
    SDL_DestroyTexture(origin.texture);
    origin.texture = SDL_CreateTextureFromSurface(gRenderer, origin.surface);
}

void ImageView::clear_surface(st_imageData &image)
{
    //SDL_Surface *dst, const SDL_Rect *rect, Uint32 color
    SDL_FillSurfaceRect(image.surface, nullptr, SDL_MapSurfaceRGB(image.surface, 0, 0, 0));
}

void ImageView::copyArea(st_rectangle rect, st_position dest_pos, st_imageData& origin, st_imageData& dest)
{
    copyArea(rect, st_rectangle(dest_pos.x, dest_pos.y, origin.surface->w, origin.surface->h), origin, dest);
}

void ImageView::copyArea(st_rectangle origin_rect, st_rectangle dest_rect, st_imageData &origin, st_imageData &dest)
{
    copySDLPortion(origin_rect, dest_rect, origin.surface, dest.surface);
    // re-generate texture
    SDL_DestroyTexture(dest.texture);
    dest.texture = SDL_CreateTextureFromSurface(gRenderer, dest.surface);
}

st_imageData ImageView::imageFromRegion(st_rectangle area, st_imageData &origin)
{
    st_imageData dest = initSurface(st_size(area.w, area.h));
    copyArea(area, st_position(0, 0), origin, dest);

    return dest;
}

void ImageView::set_spriteframe_surface(st_spriteFrame *frame_dest, st_imageData &originSurface)
{
    if (originSurface.is_null()) {
        std::cout << "ERROR: set_spriteframe_surface, originSurface is null" <<std::endl;
        exit(-1);
    }
    frame_dest->frameSurface = initSurface(st_size(originSurface.surface->w, originSurface.surface->h));
    copyArea(st_rectangle(0, 0, originSurface.surface->w, originSurface.surface->h), st_position(0, 0), originSurface, frame_dest->frameSurface);
}

void ImageView::drawCursor(st_position pos)
{
    TextView::get_instance()->renderText(pos.x, pos.y, st_color(250, 250, 250), false, ">");
}

void ImageView::eraseCursor(st_position pos)
{
    clearScreenArea(pos.x, pos.y, CURSOR_SPACING, CURSOR_SPACING, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);
}


void ImageView::place_face(std::string face_file, st_position pos)
{
    if (face_file == "") {
        return;
    }
    std::map<std::string, st_imageData>::iterator it;

    it = FACES_SURFACES.find(face_file);

    if (it == FACES_SURFACES.end()) {
        std::string filename = SharedData::get_instance()->FILEPATH + "images/faces/" + face_file;
        FACES_SURFACES[face_file] = imageFromFile(filename);
    }
    renderImageAt(pos.x, pos.y, FACES_SURFACES[face_file]);
}

st_position ImageView::get_config_menu_pos()
{
    return _config_menu_pos;
}

st_size ImageView::get_config_menu_size()
{
    return st_size(config_menu.surface->w, config_menu.surface->h);
}

void ImageView::show_btn_a(st_position btn_pos)
{
    renderImageAt(btn_pos.x, btn_pos.y, _btn_a_surface);
}

void ImageView::show_config_bg()
{
    renderImageAt(0, 0, config_menu);
}

st_imageData *ImageView::get_preloaded_image(e_PRELOADED_IMAGES image_n)
{
    return &preloaded_images[image_n];
}

void ImageView::draw_explosion(st_position pos)
{
    st_size explosion_size(56, 56);
    int max_frames = 7;

    if (_explosion_animation_pos >= max_frames) {
        _explosion_animation_pos = 0;
    }

    int anim_pos = _explosion_animation_pos*explosion_size.width;

    renderTexturePortionAt(anim_pos, 0, explosion_size.width, explosion_size.height, pos.x, pos.y, bomb_explosion_surface.texture);

    if (_explosion_animation_timer < TimerView::get_instance()->getTimer()) {
        _explosion_animation_pos++;
        _explosion_animation_timer = TimerView::get_instance()->getTimer() + PROJECTILE_DEFAULT_ANIMATION_TIME;
    }
}



void ImageView::init()
{
    load_icons();

    std::string filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/swamp.png";
    if (tileset.surface != nullptr) {
        tileset.freeGraphic();
    }
    tileset = imageFromFile(filename);
    if (tileset.surface == nullptr) {
        std::cout << "ERROR::GRAPHLIB::loadTileset: Could not find file '" << filename << "'\n";
    }

    // explosion used in death and bosses
    filename = SharedData::get_instance()->FILEPATH + "images/animations/explosion_boss.png";
    small_explosion = imageFromFile(filename);


    // bomb explosion
    filename = SharedData::get_instance()->FILEPATH + std::string("/images/animations/big_boss_explosion.png");
    bomb_explosion_surface = imageFromFile(filename);

    // --- ITEMS --- //
    filename = SharedData::get_instance()->FILEPATH + "images/animations/explosion_32.png";
    explosion32 = imageFromFile(filename);
    filename = SharedData::get_instance()->FILEPATH + "images/animations/explosion_16.png";
    explosion16 = imageFromFile(filename);
    filename = SharedData::get_instance()->FILEPATH + "images/animations/player_death_explosion.png";
    explosion_player_death = imageFromFile(filename);
    filename = SharedData::get_instance()->FILEPATH + "images/animations/dash_dust.png";
    dash_dust = imageFromFile(filename);
    filename = SharedData::get_instance()->FILEPATH + "images/animations/water_splash.png";
    water_splash = imageFromFile(filename);
    filename = SharedData::get_instance()->FILEPATH + "images/sprites/objects/armor_arms.png";
    filename = SharedData::get_instance()->FILEPATH + "images/animations/explosion_32.png";
    preloaded_images[PRELOADED_IMAGES_EXPLOSION_BUBBLE] = imageFromFile(filename);
    set_surface_alpha(120, preloaded_images[PRELOADED_IMAGES_EXPLOSION_BUBBLE]);

    texture_render_target = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_W, AREA_H);
    hud_texture_render_target = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_W, HUD_H);
}

void ImageView::load_layers_data()
{
    // MAP-LAYERS //
    //std::cout << ">>>>> ImageView::init - loadeding layers..." << std::endl;
    for (auto const& item : map_data::SharedMapData::get_instance()->layer_order_map) {
        if (item.second == map_data::map_layer_type_image) {
            std::string filename = map_data::SharedMapData::get_instance()->layer_data_map[item.first].image_filename;
            map_data::SharedMapData::get_instance()->layer_data_map[item.first].image_data = imageFromFile(filename);
            int image_alpha = map_data::SharedMapData::get_instance()->layer_data_map[item.first].opacity * 255;
            set_surface_alpha(image_alpha, map_data::SharedMapData::get_instance()->layer_data_map[item.first].image_data);
        }
    }
}


void ImageView::preload()
{
    // projectile images
    int total_projectile = GameData::get_instance()->get_projectile_list_size();
    for (int i=0; i<total_projectile; i++) {
        std::string filename(GameData::get_instance()->get_projectile(i).graphic_filename);
        filename = SharedData::get_instance()->FILEPATH + "images/projectiles/" + filename;
        projectile_surface.push_back(st_surface_with_direction());
        if (filename.length() > 0 && filename.find(".png") != std::string::npos) {
            //std::cout << "GRAPHLIB::preload_images - i[" << i << "], list.size[" << projectile_surface.size() << "]" << std::endl;
            projectile_surface.at(i).surface[ANIM_DIRECTION_RIGHT] = imageFromFile(filename);
            flip_image(projectile_surface.at(i).surface[ANIM_DIRECTION_RIGHT], projectile_surface.at(i).surface[ANIM_DIRECTION_LEFT], flip_type_horizontal);
        }
    }

    unsigned int max = GameData::get_instance()->anim_tile_list.size();
    //std::cout << "graphicsLib::preload_anim_tiles - max: " << max << std::endl;
    for (int i=0; i<max; i++) {
        std::string file(GameData::get_instance()->anim_tile_list.at(i).filename);
        if (file.empty()) {
            //std::cout << "### graphicsLib::preload_anim_tiles::STOP, file: " << file << std::endl;
            break;
        } else {
            std::string filename = SharedData::get_instance()->FILEPATH + std::string("images/tilesets/anim/") + file;

            ANIM_TILES_SURFACES.emplace_back();
            ANIM_TILES_SURFACES.at(ANIM_TILES_SURFACES.size()-1) = imageFromFile(filename);

            int frames_n = ANIM_TILES_SURFACES.at(ANIM_TILES_SURFACES.size()-1).surface->w / TILESIZE;
            anim_tile_timer anim_timer(frames_n, TimerView::get_instance()->getTimer() + GameData::get_instance()->anim_tile_list.at(i).frame_delay[0]);

            for (int j=0; j<FS_ANIM_TILE_MAX_FRAMES; j++) {
                anim_timer.frames_delay[j] = GameData::get_instance()->anim_tile_list.at(i).frame_delay[j];
            }

            ANIM_TILES_TIMERS.push_back(anim_timer);
        }
    }

    // object icons
    icon_bg = imageFromFile(SharedData::get_instance()->FILEPATH + std::string("images/backgrounds/icon.png"));
    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        // TODO: border and image size
        std::string filename = SharedData::get_instance()->FILEPATH + std::string("images/sprites/objects/") + GameData::get_instance()->v6_object_list.at(i).graphic_filename;
        object_icon_map.insert(std::pair<int, st_imageData>(i, imageFromFile(filename)));
    }

}

void ImageView::inc_scale(float inc)
{
    screen_scale += inc;
    screen_scale_adjust.x = RES_W-(RES_W*screen_scale);
    screen_scale_adjust.y = RES_H-(RES_H*screen_scale);
    screen_scale_adjust.w = RES_W;
    screen_scale_adjust.h = RES_H;
}

float ImageView::get_scale()
{
    return screen_scale;
}

void ImageView::reset_scale()
{
    screen_scale = 1.0;
    screen_scale_adjust.x = RES_W-(RES_W*screen_scale);
    screen_scale_adjust.y = RES_H-(RES_H*screen_scale);
    screen_scale_adjust.w = RES_W;
    screen_scale_adjust.h = RES_H;
}

st_position ImageView::calc_rotated_position(st_imageData &original, st_imageData &rotated)
{
    int previous_w = original.surface->w;
    int previous_h = original.surface->h;
    st_position position;
    position.x += (previous_w - rotated.surface->w)/2;
    position.y += (previous_h - rotated.surface->h)/2;

    return position;
}

void ImageView::show_item_tooltip(st_position pos, int obj_id)
{
    //std::cout << ">>>>>>>>>>>>>>>>>> show_item_tooltip <<<<<<<<<<<<<<<<<<<<<" << std::endl;
    renderImageAt(pos.x-TILESIZE/2, pos.y-TILESIZE, icon_bg);
    renderImageAt(pos.x-TILESIZE/2, pos.y-TILESIZE, object_icon_map.at(obj_id));
}

void ImageView::change_render_size()
{
    // TODO: keep aspect ratio
    float scaleX = (float)SharedData::get_instance()->window_size.width / RES_W;
    float scaleY = (float)SharedData::get_instance()->window_size.height / RES_H;

    std::cout << ">>>>>>>> ImageView::change_render_size - w[" << SharedData::get_instance()->window_size.width << "], h[" << SharedData::get_instance()->window_size.height << "], scaleX[" << scaleX << "], scaleY[" << scaleY << "]" << std::endl;

    if (scaleX > scaleY) {
        scaleX = scaleY;
    } else if (scaleY > scaleX) {
        scaleY = scaleX;
    }

    SDL_SetRenderScale(gRenderer, scaleX, scaleY);
    SDL_SetWindowSize(SharedData::get_instance()->window, RES_W*scaleX, RES_H*scaleY);
}

ImageView* ImageView::get_instance()
{
    if (!_instance) {
        _instance = new ImageView();
    }
    return _instance;
}

void ImageView::copyScreenAreaToImage(int origin_x, int origin_y, int origin_w, int origin_h, int dest_x, int dest_y, st_imageData image) {
    SDL_FRect origin_rect = SDL_FRect(origin_x, origin_y, origin_w, origin_h);
    SDL_FRect destiny_rect = SDL_FRect(dest_x, dest_y, origin_w, origin_h);
    SDL_RenderTexture(gRenderer, image.texture, &origin_rect, &destiny_rect);
    rebuildTexture(image);
}

st_imageData ImageView::imageFromFile(const std::string &filename) {
    st_imageData res;
    //The final texture
    SDL_Texture* newTexture = nullptr;
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( filename.c_str() );
    if( loadedSurface == nullptr ) {
        printf( "FAIL: Unable to load image %s! SDL_image Error: %s\n", filename.c_str(), SDL_GetError() );
        exit(EXIT_FAILURE);
    } else {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == nullptr) {
            printf( "FAIL: Unable to create texture from [%s]. SDL Error: %s\n", filename.c_str(), SDL_GetError() );
            SDL_DestroySurface( loadedSurface );
            exit(EXIT_FAILURE);
        }
        res.surface = loadedSurface;
        res.texture = newTexture;
        //Get rid of old loaded surface
    }
    return res;
}

void ImageView::renderTexturePortionAt(int origin_x, int origin_y, int origin_w, int origin_h, int dest_x, int dest_y, SDL_Texture *texture)
{
    SDL_FRect origin = {(float)origin_x, (float)origin_y, (float)origin_w, (float)origin_h};
    SDL_FRect dest  = {(float)dest_x, (float)dest_y, (float)origin_w, (float)origin_h};
    SDL_RenderTexture(gRenderer, texture, &origin, &dest);
}

void ImageView::renderImageAt(int dest_x, int dest_y, st_imageData &image)
{
    renderTexturePortionAt(0, 0, image.surface->w, image.surface->h, dest_x, dest_y, image.texture);
}


void ImageView::clearScreenArea(short x, short y, short w, short h, short r, short g, short b)
{
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(gRenderer, r, g, b, 255);
    SDL_RenderFillRect(gRenderer, &rect);
}

void ImageView::updateRender()
{
    //SDL_RenderSetViewport(gRenderer, &screen_scale_adjust);

    //renderTexturePortionAt(0, 0, RES_W, HUD_H, 0, AREA_H-50, hud_texture_render_target);

    SDL_RenderPresent(gRenderer);
}

st_imageData ImageView::initSurface(st_size size)
{
    st_imageData res;
    res.surface = SDL_CreateSurface(size.width, size.height, SDL_GetPixelFormatForMasks(VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000));
    if (res.surface == nullptr) {
        exit(EXIT_FAILURE);
    }
    res.texture = SDL_CreateTextureFromSurface(gRenderer, res.surface);

    return res;
}

void ImageView::init_target_image(st_imageData& image, int w, int h)
{
    image.surface = SDL_CreateSurface(w, h, SDL_GetPixelFormatForMasks(VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000));
    if (image.surface == nullptr) {
        exit(EXIT_FAILURE);
    }
    image.texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

}

void ImageView::clear_surface_area(short x, short y, short w, short h, short r, short g, short b, st_imageData &image)
{
    SDL_Rect dest;
    if (image.surface == nullptr) {
        return;
    }
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_FillSurfaceRect(image.surface, &dest, SDL_MapSurfaceRGB(image.surface, r, g, b));
    rebuildTexture(image);
}

void ImageView::clear_texture_area(short x, short y, short w, short h, Uint8 r, Uint8 g, Uint8 b, Uint8 alpha, st_imageData &image)
{
    SDL_SetRenderTarget(gRenderer, image.texture);
    SDL_FRect dest;
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_SetRenderDrawColor(gRenderer, r, g, b, alpha);
    SDL_SetTextureBlendMode(image.texture, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(gRenderer, &dest);
    //SDL_SetRenderDrawColor(gRenderer, r, g, b, alpha);
    //SDL_RenderFillRect(gRenderer, nullptr);
    restore_render_target();
}

void ImageView::set_surface_alpha(int alpha, st_imageData &image)
{
    if (image.surface->w <= 0 || image.surface == nullptr) {
        std::cout << "[WARNING] GRAPHLIB::set_surface_alpha[&] - invalid surface, ignoring" << std::endl;
        return;
    }
    //std::cout << "ImageView::set_surface_alpha[" << alpha << "]" << std::endl;
    SDL_SetTextureAlphaMod(image.texture, alpha);
}

void ImageView::update_anim_tiles_timers()
{
    for (int anim_tile_id=0; anim_tile_id<ANIM_TILES_TIMERS.size(); anim_tile_id++) {
        if (ANIM_TILES_TIMERS.at(anim_tile_id).timer < TimerView::get_instance()->getTimer()) {
            ANIM_TILES_TIMERS.at(anim_tile_id).frame_pos++;
            if (ANIM_TILES_TIMERS.at(anim_tile_id).frame_pos >= ANIM_TILES_TIMERS.at(anim_tile_id).max_frames) {
                ANIM_TILES_TIMERS.at(anim_tile_id).frame_pos = 0;
            }
            ANIM_TILES_TIMERS.at(anim_tile_id).timer = TimerView::get_instance()->getTimer() + ANIM_TILES_TIMERS.at(anim_tile_id).frames_delay[ANIM_TILES_TIMERS.at(anim_tile_id).frame_pos];
        }
    }
}

void ImageView::place_anim_tile(int anim_tile_id, st_position pos_destiny)
{
    //std::cout << "place_anim_tile - id[" << anim_tile_id << "]" << std::endl;
    if (anim_tile_id >= ANIM_TILES_SURFACES.size()) {
        std::cout << "place_anim_tile - ERROR Invalid anim-tile-id: " << anim_tile_id << " - ignoring..." << std::endl;
        return;
    }

    struct st_imageData* tile_ref = &ANIM_TILES_SURFACES.at(anim_tile_id);

    if (tile_ref->surface == nullptr) {
        std::cout << "place_anim_tile - ERROR surfaceDestiny is nullptr for id " << anim_tile_id << " - ignoring..." << std::endl;
        char debug_msg[255];
        sprintf(debug_msg, "EXIT:place_anim_tile[%d][%d]", anim_tile_id, ANIM_TILES_SURFACES.size());
        return;
    }


    struct st_rectangle origin_rectangle;

    origin_rectangle.x = ANIM_TILES_TIMERS.at(anim_tile_id).frame_pos * TILESIZE;
    origin_rectangle.y = 0;
    origin_rectangle.w = TILESIZE;
    origin_rectangle.h = TILESIZE;

    renderTexturePortionAt(origin_rectangle.x, origin_rectangle.y, origin_rectangle.w, origin_rectangle.h, pos_destiny.x, pos_destiny.y, tile_ref->texture);
}

void ImageView::place_easymode_block_tile(st_position destiny, st_imageData &surface)
{
    copyArea(st_rectangle(0, 0, TILESIZE, TILESIZE), st_rectangle(destiny.x, destiny.y, TILESIZE, TILESIZE), _easymode_block, surface);
}

void ImageView::place_hardmode_block_tile(st_position destiny, st_imageData &surface)
{
    copyArea(st_rectangle(0, 0, TILESIZE, TILESIZE), st_rectangle(destiny.x, destiny.y, TILESIZE, TILESIZE), _hardmode_block, surface);
}

void ImageView::placeTile(st_position origin_pos, st_position dest_pos, st_imageData &dest)
{
    if (!dest.surface) {
        std::cout << "placeTile - ERROR surfaceDestiny is nullptr - ignoring..." << std::endl;
        return;
    }
    if (!tileset.surface || tileset.is_null()) {
        std::cout << "placeTile - ERROR ImageView::placeTile.tileset is nullptr - ignoring..." << std::endl;
        return;
    }

    struct st_rectangle origin_rectangle;

    origin_rectangle.x = origin_pos.x * TILESIZE;
    origin_rectangle.y = origin_pos.y * TILESIZE;

    origin_rectangle.w = TILESIZE;
    origin_rectangle.h = TILESIZE;


    copyAreaNoTexture(origin_rectangle, dest_pos, tileset, dest);

}

void ImageView::placeSlope(st_rectangle origin_pos, st_position dest_pos, st_imageData &origin, st_imageData &dest)
{
    if (!dest.surface) {
        std::cout << "placeTile - ERROR surfaceDestiny is nullptr - ignoring..." << std::endl;
        return;
    }
    if (!origin.surface) {
        std::cout << "placeTile - ERROR surfaceOrigin is nullptr - ignoring..." << std::endl;
        return;
    }

    struct st_rectangle origin_rectangle;

    origin_rectangle.x = origin_pos.x;
    origin_rectangle.y = origin_pos.y;

    origin_rectangle.w = origin_pos.w;
    origin_rectangle.h = origin_pos.h;


    copyAreaNoTexture(origin_rectangle, dest_pos, origin, dest);
}

void ImageView::place_3rd_level_tile(int origin_x, int origin_y, int dest_x, int dest_y)
{
    st_position pos_destiny(dest_x, dest_y);
    if (origin_x < -1) {
        int anim_tile_id = (origin_x * -1) - 2;
        place_anim_tile(anim_tile_id, pos_destiny);
        return;
    }

    struct st_rectangle origin_rectangle(origin_x*TILESIZE, origin_y*TILESIZE, TILESIZE, TILESIZE);

    if (origin_rectangle.x < 0 || origin_rectangle.x > tileset.surface->w) {
        std::cout << "[WARNING] GRAPHLIB::place_tile - invalid position #1, ignoring. origin.x[" << origin_x << "], origin.y[" << origin_y << "], origin.w[" << origin_rectangle.w << "]" << std::endl;
        return;
    } else if (origin_rectangle.y < 0 || origin_rectangle.y> tileset.surface->h) {
        std::cout << "[WARNING] GRAPHLIB::place_tile - invalid position #2, ignoring. origin.x[" << origin_x << "], origin.y[" << origin_y << "], origin.w[" << origin_rectangle.w << "]" << std::endl;
        return;
    //} else {
        //std::cout << "GRAPHLIB::place_3rd_level_tile - origin.x[" << origin_x << "], origin.y[" << origin_y << "]" << std::endl;
    }

    renderTexturePortionAt(origin_rectangle.x, origin_rectangle.y, origin_rectangle.w, origin_rectangle.h, pos_destiny.x, pos_destiny.y, tileset.texture);
}

void ImageView::load_icons()
{

    std::string filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/weapon_menu.png";
    ingame_menu = imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/btn_a.png";
    _btn_a_surface = imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/easymode.png";
    _easymode_block = imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/hardmode.png";
    _hardmode_block = imageFromFile(filename);
}

void ImageView::flip_image(st_imageData &original, st_imageData &res, e_flip_type flip_mode)
{
    // TODO - SDL3 version, for now just copy it //
    res = initSurface(st_size(original.surface->w, original.surface->h));
    copyArea(original, res);
}

void ImageView::blink_screen(int r, int g, int b)
{
    // @TODO::IURI //
}

