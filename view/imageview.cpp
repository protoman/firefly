#include "imageview.h"

#include <SDL2/SDL2_rotozoom.h>

extern SDL_Renderer* gRenderer;

ImageView* ImageView::_instance = nullptr;

#include "data/st_common.h"

#include "view/timerview.h"
#include "view/textview.h"
#include "game_mediator.h"

ImageView::ImageView()
{

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
        std::cout << "FAILR: copySDLPortion - ERROR surfaceDestiny is nullptr - ignoring..." << std::endl;
        //return;
        exit(EXIT_FAILURE);
    }

    if (src.x >= surfaceOrigin->w || (src.x+src.w) > surfaceOrigin->w) {
        //printf(">> Invalid X portion src.x[%d], src.w[%d] for image.w[%d] <<\n", src.x, src.w, surfaceOrigin->w);
        fflush(stdout);
        return;
    }
    if (src.y >= surfaceOrigin->h || (src.y+src.h) > surfaceOrigin->h) {
        printf(">> Invalid Y portion[%d] h[%d] for image.w[%d] and image.h[%d] <<\n", src.y, src.h, surfaceOrigin->w, surfaceOrigin->h);
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
    if (target == RENDER_TARGET_SCREEN) {
        SDL_SetRenderTarget(gRenderer, nullptr);
    } else if (target == RENDER_TARGET_TEXTURE) {
        SDL_SetRenderTarget(gRenderer, texture_render_target);
    }
}

SDL_Texture *ImageView::get_texture_renderer()
{
    return texture_render_target;
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
    SDL_FillRect(image.surface, nullptr, SDL_MapRGBA(image.surface->format, 0, 0, 0, SDL_ALPHA_TRANSPARENT));
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

void ImageView::show_dialog(Uint8 position)
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
    renderImageAt(bg_pos.x, bg_pos.y, dialog_surface);

}

void ImageView::show_dialog_button(Uint8 position)
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

    show_btn_a(st_position(posX+dialog_surface.surface->w-_btn_a_surface.surface->w-2-TILESIZE, posY+dialog_surface.surface->h-_btn_a_surface.surface->h-TILESIZE/2));
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

st_position ImageView::get_dialog_pos() const
{
    return _config_menu_pos;
}

st_position ImageView::get_config_menu_pos() const
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

void ImageView::zoom_image(st_position dest, st_imageData &picture, int smooth)
{
    SDL_Surface *rotozoom_picture;
    st_position center(dest.x+picture.surface->w/2, dest.y+picture.surface->h/2);
    std::cout << ">>>>>>>>>>> dest[" << dest.x << "][" << dest.y << "]" << std::endl;
    std::cout << "center[" << center.x << "][" << center.y << "]" << std::endl;

    for (float i=0.1; i<1.0; i+=0.03) {
        if ((rotozoom_picture = zoomSurface(picture.surface, i, i, smooth)) != NULL) {
        //double angle = 360*i;
        //if ((rotozoom_picture = rotozoomSurface(picture.get_surface(), angle, 1.0, smooth)) != NULL) {
            std::cout << "GRAPHLIB::ZOOM #1 [" << i << "]" << std::endl;
            struct st_rectangle origin_rectangle(0, 0, rotozoom_picture->w, rotozoom_picture->h);

            st_position dest_zoom(center.x-rotozoom_picture->w/2, center.y-rotozoom_picture->h/2);
            std::cout << "rotozoom_picture[" << rotozoom_picture->w << "][" << rotozoom_picture->h << "]" << std::endl;
            std::cout << "dest_zoom[" << dest_zoom.x << "][" << dest_zoom.y << "]" << std::endl;

            // clear area
            clearScreenArea(dest_zoom.x, dest_zoom.y, rotozoom_picture->w, rotozoom_picture->h, CONFIG_BGCOLOR_R, CONFIG_BGCOLOR_G, CONFIG_BGCOLOR_B);

            SDL_Texture* newTexture = nullptr;
            newTexture = SDL_CreateTextureFromSurface(gRenderer, rotozoom_picture);
            renderTexturePortionAt(origin_rectangle.x, origin_rectangle.y, origin_rectangle.w, origin_rectangle.h, dest_zoom.x, dest_zoom.y, newTexture);
            updateScreen();
            TimerView::get_instance()->delay(20);
            SDL_FreeSurface(rotozoom_picture);
            SDL_DestroyTexture(newTexture);
        } else {
            std::cout << "Error creating zoomed surface" << std::endl;
        }
    }

    /* Pause for a sec */
    SDL_Delay(100);
}

void ImageView::rotate_image(st_imageData &picture, double angle)
{
    SDL_Surface *rotozoom_picture;

    if ((rotozoom_picture = rotozoomSurface(picture.surface, angle, 1.0, true)) != nullptr) {
        if (picture.texture != nullptr) {
            SDL_DestroyTexture(picture.texture);
        }
        picture.texture = SDL_CreateTextureFromSurface(gRenderer, rotozoom_picture);
    } else {
        std::cout << "GRAPHLIB::rotate_image - Error generating rotated image" << std::endl;
    }
}

void ImageView::rotated_from_image(st_imageData &picture, st_imageData &dest, double angle)
{
    SDL_Surface *rotozoom_picture;
    if ((rotozoom_picture = rotozoomSurface(picture.surface, angle, 1.0, true)) != nullptr) {
        if (dest.surface != nullptr) {
            SDL_FreeSurface(dest.surface);
        }
        dest.surface = rotozoom_picture;
        if (picture.texture != nullptr) {
            SDL_DestroyTexture(picture.texture);
        }
        picture.texture = SDL_CreateTextureFromSurface(gRenderer, rotozoom_picture);
    } else {
        std::cout << "GRAPHLIB::rotate_image - Error generating rotated image" << std::endl;
    }

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
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not initialize! SDL_image Error[" << IMG_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    load_icons();

    std::string filename = SharedData::get_instance()->FILEPATH + "images/tilesets/default.png";
    if (tileset.surface != nullptr) {
        tileset.freeGraphic();
    }
    tileset = imageFromFile(filename);
    if (tileset.surface == nullptr) {
        std::cout << "ERROR::GRAPHLIB::loadTileset: Could not find file '" << filename << "'\n";
    }

    // explision used in death and bosses
    filename = SharedData::get_instance()->FILEPATH + "images/animations/explosion_boss.png";
    small_explosion = imageFromFile(filename);

    // projectile images
    for (int i=0; i<GameMediator::get_instance()->get_projectile_list_size(); i++) {
        std::string filename(GameMediator::get_instance()->get_projectile(i).graphic_filename);
        filename = SharedData::get_instance()->FILEPATH + "images/projectiles/" + filename;
        projectile_surface.push_back(st_surface_with_direction());
        if (filename.length() > 0 && filename.find(".png") != std::string::npos) {
            //std::cout << "GRAPHLIB::preload_images - i[" << i << "], list.size[" << projectile_surface.size() << "]" << std::endl;
            projectile_surface.at(i).surface[ANIM_DIRECTION_RIGHT] = imageFromFile(filename);
            flip_image(projectile_surface.at(i).surface[ANIM_DIRECTION_RIGHT], projectile_surface.at(i).surface[ANIM_DIRECTION_LEFT], flip_type_horizontal);
        }
    }

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

    int max = GameMediator::get_instance()->anim_tile_list.size();
    //std::cout << "graphicsLib::preload_anim_tiles - max: " << max << std::endl;
    for (int i=0; i<max; i++) {
        std::string file(GameMediator::get_instance()->anim_tile_list.at(i).filename);
        if (file.length() < 1) {
            //std::cout << "### graphicsLib::preload_anim_tiles::STOP, file: " << file << std::endl;
            break;
        } else {
            std::string filename = SharedData::get_instance()->FILEPATH + std::string("images/tilesets/anim/") + file;

            ANIM_TILES_SURFACES.push_back(st_imageData());
            ANIM_TILES_SURFACES.at(ANIM_TILES_SURFACES.size()-1) = imageFromFile(filename);

            int frames_n = ANIM_TILES_SURFACES.at(ANIM_TILES_SURFACES.size()-1).surface->w / TILESIZE;
            anim_tile_timer anim_timer(frames_n, TimerView::get_instance()->getTimer() + GameMediator::get_instance()->anim_tile_list.at(i).frame_delay[0]);

            for (int j=0; j<FS_ANIM_TILE_MAX_FRAMES; j++) {
                anim_timer.frames_delay[j] = GameMediator::get_instance()->anim_tile_list.at(i).frame_delay[j];
            }

            ANIM_TILES_TIMERS.push_back(anim_timer);
        }
    }

    texture_render_target = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_W, AREA_H);

}

ImageView* ImageView::get_instance()
{
    if (!_instance) {
        _instance = new ImageView();
    }
    return _instance;
}

void ImageView::copyScreenAreaToImage(int origin_x, int origin_y, int origin_w, int origin_h, int dest_x, int dest_y, st_imageData image)
{
    copySDLPortion(st_rectangle(origin_x, origin_y, origin_w, origin_h), st_rectangle(dest_x, dest_y, origin_w, origin_h), SharedData::get_instance()->screenSurface, image.surface);
    rebuildTexture(image);

}



st_imageData ImageView::imageFromFile(std::string filename)
{
    st_imageData res;
    //The final texture
    SDL_Texture* newTexture = nullptr;
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( filename.c_str() );
    if( loadedSurface == nullptr ) {
        printf( "FAIL: Unable to load image %s! SDL_image Error: %s\n", filename.c_str(), IMG_GetError() );
        exit(EXIT_FAILURE);
    } else {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == nullptr) {
            printf( "FAIL: Unable to create texture from [%s]. SDL Error: %s\n", filename.c_str(), SDL_GetError() );
            SDL_FreeSurface( loadedSurface );
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
    SDL_Rect origin = {origin_x, origin_y, origin_w, origin_h};
    SDL_Rect dest  = {dest_x, dest_y, origin_w, origin_h};
    SDL_RenderCopy(gRenderer, texture, &origin, &dest);
}

void ImageView::renderImageAt(int dest_x, int dest_y, st_imageData &image)
{
    renderTexturePortionAt(0, 0, image.surface->w, image.surface->h, dest_x, dest_y, image.texture);
}


void ImageView::clearScreenArea(short x, short y, short w, short h, short r, short g, short b)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(gRenderer, r, g, b, 255);
    SDL_RenderFillRect(gRenderer, &rect);
}

void ImageView::updateScreen()
{
    SDL_RenderPresent(gRenderer);
}

st_imageData ImageView::initSurface(st_size size)
{
    st_imageData res;
    res.surface = SDL_CreateRGBSurface(SDL_RLEACCEL , size.width, size.height, VIDEO_MODE_COLORS, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    if (res.surface == nullptr) {
        exit(EXIT_FAILURE);
    }
    res.texture = SDL_CreateTextureFromSurface(gRenderer, res.surface);

    return res;
}

void ImageView::clear_surface_area(short x, short y, short w, short h, short r, short g, short b, st_imageData &image) const
{
    SDL_Rect dest;
    if (image.surface == nullptr || image.surface->format == nullptr) {
        return;
    }
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_FillRect(image.surface, &dest, SDL_MapRGB(image.surface->format, r, g, b));
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
    std::string filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/dialog.png";
    dialog_surface = imageFromFile(filename);

    filename = SharedData::get_instance()->FILEPATH + "images/backgrounds/weapon_menu.png";
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
    res = original;

    //If the surface must be locked
    if (SDL_MUSTLOCK( original.surface )) {
        //Lock the surface
        SDL_LockSurface( original.surface );
    }

    //Go through columns
    for (int x = 0, rx = original.surface->w - 1; x < original.surface->w; x++, rx-- ) {
        //Go through rows
        for (int y = 0, ry = original.surface->h - 1; y < original.surface->h; y++, ry-- ) {
            //Get pixel
            Uint32 pixel = original.get_pixel(x, y);

            //Copy pixel
            if ((flip_mode == flip_type_both)) {
                res.put_pixel(rx, ry, pixel);
            } else if (flip_mode == flip_type_horizontal) {
                res.put_pixel(rx, y, pixel );
            } else if(flip_mode == flip_type_vertical) {
                res.put_pixel(x, ry, pixel );
            } else {
                std::cout << "UNKNOWN flip mode [" << flip_mode << "]" << std::endl;
                char enum_str[20];
                sprintf(enum_str, "%d", flip_mode);
                exception_manager::throw_param_exception(std::string("graphicsLib::flip_image, invalid mode"), std::string(enum_str));
            }
        }
    }
}

void ImageView::blink_screen(int r, int g, int b)
{
    // @TODO::IURI //
}

