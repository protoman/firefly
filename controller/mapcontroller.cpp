#include "view/animation.h"
#include "mapcontroller.h"
#include "GameManager.h"
#include "objects/GameObject.h"
#include "collision_detection.h"

MapController::MapController()
{

}

void MapController::loadMap()
{
    // TODO::IURO - reset objects, npcs, etc //

    imageLayerMap.clear();
    layerScrollMap.clear();
    object_list.clear();
    SharedData::get_instance()->current_area_link_list.clear();
    map_was_reloaded = true;
    reset_scrolled();

    unsigned int mapNumber = SharedData::get_instance()->v6_selected_area;
    if (SharedData::get_instance()->v6_area_list.size() <= mapNumber) {
        std::cout << "ERROR::map::loadMap - Invalid map number[" << mapNumber << "] for list.size[" << SharedData::get_instance()->v6_area_list.size() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }


    // determine rooms of the area //
    SharedData::get_instance()->leftmost_room = FILE_AREA_W;
    SharedData::get_instance()->rightmost_room = 0;
    SharedData::get_instance()->topmost_room = FILE_AREA_H;
    SharedData::get_instance()->bottommost_room = 0;
    SharedData::get_instance()->area_room_list.clear();
    for (int i=0; i<FILE_AREA_W; i++) {
        for (int j=0; j<FILE_AREA_H; j++) {
            //std::cout << "room[" << i << "][" << j << "].area_n[" << SharedData::get_instance()->v6_current_level_data.rooms[i][j].area_n << "]" << std::endl;
            if (SharedData::get_instance()->v6_current_level_data.rooms[i][j].area_n == SharedData::get_instance()->v6_selected_area) {
                //std::cout << "@@@@@@@ MapController::loadMap - room[" << i << "][" << j << "].area_n[" << SharedData::get_instance()->v6_current_level_data.rooms[i][j].area_n << "]" << std::endl;
                SharedData::get_instance()->area_room_list.push_back(st_position(i, j));
                if (i < SharedData::get_instance()->leftmost_room) {
                    SharedData::get_instance()->leftmost_room = i;
                }
                if (i > SharedData::get_instance()->rightmost_room) {
                    SharedData::get_instance()->rightmost_room = i;
                }
                if (j > SharedData::get_instance()->bottommost_room) {
                    SharedData::get_instance()->bottommost_room = j;
                }
                if (j < SharedData::get_instance()->topmost_room) {
                    SharedData::get_instance()->topmost_room = j;
                }
            }
        }
    }

    map_tiles_w = (SharedData::get_instance()->rightmost_room-SharedData::get_instance()->leftmost_room+1)*AREA_ROOM_W;
    map_tiles_h = (SharedData::get_instance()->bottommost_room-SharedData::get_instance()->topmost_room+1)*AREA_ROOM_H;

    //std::cout << "MapController::loadMap - v6_selected_area[" << SharedData::get_instance()->v6_selected_area << "], rightmost_room[" << SharedData::get_instance()->rightmost_room << "], leftmost_room[" << SharedData::get_instance()->leftmost_room << "], bottommost_room[" << SharedData::get_instance()->bottommost_room << "], topmost_room[" << SharedData::get_instance()->topmost_room << "], map_tiles_w[" << map_tiles_w << "], map_tiles_h[" << map_tiles_h << "]" << std::endl;

    area_tile_map.clear();
    level3_tiles.clear();
    level3_water_tiles.clear();
    int i_count = 0;
    for (int i=SharedData::get_instance()->leftmost_room; i<=SharedData::get_instance()->rightmost_room; i++) {
        int j_count = 0;
        for (int j=SharedData::get_instance()->topmost_room; j<=SharedData::get_instance()->bottommost_room; j++) {
            if (SharedData::get_instance()->v6_current_level_data.rooms[i][j].area_n == SharedData::get_instance()->v6_selected_area) {
                for (int m=0; m<AREA_ROOM_W; m++) {
                    for (int n=0; n<AREA_ROOM_H; n++) {
                        int tile_x = m + i_count*AREA_ROOM_W;
                        int tile_y = n + j_count*AREA_ROOM_H;
                        int tile_type = SharedData::get_instance()->v6_current_level_data.rooms[i][j].tiles[m][n].locked;
                        area_tile_map.insert(std::pair<st_position, file_v6_room_tile>(st_position(tile_x, tile_y), SharedData::get_instance()->v6_current_level_data.rooms[i][j].tiles[m][n]));
                        //if (tile_type != 0) { std::cout << "tile[" << i << "][" << j << "].type[" << tile_type << "]" << std::endl; }
                        if (tile_type == TERRAIN_WATER) {
                            level3_water_tiles.push_back(st_position(tile_x, tile_y));
                        }

                        int overlay_x = SharedData::get_instance()->v6_current_level_data.rooms[i][j].tiles[m][n].tile_overlay.x;
                        int overlay_y = SharedData::get_instance()->v6_current_level_data.rooms[i][j].tiles[m][n].tile_overlay.y;
                        if (overlay_x != -1 && overlay_y != -1) {
                            struct st_level3_tile temp_tile(st_position(overlay_x, overlay_y), st_position(tile_x, tile_y));
                            level3_tiles.push_back(temp_tile);
                        }
                    }
                }
            }
            j_count++;
        }
        i_count++;
    }

    // load area-links for this area //
    //std::map<unsigned int, std::vector<struct_file_v5_area_link>> file_v5_area_link_map;
    //SharedData::get_instance()->current_area_link_list
    for (auto const& item : SharedData::get_instance()->file_v5_area_link_map) {
        for (int i=0; i<item.second.size(); i++) {
            int p_area_n1 = get_level_from_room(item.second.at(i).p1.x, item.second.at(i).p1.y);
            int p_area_n2 = get_level_from_room(item.second.at(i).p2.x, item.second.at(i).p2.y);
            if (p_area_n1 == mapNumber || p_area_n2 == mapNumber) {
                SharedData::get_instance()->current_area_link_list.push_back(item.second.at(i));
            }
        }
    }



    load_map_npcs();
    load_map_objects();

    _show_map_pos_x = -1;
    _show_map_pos_y = -1;
    create_dynamic_background_surfaces();
    init_animated_tiles();
    map_screen = ImageView::get_instance()->initSurface(st_size(RES_W+TILESIZE*2, AREA_H+TILESIZE*2));

    preload_slope_images();

    draw_map_tiles();
    GameManager::get_instance()->start_stage_music();
}



void MapController::reset_map()
{
    // reset objects
    /*
    for (std::vector<object>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        object& temp_obj = (*it);
        // if object is a player item, remove it
        if (temp_obj.get_id() == game_data.player_items[0] || temp_obj.get_id() == game_data.player_items[1]) {
            temp_obj.set_finished(true);
        } else {
            temp_obj.reset();
        }
    }
    */
}

void MapController::set_scroll_to_bottom()
{
    //scroll.y = gameManager::get_instance()->get_current_map_obj()->get_size().height*TILESIZE-AREA_H;
    int lockY = get_first_bottom_lock(0);
    int scrollY = lockY*TILESIZE-AREA_H;
    if (scrollY < 0) {
        scrollY = 0;
    }
    scroll.y = scrollY;
    //std::cout << "### MapController::set_scroll_to_bottom, y[" << scroll.y << "], lockY[" << lockY << "]" << std::endl;

}


void MapController::show()
{
    drawLayers(false);
    if (get_map_gfx_mode() == SCREEN_GFX_MODE_BACKGROUND) {
        draw::get_instance()->show_gfx();
    }

    //std::cout << "_show_map_pos_y[" << _show_map_pos_y << "], scroll.y[" << scroll.y << "]" << std::endl;

    // redraw screen, if needed
    if (_show_map_pos_x == -1 || abs(_show_map_pos_x - scroll.x) > TILESIZE) {
        draw_map_tiles();
    // use memory screen
    } else if (_show_map_pos_y == -1 || abs(_show_map_pos_y - scroll.y) > TILESIZE) {
        draw_map_tiles();
    }
    int diff_scroll_x = scroll.x - _show_map_pos_x;
    int diff_scroll_y = scroll.y - _show_map_pos_y;

    //std::cout << "MapController::show, diff_scroll_y.y[" << diff_scroll_y << "]" << std::endl;
    //ImageView::get_instance()->renderTexturePortionAt(diff_scroll_x+TILESIZE, diff_scroll_y+TILESIZE, RES_W, AREA_H, 0, 0, map_screen.texture);
    ImageView::get_instance()->renderTexturePortionAt(diff_scroll_x+TILESIZE, diff_scroll_y+TILESIZE, RES_W, AREA_H, 0, 0, map_screen.texture);
    // draw animated tiles
    draw_animated_tiles();

    if (get_map_gfx_mode() == SCREEN_GFX_MODE_FULLMAP) {
        draw::get_instance()->show_gfx();
    }
    updated_visited_room();
}

void MapController::updated_visited_room()
{
    SharedData::get_instance()->current_room_pos.x = SharedData::get_instance()->leftmost_room + (scroll.x+RES_W/2)/(AREA_ROOM_W*TILESIZE);
    SharedData::get_instance()->current_room_pos.y = SharedData::get_instance()->topmost_room + (scroll.y+AREA_H/2)/(AREA_ROOM_H*TILESIZE);

    //std::cout << "MapController::updated_visited_room - room_x[" << SharedData::get_instance()->current_room_pos.y << "], room_y[" << SharedData::get_instance()->current_room_pos.y << "]" << std::endl;

    if (SharedData::get_instance()->visited_level_list.at(SharedData::get_instance()->v6_selected_level).visited[SharedData::get_instance()->current_room_pos.x][SharedData::get_instance()->current_room_pos.y] == false) {
        SharedData::get_instance()->visited_level_list.at(SharedData::get_instance()->v6_selected_level).visited[SharedData::get_instance()->current_room_pos.x][SharedData::get_instance()->current_room_pos.y] = true;
    }
}

int MapController::get_level_from_room(int x, int y)
{
    return SharedData::get_instance()->v6_current_level_data.rooms[x][y].area_n;
}

void MapController::addLayer(unsigned int n, bool isFg)
{
    file_v6_area_layer layer = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[n];
    std::string filename = layer.filename;
    // only add if not existing in map
    if (imageLayerMap.find(n) == imageLayerMap.end()) {
        if (filename.length() < 5 || filename.find(".png") == std::string::npos) {
            return;
        }
        //std::cout << ">>>>>>>>>>>>>>>>>>>>>>> MapController::addLayer[" << n << "][" << filename << "]" << std::endl;
        imageLayerMap.insert(std::pair<unsigned int, st_background>(n, st_background()));
        imageLayerMap.at(n).imageData = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH+std::string("/images/map_backgrounds/")+filename);


        if (layer.alpha != 255) {
            ImageView::get_instance()->set_surface_alpha(layer.alpha, imageLayerMap.at(n).imageData);
            SDL_SetTextureBlendMode(imageLayerMap.at(n).imageData.texture, SDL_BLENDMODE_BLEND);
        } else {
            SDL_SetTextureBlendMode(imageLayerMap.at(n).imageData.texture,
            SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDOPERATION_ADD));
        }

        imageLayerMap.at(n).position.y = layer.adjust_y;

        layerScrollMap.insert(std::pair<unsigned int, st_layer_pos>(n, st_layer_pos(isFg)));
        if (n == 3) {
            ImageView::get_instance()->set_surface_alpha(150, imageLayerMap.at(n).imageData);
        }
    }
}


void MapController::clearLayers()
{
    imageLayerMap.clear();
}



void MapController::get_map_area_surface(st_imageData& mapSurface)
{
    mapSurface = ImageView::get_instance()->initSurface(st_size(RES_W, RES_H));

    if (!mapSurface.surface) {
        exception_manager::throw_general_exception(std::string("MapController::get_map_area_surface"), "Could not init map surface");
    }

    st_color& map_color = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).background_color;

    ImageView::get_instance()->clear_surface_area(0, 0, RES_W, RES_H, map_color.r, map_color.g, map_color.b, mapSurface);

    draw_dynamic_backgrounds_into_surface(mapSurface);

    // redraw screen, if needed
    if (_show_map_pos_x == -1 || abs(_show_map_pos_x - scroll.x) > TILESIZE) {
        draw_map_tiles();
    } else if (_show_map_pos_y == -1 || abs(_show_map_pos_y - scroll.y) > TILESIZE) {
        draw_map_tiles();
    }
    // use memory screen
    int diff_scroll_x = scroll.x - _show_map_pos_x;
    int diff_scroll_y = scroll.y - _show_map_pos_y;
    ImageView::get_instance()->copyArea(st_rectangle(diff_scroll_x+TILESIZE, diff_scroll_y+TILESIZE, RES_W, RES_H), st_position(0, 0), map_screen, mapSurface);

    // draw animated tiles
    // @TODO-Iuri //
    //draw_animated_tiles(mapSurface);
}

void MapController::draw_map_tiles()
{
    _show_map_pos_x = scroll.x;
    _show_map_pos_y = scroll.y;

    int tile_x_ini = scroll.x/TILESIZE-1;
    if (tile_x_ini < 0) {
        tile_x_ini = 0;
    }

    int tile_y_ini = scroll.y/TILESIZE-1;
    if (tile_y_ini < 0) {
        tile_y_ini = 0;
    }


    // TODO::IURI //
    ImageView::get_instance()->clear_surface(map_screen);

    // draw the tiles of the screen region
    struct st_position pos_origin;
    struct st_position pos_destiny;
    int n = -1;


    int tile_end_x = tile_x_ini+(RES_W/TILESIZE)+3;
    int tile_end_y = tile_y_ini+(AREA_H/TILESIZE)+2;
    if (tile_end_x > map_tiles_w) {
        tile_end_x = map_tiles_w;
    }
    if (tile_end_y > map_tiles_h) {
        tile_end_y = map_tiles_h;
    }
    //std::cout << "MapController::draw_map_tiles - tile_y_ini[" << tile_y_ini << "], tile_end_y[" << tile_end_y << "]" << std::endl;
    //std::cout << "MapController::draw_map_tiles - RES_W/TILESIZE[" << RES_W/TILESIZE << ", start[" << tile_x_ini << "], end[" << tile_end << "]" << std::endl;

    for (int i=tile_x_ini; i<tile_end_x; i++) {
        int diff_x = scroll.x - (tile_x_ini+1)*TILESIZE;
        pos_destiny.x = n*TILESIZE - diff_x + TILESIZE;
        for (int j=tile_y_ini; j<tile_end_y; j++) {

            // don't draw easy-mode blocks if game difficulty not set to easy
            int diff_y = scroll.y - (tile_y_ini+1)*TILESIZE;
            pos_destiny.y = j*TILESIZE - scroll.y + TILESIZE;
            //std::cout << "pos_destiny.y[" << pos_destiny.y << "]" << std::endl;


            if (getTileFromPosition(i, j).tile_underlay.type == TILE_TYPE_SLOPE) {
                //std::cout << "FOUND-SLOPE #1 i[" << i << "], j[" << j << "]" << std::endl;
                draw_slope_tile(getTileFromPosition(i, j).tile_underlay.x,getTileFromPosition(i, j).tile_underlay.y, pos_destiny.x, pos_destiny.y);
            } else {
                pos_origin.x = getTileFromPosition(i, j).tile_underlay.x;
                pos_origin.y = getTileFromPosition(i, j).tile_underlay.y;

                if (pos_origin.x >= 0 && pos_origin.y >= 0) {
                    if (map_screen.surface == nullptr) {
                        std::cout << "map_screen is NULL" << std::endl;
                    }
                    /*
                    if (i == 5) {
                        std::cout << ">>>>> MapController::draw_map_tiles #2 diff_y[" << diff_y << "], scroll.y[" << scroll.y << "], tile_y_ini[" << tile_y_ini << "], x[" << i << "], y[" << j << "], tile.x[" << getTileFromPosition(i, j).tile_underlay.x << "], tile.y[" << getTileFromPosition(i, j).tile_underlay.x << "], dest.y[" << pos_destiny.y << "] <<<<<<<<" << std::endl;
                    }
                    */
                    ImageView::get_instance()->placeTile(pos_origin, pos_destiny, map_screen);
                }
            }
        }
        n++;
    }
    // re-generate texture
    ImageView::get_instance()->rebuildTexture(map_screen);
}



void MapController::draw_animated_tiles()
{

    //std::cout << "MapController::draw_animated_tiles anim_tile_list.size[" << anim_tile_list.size() << "]" << std::endl;

    //scroll.x - dest.x
    for (int i=0; i<anim_tile_list.size(); i++) {
        //std::cout << "draw-anim-tile[" << i << "][" << anim_tile_list.at(i).anim_tile_id << "], x[" << anim_tile_list.at(i).dest_x << "], y[" << anim_tile_list.at(i).dest_y << "]" << std::endl;

        int pos_x = anim_tile_list.at(i).dest_x-scroll.x;
        if (pos_x >= -TILESIZE && pos_x <= RES_W+1) {
            //std::cout << "## scroll.x[" << scroll.x << "], dest.x[" << anim_tile_list.at(i).dest_x << "]" << std::endl;
            st_position dest_pos(pos_x, anim_tile_list.at(i).dest_y);
            ImageView::get_instance()->place_anim_tile(anim_tile_list.at(i).anim_tile_id, dest_pos);
        }
    }

    ImageView::get_instance()->update_anim_tiles_timers();
}

void MapController::init_animated_tiles()
{
    // draw the tiles of the screen region
    struct st_position pos_origin;
    struct st_position pos_destiny;
    // @TODO v6 //

    for (int map_x=SharedData::get_instance()->leftmost_room; map_x<=SharedData::get_instance()->rightmost_room; map_x++) {
        //std::cout << "@#### MapController::init_animated_tiles - map_x[" << map_x << "]" << std::endl;
        for (int map_y=SharedData::get_instance()->topmost_room; map_y<=SharedData::get_instance()->bottommost_room; map_y++) {
            //std::cout << "@#### MapController::init_animated_tiles - map_y[" << map_y << "]" << std::endl;
            for (int i=0; i<AREA_ROOM_W; i++) {
                for (int j=0; j<AREA_ROOM_H; j++) {
                    file_v6_tile_piece underlay_tile = SharedData::get_instance()->v6_current_level_data.rooms[map_x][map_y].tiles[i][j].tile_underlay;
                    // @TODO: overlay //
                    //file_v6_tile_piece overlay_tile = SharedData::get_instance()->v6_current_level_data.rooms[map_x][map_y].tiles[i][j].tile_overlay;
                    if (underlay_tile.type == TILE_TYPE_ANIM &&  SharedData::get_instance()->v6_current_level_data.rooms[map_x][map_y].area_n == SharedData::get_instance()->v6_selected_area) {


                        pos_origin.x = underlay_tile.x;
                        pos_origin.y = underlay_tile.y;

                        //std::cout << "FOUND-ANIM-TYPE - underlay_tile[" << underlay_tile.x << "][" << underlay_tile.y << "]" << std::endl;


                        int anim_tile_id = pos_origin.x;
                        pos_destiny.y = j*TILESIZE + map_y*AREA_ROOM_H;
                        pos_destiny.x = i*TILESIZE + map_x*AREA_ROOM_W;
                        //std::cout << "MAP::showMap::place_anim_tile - pos_destiny[" << pos_destiny.x << "][" << pos_destiny.x << "]" << std::endl;
                        anim_tile_list.push_back(anim_tile_desc(anim_tile_id, pos_destiny));


                    }

                }
            }
        }
    }


    /*
    for (int i=0; i<map_tiles_w; i++) {
        pos_destiny.x = i*TILESIZE;
        for (int j=0; j<map_tiles_h; j++) {



            int n = j*map_tiles_w + i;
            pos_origin.x = SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->v6_selected_area).at(n).tile_underlay.x;
            pos_origin.y = SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->v6_selected_area).at(n).tile_underlay.y;

            if (pos_origin.x < -1 && pos_origin.y == 0) {
                int anim_tile_id = (pos_origin.x * -1) - 2;
                pos_destiny.y = j*TILESIZE;
                //std::cout << "MAP::showMap::place_anim_tile[" << i << "][" << j << "]" << std::endl;
                anim_tile_list.push_back(anim_tile_desc(anim_tile_id, pos_destiny));
            }
        }
    }
    */
}


// ********************************************************************************************** //
// show the third level of tiles                                                                  //
// ********************************************************************************************** //
void MapController::showAbove(int scroll_y, int temp_scroll_x, bool show_fg)
{
    int scroll_x = scroll.x;
    if (temp_scroll_x != -99999) {
        scroll_x = temp_scroll_x;
    }
    // only show pieces that in current screen position
    short start_point_x = scroll_x/TILESIZE;
    if (start_point_x > 0) { start_point_x--; }
    short end_point_x = (scroll_x+RES_W)/TILESIZE;
    if (end_point_x < map_tiles_w-1) { end_point_x++; }
    //std::cout << "showAbove - start_point: " << start_point << ", end_point: " << end_point << std::endl;

    short start_point_y = scroll.y/TILESIZE;
    if (start_point_y > 0) { start_point_y--; }
    short end_point_y = (scroll.y+AREA_H)/TILESIZE;
    if (end_point_y < map_tiles_h-1) { end_point_y++; }


    // draw 3rd tile level
    std::vector<st_level3_tile>::iterator tile3_it;
    for (tile3_it = level3_tiles.begin(); tile3_it != level3_tiles.end(); tile3_it++) {

        if (_3rd_level_ignore_area.x != -1 && _3rd_level_ignore_area.w > 0 && ((*tile3_it).map_position.x >= _3rd_level_ignore_area.x && (*tile3_it).map_position.x < _3rd_level_ignore_area.x+_3rd_level_ignore_area.w && (*tile3_it).map_position.y >= _3rd_level_ignore_area.y && (*tile3_it).map_position.y < _3rd_level_ignore_area.y+_3rd_level_ignore_area.h)) {
            continue;
        }
        int pos_x = (*tile3_it).tileset_pos.x;
        int pos_y = (*tile3_it).tileset_pos.y;
        // only show tile if it is on the screen range

        ImageView::get_instance()->place_3rd_level_tile(pos_x, pos_y, ((*tile3_it).map_position.x*TILESIZE)-scroll_x, ((*tile3_it).map_position.y*TILESIZE)-scroll.y);
        //ImageView::get_instance()->clearScreenArea(pos_x, pos_y, TILESIZE, TILESIZE, 100, 0, 0);
    }

    // draw a blue hue over water tiles
    //std::cout << "$$$$$$$$$$$$$$$$$ level3_water_tiles.size[" << level3_water_tiles.size() << "]" << std::endl;

    std::vector<st_position>::iterator tile3_water_it;
    for (tile3_water_it = level3_water_tiles.begin(); tile3_water_it != level3_water_tiles.end(); tile3_water_it++) {
        int pos_x = (*tile3_water_it).x;
        int pos_y = (*tile3_water_it).y;

        /*
        if (_3rd_level_ignore_area.x != -1 && _3rd_level_ignore_area.w > 0 && ((*tile3_water_it).x >= _3rd_level_ignore_area.x && (*tile3_water_it).x < _3rd_level_ignore_area.x+_3rd_level_ignore_area.w && (*tile3_water_it).y >= _3rd_level_ignore_area.y && (*tile3_water_it).y < _3rd_level_ignore_area.y+_3rd_level_ignore_area.h)) {
            continue;
        }
        */

        if ((pos_x >= start_point_x && pos_x <= end_point_x) && (pos_y >= start_point_y && pos_y <= end_point_y)) {
            // only show tile if it is on the screen range
            int dest_x = pos_x*TILESIZE-scroll_x;
            int dest_y = pos_y*TILESIZE-scroll.y;
            //std::cout << "$$$$$$$$$$$$$$$$$$$$$ tile.water[" << pos_x << "][" << pos_y << "], scroll_x[" << scroll_x << "], scroll_y[" << scroll_y << "], dest[" << dest_x << "][" << dest_y << "]" << std::endl;

            //ImageView::get_instance()->clearScreenArea(dest_x, dest_y, TILESIZE, TILESIZE, 100, 0, 0);
            draw::get_instance()->draw_water_tile_overlay(dest_x, dest_y);
        }
    }




    if (_water_bubble.pos.x != -1) {
        draw::get_instance()->show_bubble(_water_bubble.pos.x+_water_bubble.x_adjust, _water_bubble.pos.y);
        int water_lock = getMapPointLock(st_position((_water_bubble.pos.x+2+scroll_x)/TILESIZE, _water_bubble.pos.y/TILESIZE));
        _water_bubble.pos.y -= 2;
        if (_water_bubble.x_adjust_direction == ANIM_DIRECTION_LEFT) {
            _water_bubble.x_adjust -= 0.5;
            if (_water_bubble.x_adjust < -4) {
                _water_bubble.x_adjust_direction = ANIM_DIRECTION_RIGHT;
            }
        } else {
            _water_bubble.x_adjust += 0.5;
            if (_water_bubble.x_adjust >= 0) {
                _water_bubble.x_adjust_direction = ANIM_DIRECTION_LEFT;
            }
        }
        if (water_lock != TERRAIN_WATER || _water_bubble.timer < TimerView::get_instance()->getTimer()) {
            //std::cout << ">> MAP::showAbove::HIDE_BUBBLE <<" <<std::endl;
            _water_bubble.pos.x = -1;
            _water_bubble.pos.y = -1;
        }
    }

    // animations
    /// @TODO: remove "finished" animations
    std::vector<animation>::iterator animation_it;
    for (animation_it = animation_list.begin(); animation_it != animation_list.end(); animation_it++) {
        if ((*animation_it).finished() == true) {
            animation_list.erase(animation_it);
            break;
        } else {
            (*animation_it).execute(); // TODO: must pass scroll map to npcs somwhow...
        }
    }

    drawLayers(true);
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
int MapController::getMapPointLock(st_position pos) const
{
    //std::cout << ">>>>> MapController::getMapPointLock x[" << pos.x << "], y[" << pos.y << "]" << std::endl;
    if (pos.x < 0 || pos.y < 0 || pos.y >= map_tiles_h || pos.x >= map_tiles_w) {
        //std::cout << ">>>>> MapController::getMapPointLock OUT OF MAP" << std::endl;
        return TERRAIN_UNBLOCKED;
    }
    if (area_tile_map.find(pos) == area_tile_map.end()) {
        return -2;
    }

    return area_tile_map.at(pos).locked;
}

file_v6_tile_piece MapController::get_map_point_tile1(st_position pos)
{
    file_v6_tile_piece res;
    res.x = -1;
    res.y = -1;
    if (pos.x < 0 || pos.y < 0 || pos.y > map_tiles_h || pos.x >= map_tiles_w) {
        return res;
    }
    if (area_tile_map.find(pos) == area_tile_map.end()) {
        return res;
    }
    return area_tile_map.at(pos).tile_underlay;
}

short MapController::get_map_point_lock(int tile_x, int tile_y)
{
    if (tile_x < 0 || tile_y < 0 || tile_y > map_tiles_h || tile_x >= map_tiles_w) {
        //std::cout << "INVALID TILE #1 x[" << tile_x << "], tile_y[" << tile_y << "]" << std::endl;
        return -2;
    }

    if (area_tile_map.find(st_position(tile_x, tile_y)) == area_tile_map.end()) {
        return -2;
    }

    return area_tile_map.at(st_position(tile_x, tile_y)).locked;
}


bool MapController::is_point_solid(st_position pos) const
{
    short int lock_p = getMapPointLock(pos);

    if (lock_p == TERRAIN_UNBLOCKED || lock_p != TERRAIN_WATER || lock_p == TERRAIN_VSCROLL_LOCK || lock_p == TERRAIN_HSCROLL_LOCK || lock_p == TERRAIN_SLOPE) {
        return false;
    }
    return true;
}

file_v6_room_tile MapController::getTileFromPosition(int x, int y)
{
    file_v6_room_tile locked;
    locked.locked = -2;
    if (x < 0 || y < 0 || y >= map_tiles_h || x >= map_tiles_w) {
        return locked;
    }
    if (area_tile_map.find(st_position(x, y)) == area_tile_map.end()) {
        return locked;
    }
    return area_tile_map.at(st_position(x, y));
}

int MapController::get_first_bottom_lock(int initialY)
{
    bool isLockedY = true;
    int initX = scroll.x/TILESIZE;
    int endX = initX + RES_W/TILESIZE;
    int lockedRow = map_tiles_h/TILESIZE;


    int initY = (initialY)/TILESIZE;
    int endY = initY + map_tiles_h;

    if (endX >= map_tiles_w) {
        endX = map_tiles_w-1;
    }
    if (endY >= map_tiles_h) {
        endY = map_tiles_h-1;
    }

    //std::cout << "########## MapController::get_first_bottom_lock - initX[" << initX << "], endX[" << endX << "], initY[" << initY << "], endY[" << endY << "]" << std::endl;
    for (int y=SharedData::get_instance()->topmost_room*AREA_ROOM_H; y<=SharedData::get_instance()->bottommost_room*AREA_ROOM_H; y++) {
        isLockedY = true;
        for (int x=SharedData::get_instance()->leftmost_room*AREA_ROOM_W; x<SharedData::get_instance()->rightmost_room*AREA_ROOM_W; x++) {
            file_v6_room_tile tile = getTileFromPosition(x, y);
            //std::cout << "x[" << x << "], y[" << y << "], locked[" << tile.locked << "]" << std::endl;
            if (tile.locked == TERRAIN_UNBLOCKED || tile.locked == TERRAIN_WATER) {
                isLockedY = false;
                break;
            }
        }
        if (isLockedY) {
            //std::cout << "LOCKED-ROW[" << y << "]" << std::endl;
            lockedRow = y;
            break;
        }
    }
    //std::cout << ":::::::::: get_first_bottom_lock, scroll.y[" << scroll.y << "], lockedRow[" << lockedRow << "]" << std::endl;
    return lockedRow;

}

bool MapController::isEdgeRowLocked(int incY, bool first)
{
    int y = (scroll.y+AREA_H-TILESIZE)/TILESIZE;
    if (first == true) {
        y = (scroll.y-TILESIZE+TILESIZE)/TILESIZE;
    }

    if (y < 0) {
        //std::cout << "MapController::isEdgeRowLocked - OUT OF SCREEN" << std::endl;
        return true;
    }
    int initX = scroll.x/TILESIZE;
    int endX = initX + RES_W/TILESIZE;

    //std::cout << "########### MapController::isEdgeRowLocked - calc_endX[" << endX << "], map_tiles_w[" << map_tiles_w << "]" << std::endl;
    if (endX >= map_tiles_w) {
        endX = map_tiles_w-1;
    }
    bool isLockedY = true;
    for (int x=initX; x<=endX; x++) {
        file_v6_room_tile tile = getTileFromPosition(x, y);
        //std::cout << "MapController::isEdgeRowLocked - x[" << x << "], y[" << y << "], locked[" << tile.locked << "]" << std::endl;
        if (tile.locked == TERRAIN_UNBLOCKED || tile.locked == TERRAIN_WATER) {
            isLockedY = false;
            break;
        }
    }


    //std::cout << "########### MapController::isEdgeRowLocked - first[" << first << "], incY[" << incY << "], scroll.y[" << scroll.y << "], y[" << y << "]" << std::endl;
    return isLockedY;
}

bool MapController::isEdgeColumnLocked(int incX, bool first)
{
    //std::cout << "## MapController::isEdgeColumnLocked::START - incX[" << incX << "], first[" << first << "]" << std::endl;


    int tileX = (scroll.x + incX + RES_W)/TILESIZE - 1;
    if (incX < 0) {
        tileX = (scroll.x + incX)/TILESIZE + 1;
    }

    if (tileX < 0 || tileX > SharedData::get_instance()->rightmost_room*AREA_ROOM_W) {
        //std::cout << "## MapController::isEdgeColumnLocked - TRUE #1" << std::endl;
        return true;
    }
    int inity = scroll.y/TILESIZE;
    int endY = inity + AREA_H/TILESIZE;
    if (endY >= map_tiles_w) {
        endY = map_tiles_w-1;
    }
    bool isLockedX = true;

    //std::cout << "## MapController::isEdgeColumnLocked - scroll.x[" << scroll.x << "], tileX[" << tileX << "], scroll.y[" << scroll.y << "], inity[" << inity << "], endY[" << endY << "]" << std::endl;

    for (int y=inity; y<endY; y++) {
        file_v6_room_tile tile = getTileFromPosition(tileX, y);
        //std::cout << "## MapController::isEdgeColumnLocked - incX[" << incX << "], x[" << tileX << "], y[" << y << "], tile.locked[" << tile.locked << "]" << std::endl;
        if (tile.locked == TERRAIN_UNBLOCKED || tile.locked == TERRAIN_WATER) {
            //std::cout << "## MapController::isEdgeColumnLocked - ONLOCKED - x[" << tileX << "], y[" << y << "], locked[" << tile.locked << "]" << std::endl;
            isLockedX = false;
            break;
        }
    }


    //std::cout << "########### MapController::isEdgeColumnLocked - isLockedX[" << isLockedX << "], first[" << first << "], incY[" << incX << "], scroll.x[" << scroll.x << "], x[" << tileX << "]" << std::endl;
    return isLockedX;
}



// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void MapController::changeScrolling(st_float_position pos, bool check_lock)
{
    if (abs(pos.x) > TILESIZE) {
        if (pos.x > 0) {
            pos.x = 1;
        } else {
            pos.x = -1;
        }
    }
    // moving player to right, screen to left
    if (pos.x > 0 && ((scroll.x/TILESIZE+RES_W/TILESIZE)-1 < map_tiles_w-1)) {
        int x_change = pos.x;
        if (pos.x >= TILESIZE) { // if change is too big, do not update (TODO: must check all wall until lock)
            x_change = 1;
        }
        int tile_x = (scroll.x+RES_W-TILESIZE+2)/TILESIZE;
        // this means there is a scroll-lock, so we ignore the excess
        if (check_lock == false || isEdgeColumnLocked(pos.x, tile_x) == false) {
            //std::cout << "MapController::changeScrolling #1, pos.x[" << pos.x << "], tile_x[" << tile_x << "]" << std::endl;
            incScrollValue(x_change, 0);
            changeLayerScroll(x_change, 0);
            adjust_dynamic_backgrounds_position();
        }
    } else if (pos.x < 0) {
        int x_change = pos.x;
        if (scroll.x/TILESIZE >= 0) { // if change is too big, do not update (TODO: must check all wall until lock)
            int tile_x = (scroll.x+TILESIZE-2)/TILESIZE;
            //std::cout << "#2 LEFT changeScrolling - scroll.x: " << scroll.x << ", testing tile_x: " << tile_x << std::endl;
            if (check_lock == false || isEdgeColumnLocked(pos.x, tile_x) == false) {
                //std::cout << "MapController::changeScrolling - 2" << std::endl;
                incScrollValue(x_change, 0);
                changeLayerScroll(x_change, 0);
                adjust_dynamic_backgrounds_position();
            }
        }
    }

    // TODO: adjust layers position for vertical scrolling //
    if (pos.y != 0) {
        //std::cout << "$$$$$$$$$$$$$$$ pos.y[" << pos.y << "]" << std::endl;
        if (pos.y > 0) {
            bool locked = isEdgeRowLocked(pos.y, false);
            //std::cout << "MapController::changeScrolling #1 - check_lock[" << check_lock << "], locked[" << locked << "], pos.y[" << pos.y << "]" << std::endl;
            if (check_lock == false || locked == false) {
                incScrollValue(0, pos.y);
            }
        } else {
            //std::cout << "%%%%%%%%%%%%%% CHECK-TOP" << std::endl;
            bool locked = isEdgeRowLocked(pos.y, true);
            //std::cout << "MapController::changeScrolling #2 - check_lock[" << check_lock << "], locked[" << locked << "], pos.y[" << pos.y << "]" << std::endl;
            if (check_lock == false || locked == false) {
                incScrollValue(0, pos.y);
            }
        }
    }
}

void MapController::incScrollValue(float xinc, float yinc)
{
    scroll.x += xinc;
    scroll.y += yinc;
    SharedData::get_instance()->area_scroll_x += xinc;
    SharedData::get_instance()->area_scroll_y += yinc;

    if (scroll.x < 0) {
        scroll.x = 0;
    }
    if (scroll.y < 0) {
        scroll.y = 0;
    }
    if (SharedData::get_instance()->area_scroll_x < 0) {
        SharedData::get_instance()->area_scroll_x = 0;
    }
    if (SharedData::get_instance()->area_scroll_y < 0) {
        SharedData::get_instance()->area_scroll_y = 0;
    }
}



void MapController::changeLayerScroll(int x_change, int y_change)
{
    for (std::map<unsigned int, st_background>::iterator it = imageLayerMap.begin(); it != imageLayerMap.end(); ++it) {
        unsigned int bg_n = it->first;
        float layer_speed = (float)SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[bg_n].speed/10;

        if (layerScrollMap.find(bg_n) == layerScrollMap.end()) {
            layerScrollMap.insert(std::pair<unsigned int, st_layer_pos>(bg_n, st_layer_pos(false)));
        }
        if (SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[bg_n].auto_scroll == BG_SCROLL_MODE_NONE) {
            layerScrollMap.at(bg_n).pos.x -= ((float)x_change*layer_speed);
        }
    }

}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void MapController::set_scrolling(st_float_position pos)
{
    scrolled = pos;
    scroll.x = pos.x;
    scroll.y = pos.y;
    //std::cout << "------- MapController::set_scrolling - map: " << SharedData::get_instance()->v6_selected_area << ", pos.x: " << pos.x << "-------" << std::endl;
}

void MapController::reset_scrolling()
{
    scrolled = st_position(0, 0);
    scroll.x = 0;
    scroll.y = 0;
}




// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
st_float_position MapController::getMapScrolling() const
{
    //std::cout << "getMapScrolling, x: " << scroll.x << ", y: " << scroll.y << std::endl;
    return scroll;
}

st_size MapController::get_size()
{
    return st_size(map_tiles_w, map_tiles_h);
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void MapController::load_map_npcs()
{

    // remove all elements currently in the list
    if (map_enemy_list.size() > 0) {
        map_enemy_list.back().clean_character_graphics_list();
    }
    while (!map_enemy_list.empty()) {
        map_enemy_list.pop_back();
    }


    //std::cout << ">>>>>>>>>>> MapController::load_map_npcs - file_v5_map_npc_map[" << SharedData::get_instance()->v6_selected_area << "] size[" << SharedData::get_instance()->file_v5_map_npc_map.at(SharedData::get_instance()->v6_selected_area).size() << "]" << std::endl;

    for (int i=0; i<SharedData::get_instance()->file_v5_map_npc_map.at(SharedData::get_instance()->v6_selected_area).size(); i++) {
        file_v5_map_npc& npc_ref = SharedData::get_instance()->file_v5_map_npc_map.at(SharedData::get_instance()->v6_selected_area).at(i);

        int npc_id = npc_ref.id_npc;

        //std::cout << ">>>>>>>>>>> MapController::load_map_npcs - add NPC[" << npc_id << "]" << std::endl;

        if (npc_id != -1) {
            GameEnemy new_npc = GameEnemy(SharedData::get_instance()->v6_selected_area, npc_id, i);


            if (GameMediator::get_instance()->get_enemy(npc_id)->is_boss == true) {
                new_npc.set_is_boss(true);
            // TODO - move boss flag to map //
            //} else if (stage_data.boss.id_npc == npc_ic) {
                //new_npc.set_stage_boss(true);
            // adjust NPC position to ground, if needed
            } else if (new_npc.is_able_to_fly() == false && new_npc.hit_ground() == false) {
                new_npc.initialize_position_to_ground();
            }
            new_npc.init_animation();

            std::string static_bg(GameMediator::get_instance()->get_enemy(npc_id)->bg_graphic_filename);
            if (new_npc.is_static() && static_bg.length() > 0) {
                set_map_enemy_static_background(SharedData::get_instance()->FILEPATH + std::string("images/sprites/enemies/backgrounds/") + static_bg, new_npc.get_bg_position());
            }

            map_enemy_list.push_back(new_npc); // insert new npc at the list-end
            //std::cout << "(A) ######### _npc_list.add, size[" << _npc_list.size() << "]" << std::endl;

        }


    }
}


void MapController::drawLayers(bool isFg)
{
    // only draw solid background color, if map-heigth is less than RES_H
    //std::cout << "number[" << SharedData::get_instance()->v6_selected_area << "], bg1_surface.height[" << bg1_surface.height << "], bg1.y[" << GameMediator::get_instance()->map_data[SharedData::get_instance()->v6_selected_area].backgrounds[0].adjust_y << "]" << std::endl;
    file_v6_area& map_data_ref = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area);

    if (isFg == false) {
        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, map_data_ref.background_color.r, map_data_ref.background_color.g, map_data_ref.background_color.b);
    }

    for (std::map<unsigned int, st_layer_pos>::iterator it = layerScrollMap.begin(); it != layerScrollMap.end(); ++it) {
        unsigned int bg_n = it->first;
        if (isFg == false && bg_n >= LAYERS_BG_COUNT) {
            //std::cout << "bg_n[" << bg_n << "] is FG" << std::endl;
            continue;
        } else if (isFg == true && bg_n < LAYERS_BG_COUNT) {
            //std::cout << "bg_n[" << bg_n << "] is BG" << std::endl;
            continue;
        }
        file_v6_area_layer& bg_ref = map_data_ref.layers[bg_n];

        //std::cout << ">>>>>>>>>>>>>>>>>>> bg_n[" << bg_n << "], filename[" << bg_ref.filename << "]" << std::endl;

        float bg1_speed = (float)bg_ref.speed/10;
        // dynamic background won't work in low-end graphics more
        if (bg_ref.auto_scroll == BG_SCROLL_MODE_LEFT) {
            it->second.pos.x -= bg1_speed;
            adjust_dynamic_background_position(bg_n);
        } else if (bg_ref.auto_scroll == BG_SCROLL_MODE_RIGHT) {
            it->second.pos.x += bg1_speed;
            adjust_dynamic_background_position(bg_n);
        } else if (bg_ref.auto_scroll == BG_SCROLL_MODE_UP) {
            it->second.pos.y -= bg1_speed;
            adjust_dynamic_background_position(bg_n);
        } else if (bg_ref.auto_scroll == BG_SCROLL_MODE_DOWN) {
            it->second.pos.y += bg1_speed;
            adjust_dynamic_background_position(bg_n);
        }

        st_imageData* surface_bg = get_dynamic_bg(it->first);


        //std::cout << "## bg1_speed[" << bg1_speed << "], bg_scroll.x[" << bg_scroll.x << "]" << std::endl;

        float x1 = it->second.pos.x;
        if (x1 > 0.0) { // moving to right
            x1 = (RES_W - x1) * -1;
        }

        float y1 = it->second.pos.y + bg_ref.adjust_y;


        if (surface_bg->surface != nullptr && surface_bg->surface->w > 0) {
            int repeat_y_n = 1;
            if (bg_ref.repeatY) {
                repeat_y_n = AREA_H/surface_bg->surface->h;
                if (AREA_H % surface_bg->surface->h) {
                    repeat_y_n++;
                }
            }


            for (unsigned int j=0; j<repeat_y_n; j++) {
                // draw leftmost part
                //ImageView::get_instance()->renderTexturePortionAt(0, 0, surface_bg->surface->w, surface_bg->surface->h, x1, y1+(j*surface_bg->surface->h), surface_bg->texture);
                render_layer(x1, y1+(j*surface_bg->surface->h), surface_bg);
                // draw rightmost part, if needed

                if (abs(it->second.pos.x) > RES_W) {
                    //std::cout << "### MUST DRAW SECOND BG-POS-LEFT ###" << std::endl;
                    float bg_pos_x = RES_W - (abs(x1)-RES_W);
                    //ImageView::get_instance()->renderTexturePortionAt(0, 0, surface_bg->surface->w, surface_bg->surface->h, bg_pos_x, y1+(j*surface_bg->surface->h), surface_bg->texture);
                    render_layer(bg_pos_x, y1+(j*surface_bg->surface->h), surface_bg);
                }  else if (surface_bg->surface->w - abs(it->second.pos.x) < RES_W) {
                    int repeat_x_n = 1;
                    if (bg_ref.repeatX) {
                        int repeat_x_n = RES_W/surface_bg->surface->w;
                    }
                    //std::cout << ">>>>>>>>>>>>>> repeat_x_n[" << repeat_x_n << "]" << std::endl;
                    for (unsigned int i=0; i<repeat_x_n; i++) {
                        //std::cout << "### MUST DRAW SECOND BG-POS-RIGHT ###" << std::endl;
                        float bg_pos_x = surface_bg->surface->w - (int)abs(it->second.pos.x) + i*surface_bg->surface->w;
                        render_layer(bg_pos_x, y1+(j*surface_bg->surface->h), surface_bg);
                        //ImageView::get_instance()->renderTexturePortionAt(0, 0, surface_bg->surface->w, surface_bg->surface->h, bg_pos_x, y1+(j*surface_bg->surface->h), surface_bg->texture);
                    }
                }
            }
        }
    }
}

void MapController::render_layer(float x, float y, st_imageData *surface_bg)
{
    ImageView::get_instance()->renderTexturePortionAt(0, 0, surface_bg->surface->w, surface_bg->surface->h, x, y, surface_bg->texture);
}



void MapController::adjust_dynamic_background_position(unsigned int bg_n)
{

    file_v6_area_layer& bg_ref = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[bg_n];
    st_imageData* surface_bg = get_dynamic_bg(bg_n);

    //int bg_limit = get_dynamic_bg()->width-RES_W;
    int bg_limit = imageLayerMap.at(bg_n).imageData.surface->w;

    // esq -> direita: #1 bg_limt[640], scroll.x[-640.799]

    if (layerScrollMap.at(bg_n).pos.x < -bg_limit) {
        //std::cout << "#1 bg_limt[" << bg_limit << "], scroll.x[" << bg_scroll.x << "]" << std::endl;
        //std::cout << "RESET BG-SCROLL #1" << std::endl;
        layerScrollMap.at(bg_n).pos.x = 0;
    } else if (layerScrollMap.at(bg_n).pos.x > bg_limit) {
        //std::cout << "#2 bg_limt[" << bg_limit << "], scroll.x[" << bg_scroll.x << "]" << std::endl;
        //std::cout << "RESET BG-SCROLL #2" << std::endl;
        layerScrollMap.at(bg_n).pos.x = 0;
    } else if (layerScrollMap.at(bg_n).pos.x > 0) {
        //std::cout << "#3 bg_limt[" << bg_limit << "], scroll.x[" << bg_scroll.x << "]" << std::endl;
        //std::cout << "RESET BG-SCROLL #3" << std::endl;
        layerScrollMap.at(bg_n).pos.x = -(surface_bg->surface->w); // erro aqui
    }


    if (layerScrollMap.at(bg_n).pos.y < -RES_H) {
        layerScrollMap.at(bg_n).pos.y = 0;
    } else if (layerScrollMap.at(bg_n).pos.y > RES_H) {
        layerScrollMap.at(bg_n).pos.y = 0;
    }
}

// called when scroll changes
void MapController::adjust_dynamic_backgrounds_position()
{
    for (std::map<unsigned int, st_layer_pos>::iterator it = layerScrollMap.begin(); it != layerScrollMap.end(); ++it) {
        unsigned int bg_n = it->first;
        if (SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[bg_n].auto_scroll == BG_SCROLL_MODE_NONE) {
            adjust_dynamic_background_position(bg_n);
        }
    }
}



bool MapController::must_show_static_bg()
{
    if (static_bg.is_null() == false && static_bg_pos.x >= scroll.x-1 && static_bg_pos.x < scroll.x+RES_W) {
        return true;
    }
    return false;
}

void MapController::reset_map_loaded_flag()
{
    map_was_reloaded = false;
}


void MapController::set_map_enemy_static_background(std::string filename, st_position pos)
{
    if (static_bg.is_null() == false) {
        static_bg.freeGraphic();
    }
    if (filename.length() > 0) {
        static_bg = ImageView::get_instance()->imageFromFile(filename);
    }
    static_bg_pos = pos;
}

void MapController::preload_slope_images()
{
    slope_image_map.clear();
    for (int i=0; i<SharedData::get_instance()->slope_list.size(); i++) {
        file_v5_slope_tile* slope_data = &SharedData::get_instance()->slope_list.at(i);
        std::string full_filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/slope/" + slope_data->filename;

        file_io fio;
        if (fio.file_exists(full_filename) == false) {
            continue;
        }
        slope_image_map.insert(std::pair<int, st_imageData>(i, st_imageData()));
        slope_image_map.at(i) = ImageView::get_instance()->imageFromFile(full_filename);
        //std::cout << ">>>>>>>>> ADDED SLOPE[" << i << "]" << std::endl;
    }
}

void MapController::draw_slope_tile(int x, int y, int dest_x, int dest_y)
{
    if (slope_image_map.find(x) == slope_image_map.end()) {
        std::cout << ">>> slope x[" << x << "] not found at list.size[" << slope_image_map.size() << "]" << std::endl;
        return;
    }
    //std::cout << ">> MapController::draw_slope_tile DRAW - x[" << x << "], y[" << y << "], dest_x[" << dest_x << "], dest_y[" << dest_y << "], img_w[" << slope_image_map.at(x).surface->w << "], img_h[" << slope_image_map.at(x).surface->h << "]" << std::endl;
    //ImageView::get_instance()->renderTexturePortionAt(0, 0, slope_image_map.at(x).surface->w, slope_image_map.at(x).surface->h, dest_x, dest_y, slope_image_map.at(x).texture);
    ImageView::get_instance()->placeSlope(st_rectangle(TILESIZE*y, 0, TILESIZE, TILESIZE), st_position(dest_x, dest_y), slope_image_map.at(x), map_screen);
}



void MapController::draw_dynamic_backgrounds_into_surface(st_imageData &surface)
{
    // @TODO::IURI - usar mesmo método que desenha na tela, só passar parâmetro opcional //
}

void MapController::add_object(GameObject obj)
{
    std::cout <<"MapController::add_object::START" << std::endl;
    object_list.push_back(obj);
}

st_position MapController::get_first_lock_in_direction(st_position pos, st_size max_dist, int direction)
{
    st_position res;
    st_position x_limit_pos;

    std::cout << "########### get_first_lock_in_direction pos[" << pos.x << "][" << pos.y << "]" << std::endl;

    switch (direction) {

    case ANIM_DIRECTION_LEFT:
        res.y = pos.y;
        res.x = pos.x - max_dist.width;
        for (int pos_i=pos.x; pos_i>(pos.x-max_dist.width); pos_i--) {
            int map_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(pos_i/TILESIZE, pos.y/TILESIZE));
            //std::cout << "TELEPORT::LEFT x[" << pos_i << ", map_x[" << (pos_i/TILESIZE) << "], map_lock[" << map_lock << "]" << std::endl;
            if (map_lock != TERRAIN_UNBLOCKED && map_lock != TERRAIN_WATER) {
                std::cout << "LEFT - pos_i[" << pos_i << "]" << std::endl;
                res.x = pos_i+1;
                break;
            }
        }
        break;

    case ANIM_DIRECTION_RIGHT:
        res.y = pos.y;
        res.x = pos.x + max_dist.width;
        for (int pos_i=pos.x; pos_i<(pos.x+max_dist.width); pos_i++) {
            int map_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(pos_i/TILESIZE, pos.y/TILESIZE));
            //std::cout << "TELEPORT::RIGHT #1 x[" << pos_i << ", map_x[" << (pos_i/TILESIZE) << "], map_lock[" << map_lock << "]" << std::endl;
            if (map_lock != TERRAIN_UNBLOCKED && map_lock != TERRAIN_WATER) {
                std::cout << "TELEPORT::RIGHT #2 - pos_i[" << pos_i << "]" << std::endl;
                res.x = pos_i-1;
                break;
            }
        }
        break;

    case ANIM_DIRECTION_UP:
        res.y = pos.y - max_dist.height;
        res.x = pos.x;
        for (int pos_i=pos.y; pos_i>(pos.y-max_dist.height); pos_i--) {
            int map_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(pos.x/TILESIZE, pos_i/TILESIZE));
            //std::cout << "TELEPORT::LEFT x[" << pos_i << ", map_x[" << (pos_i/TILESIZE) << "], map_lock[" << map_lock << "]" << std::endl;
            if (map_lock != TERRAIN_UNBLOCKED && map_lock != TERRAIN_WATER) {
                std::cout << "UP - pos_i[" << pos_i << "]" << std::endl;
                res.y = pos_i+1;
                break;
            }
        }
        break;


    case ANIM_DIRECTION_DOWN:
        res.y = pos.y + max_dist.height;
        res.x = pos.x;
        for (int pos_i=pos.y; pos_i<(pos.y+max_dist.height); pos_i++) {
            int map_lock = GameManager::get_instance()->get_current_map_obj()->getMapPointLock(st_position(pos.x/TILESIZE, pos_i/TILESIZE));
            //std::cout << "TELEPORT::RIGHT #1 x[" << pos_i << ", map_x[" << (pos_i/TILESIZE) << "], map_lock[" << map_lock << "]" << std::endl;
            if (map_lock != TERRAIN_UNBLOCKED && map_lock != TERRAIN_WATER) {
                std::cout << "TELEPORT::DOWN #2 - pos_i[" << pos_i << "]" << std::endl;
                res.y = pos_i-1;
                break;
            }
        }
        break;

    case ANIM_DIRECTION_UP_LEFT:
        x_limit_pos = get_first_lock_in_direction(pos, max_dist, ANIM_DIRECTION_LEFT);
        res = get_first_lock_in_direction(x_limit_pos, max_dist, ANIM_DIRECTION_UP);
        break;

    case ANIM_DIRECTION_UP_RIGHT:
        x_limit_pos = get_first_lock_in_direction(pos, max_dist, ANIM_DIRECTION_RIGHT);
        res = get_first_lock_in_direction(x_limit_pos, max_dist, ANIM_DIRECTION_UP);
        break;

    case ANIM_DIRECTION_DOWN_LEFT:
        x_limit_pos = get_first_lock_in_direction(pos, max_dist, ANIM_DIRECTION_LEFT);
        res = get_first_lock_in_direction(x_limit_pos, max_dist, ANIM_DIRECTION_DOWN);
        break;

    case ANIM_DIRECTION_DOWN_RIGHT:
        x_limit_pos = get_first_lock_in_direction(pos, max_dist, ANIM_DIRECTION_RIGHT);
        res = get_first_lock_in_direction(x_limit_pos, max_dist, ANIM_DIRECTION_DOWN);
        break;

    default:
        break;
    }

    return res;
}

int MapController::get_first_lock_on_left(int x_pos)
{
    for (int i=x_pos; i>= 0; i--) {
        if (isEdgeColumnLocked(-1, i) == true) {
            return i*TILESIZE;
        }
    }
    return -1;
}

int MapController::get_first_lock_on_right(int x_pos)
{
    int limit = (scroll.x+RES_W)/TILESIZE;
    x_pos += 1;
    std::cout << "MapController::get_first_lock_on_right - x_pos: " << x_pos << ", limit: " << limit << std::endl;
    for (int i=x_pos; i<=limit; i++) {
        if (isEdgeColumnLocked(1, i) == true) {
            std::cout << "MapController::get_first_lock_on_right - found lock at: " << i << std::endl;
            return i*TILESIZE;
        }
    }
    return -1;
}

// gets the first tile locked that have at least 3 tiles unlocked above it
int MapController::get_first_lock_on_bottom(int x_pos, int y_pos)
{
    return get_first_lock_on_bottom(x_pos, y_pos, TILESIZE, TILESIZE*3);
}

int MapController::get_first_lock_on_bottom(int x_pos, int y_pos, int w, int h)
{

    //std::cout << ">>>>>> MAP::get_first_lock_on_bottom - START - x_pos[" << x_pos << "], y_pos[" << y_pos << "]" << std::endl;

    int tilex = x_pos/TILESIZE;
    int above_tiles_to_test = h/TILESIZE;
    if (above_tiles_to_test < 2) { // at least two tiles above even for small npcs
        above_tiles_to_test = 2;
    }
    int right_tiles_to_test = w/TILESIZE;
    if (right_tiles_to_test < 1) {
        right_tiles_to_test = 1;
    }

    //int initial_y = map_tiles_h-1;
    int initial_y = (y_pos+AREA_H)/TILESIZE;
    if (y_pos >= 0) {
        initial_y = y_pos/TILESIZE;
    }

    //std::cout << ">>>>>> MAP::get_first_lock_on_bottom - initial_y[" << initial_y << "], above_tiles_to_test[" << (above_tiles_to_test+1) << "]" << std::endl;
    for (int i=initial_y; i>=above_tiles_to_test+1; i--) { // ignore here first tiles, as we need to test them next

        //std::cout << "get_first_lock_on_bottom, i[" << i << "]" << std::endl;
        int map_lock = getMapPointLock(st_position(tilex, i));
        bool found_bad_point = false;
        if (map_lock != TERRAIN_UNBLOCKED && map_lock != TERRAIN_WATER) {
            // found a stop point, now check above tiles
            for (int j=i-1; j>=i-above_tiles_to_test; j--) {
                for (int k=0; k<right_tiles_to_test; k++) {
                    int map_lock2 = getMapPointLock(st_position(tilex+k, j));

                    //std::cout << ">>>>>> MAP::get_first_lock_on_bottom - test-point[" << (tilex+k) << "][" << j << "].terrain[" << map_lock2 << "], above_tiles_to_test[" << above_tiles_to_test << "],right_tiles_to_test[" << right_tiles_to_test << "]" << std::endl;
                    if (map_lock2 != TERRAIN_UNBLOCKED && map_lock2 != TERRAIN_WATER) { // found a stop point, now check above ones
                        //std::cout << ">>>>>> MAP::get_first_lock_on_bottom::BAD-POINT - test-point[" << (tilex+k) << "][" << j << "].terrain[" << map_lock2 << "], above_tiles_to_test[" << above_tiles_to_test << "],right_tiles_to_test[" << right_tiles_to_test << "]" << std::endl;
                        found_bad_point = true;
                        break;
                    }
                }
                if (found_bad_point) {
                    break;
                }
            }
            if (found_bad_point == false) {
                //std::cout << ">>>>>> MAP::get_first_lock_on_bottom - good-point[" << (i-1) << "]" << std::endl;
                return i-1;
            }
        }
    }
    //std::cout << ">>>>>> MAP::get_first_lock_on_bottom - FAIL" << std::endl;
    return 0;
}

void MapController::drop_item(GameEnemy* npc_ref)
{
    std::cout << ">>>>>>> MapController::drop_item::START" << std::endl;
    st_float_position position = st_float_position(npc_ref->getPosition().x + npc_ref->get_size().width/2, npc_ref->getPosition().y + npc_ref->get_size().height/2);
    // dying out of screen should not drop item
    if (position.y > RES_H) {
        return;
    }
    srand(static_cast<unsigned int>(TimerView::get_instance()->getTimer()));
    //int rand_n = rand() % 100;
    int rand_n = static_cast<int> (100.0 * (rand() / (RAND_MAX + 1.0)));
    std::cout << ">>>>>>> MapController::drop_item() - rand_n: " << rand_n << std::endl;
    DROP_ITEMS_LIST obj_type;

    // sub-bosses always will drop energy big
    if (npc_ref->is_subboss()) {
        obj_type = DROP_ITEM_ENERGY_BIG;
    } else {
        // 1UP (1%), Big Energy (2%), Big Weapon (2%), Small Energy (15)%, Small Weapon (15%), Score Pearl (53%)
        // .byt 99, 97, 95, 80, 65, 12 (http://tasvideos.org/RandomGenerators.html)
        int drop_ratio[] = {99, 97, 95, 80, 65, 50};

        if (rand_n == drop_ratio[0]) {
            obj_type = DROP_ITEM_1UP;
        } else if (rand_n >= drop_ratio[1]) {
            obj_type = DROP_ITEM_ENERGY_BIG;
        } else if (rand_n >= drop_ratio[2]) {
            obj_type = DROP_ITEM_WEAPON_BIG;
        } else if (rand_n >= drop_ratio[3]) {
            obj_type = DROP_ITEM_ENERGY_SMALL;
        } else if (rand_n >= drop_ratio[4]) {
            obj_type = DROP_ITEM_WEAPON_SMALL;
        } else if (rand_n >= drop_ratio[5]) {
            obj_type = DROP_ITEM_COIN;
        } else {
            return;
        }
    }
    st_position obj_pos;
    obj_pos.y = static_cast<short>(position.y/TILESIZE);
    obj_pos.x = static_cast<short>((position.x - TILESIZE)/TILESIZE);

    short obj_type_n = GameManager::get_instance()->get_drop_item_id(obj_type);
    if (obj_type_n == -1) {
        std::cout << ">>>>>>>>> obj_type_n(" << obj_type_n << ") invalid for obj_type(" << obj_type << ")" << std::endl;
        return;
    }

    //short _id,
    //MapController *set_map,
    //st_position map_pos,
    //st_position teleporter_dest,
    //short map_dest
    short map_dest = -1;

    GameObject temp_obj = GameObject(obj_type_n, this, obj_pos, st_position(-1, -1), map_dest);
    temp_obj.set_position(st_position(static_cast<int>(position.x), static_cast<int>(position.y)));
    temp_obj.set_duration(4500);
    add_object(temp_obj);
}

void MapController::drop_game_item(int obj_id, int uuid, int x, int y)
{
    std::cout << ">>>>>>> MapController::drop_game_item::START" << std::endl;
    short map_dest = -1;
    st_position obj_pos(x, y);

    GameObject temp_obj = GameObject(obj_id, this, obj_pos, st_position(-1, -1), map_dest);
    temp_obj.set_position(st_position(static_cast<int>(obj_pos.x), static_cast<int>(obj_pos.y)));
    // @TODO: better to remove the item
    if (SharedData::get_instance()->game_object_state_map.find(uuid) != SharedData::get_instance()->game_object_state_map.end()) {
        SharedData::get_instance()->game_object_state_map.at(uuid).area_n = SharedData::get_instance()->v6_selected_area;
        SharedData::get_instance()->game_object_state_map.at(uuid).x = x;
        SharedData::get_instance()->game_object_state_map.at(uuid).y = y;
    } else {
        v6_file_game_object_state obj_state;
        obj_state.uuid = uuid;
        obj_state.obj_id = obj_id;
        obj_state.area_n = SharedData::get_instance()->v6_selected_area;
        obj_state.x = x;
        obj_state.y = y;
        std::cout << ">>>>>>> MapController::drop_game_item - obj_state.x[" << obj_state.x << "], obj_state.y[" << obj_state.y << "]" << std::endl;
        obj_state.finished = false;
        SharedData::get_instance()->game_object_state_map.insert(std::pair<int, v6_file_game_object_state>(uuid, obj_state));
    }
    add_object(temp_obj);
}

void MapController::set_bg_scroll(int scrollx)
{
    for (std::map<unsigned int, st_background>::iterator it = imageLayerMap.begin(); it != imageLayerMap.end(); ++it) {
        unsigned int bg_n = it->first;
        layerScrollMap.at(bg_n).pos.x = scrollx;
    }
}

// @TODO: must return a list //
int MapController::get_bg_scroll() const
{
    /*
    for (std::map<unsigned int, st_background>::iterator it = mapBackgroundMap.begin(); it != mapBackgroundMap.end(); ++it) {
        unsigned int bg_n = it->first;
        return layerScrollMap.at(bg_n).x;
    }
    */
    return 0;
}


void MapController::reset_map_timers()
{
    //reset_objects_timers();
    reset_objects_anim_timers();
    reset_enemies_timers();
}

void MapController::reset_enemies_timers()
{
    //std::cout << ">>>>>> MAP::reset_enemies_timers - _npc_list.size: " << _npc_list.size() << std::endl;
    std::vector<GameEnemy>::iterator enemy_it;
    for (enemy_it = map_enemy_list.begin(); enemy_it != map_enemy_list.end(); enemy_it++) {
        (*enemy_it).reset_timers(); // TODO: must pass scroll map to npcs somwhow...
    }
}


void MapController::reset_objects_timers()
{
    //std::cout << ">>>>>> MAP::reset_objects_timers - object_list.size: " << object_list.size() << std::endl;
    std::vector<GameObject>::iterator object_it;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        (*object_it).reset_timers(); // TODO: must pass scroll map to npcs somwhow...
    }
}

void MapController::reset_objects_anim_timers()
{
    std::vector<GameObject>::iterator object_it;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        (*object_it).reset_obj_anim_timer(); // TODO: must pass scroll map to npcs somwhow...
    }
}

void MapController::reset_objects()
{
    //std::cout << ">>>>>> MAP::reset_objects - object_list.size: " << object_list.size() << std::endl;
    std::vector<GameObject>::iterator object_it;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        (*object_it).reset();
    }
}

void MapController::print_objects_number()
{
    //std::cout << ">>>>>> MAP::print_objects_number - n: " << object_list.size() << std::endl;
}

void MapController::add_bubble_animation(st_position pos)
{
    if (_water_bubble.timer > TimerView::get_instance()->getTimer()) {
        //std::cout << ">> MAP::add_bubble::CANT_ADD <<" <<std::endl;
        return;
    }
    //std::cout << ">> MAP::add_bubble::ADDED <<" <<std::endl;
    _water_bubble.timer = TimerView::get_instance()->getTimer()+3000;
    _water_bubble.pos.x = pos.x;
    _water_bubble.pos.y = pos.y;
    _water_bubble.x_adjust = 0;
}

// checks if player have any special object in screen
bool MapController::have_player_object()
{
    for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        GameObject& temp_obj = (*it);
        int item_id = temp_obj.get_id();
    }
    return false;
}

bool MapController::subboss_alive_on_left(short tileX)
{
    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if ((npc_ref->is_boss() == true || npc_ref->is_subboss() == true) && npc_ref->is_dead() == false) {
            std::cout << "Opa, achou um boss/sub-boss!" << std::endl;
            int dist_door_npc = tileX*TILESIZE - npc_ref->getPosition().x;
            std::cout << "dist_door_npc[" << dist_door_npc << "], NPC-pos.x: " << npc_ref->getPosition().x << ", tileX*TILESIZE: " << tileX*TILESIZE << std::endl;
            if (npc_ref->getPosition().x >= (tileX-20)*TILESIZE && npc_ref->getPosition().x <= tileX*TILESIZE) { // 20 tiles is the size of a visible screen
                std::cout << "Opa, achou um sub-boss NA ESQUERDA!!" << std::endl;
                return true;
            }
        }
    }
    return false;
}

void MapController::finish_object_teleporter(int obj_number)
{
    for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        GameObject& temp_obj = (*it);
        std::cout << "number: " << obj_number << ", obj.id: " << temp_obj.get_obj_map_id() << std::endl;
        if (temp_obj.get_obj_map_id() == obj_number) {
            temp_obj.set_direction(ANIM_DIRECTION_RIGHT);
        }
    }
}

void MapController::activate_final_boss_teleporter()
{
    for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        GameObject& temp_obj = (*it);
        std::cout << "number: " << SharedData::get_instance()->v6_selected_area << ", obj.id: " << temp_obj.get_obj_map_id() << ", type: " << temp_obj.get_type() << ", OBJ_FINAL_BOSS_TELEPORTER: " << OBJ_FINAL_BOSS_TELEPORTER << std::endl;
        if (temp_obj.get_type() == OBJ_FINAL_BOSS_TELEPORTER) {
            temp_obj.start();
        }
    }
}

// @TODO::IURI - those methods need to receive the map_n parameter //
Uint8 MapController::get_map_gfx()
{
    return SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).gfx_effect;
}

Uint8 MapController::get_map_gfx_mode()
{
    SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).gfx_mode;
    return 0;
}

st_float_position MapController::get_bg_scroll()
{
    //return bg_scroll;
    return st_float_position(0, 0);
}

void MapController::set_bg_scroll(st_float_position pos)
{
    //bg_scroll = pos;
}

st_rectangle MapController::get_player_hitbox()
{
    return GameManager::get_instance()->get_player()->get_hitbox();
}





// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void MapController::load_map_objects() {
    std::map<std::string, GameObject>::iterator it;

    // remove all elements currently in the list
    while (object_list.size() > 0) {
        object_list.pop_back();
    }

    while (animation_list.size() > 0) {
        animation_list.pop_back();
    }

    object_list.clear();

    unsigned int mapNumber = SharedData::get_instance()->v6_selected_area;
    for (int i=0; i<SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).size(); i++) {
        if (SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).id_object != -1) {

            //std::cout << "################# OBJ[" << i << "].name[" << SharedData::get_instance()->v6_object_list.at(SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).id_object).name  << "].uuid[" << SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).uuid << "]" << std::endl;


            GameObject temp_obj(SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).id_object, this, SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).start_point, SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).dest_position, SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).dest_map);
            temp_obj.set_obj_map_id(i);
            temp_obj.set_direction(SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).direction);
            temp_obj.set_uuid(SharedData::get_instance()->file_v6_map_object_map.at(mapNumber).at(i).uuid);
            st_position obj_state_id = st_position(SharedData::get_instance()->v6_selected_area, temp_obj.get_id());
            if (SharedData::get_instance()->game_object_state_map.find(temp_obj.get_uuid()) != SharedData::get_instance()->game_object_state_map.end()) {
                // finished objects should be ignored and not added to the map
                if (SharedData::get_instance()->game_object_state_map.at(temp_obj.get_uuid()).finished == true) {
                    continue;
                }
                st_position obj_state_position(SharedData::get_instance()->game_object_state_map.at(temp_obj.get_uuid()).x, SharedData::get_instance()->game_object_state_map.at(temp_obj.get_uuid()).y);
                //std::cout << "SET obj-pos to x[" << obj_state_position.x << "], y[" << obj_state_position.y << "]" << std::endl;
                temp_obj.set_position(obj_state_position);
            }
            // if object is smaller in width than the tileset, center it
            if (temp_obj.get_size().width < TILESIZE) {
                int obj_tile_x = temp_obj.get_position().x/TILESIZE;
                int new_pos_x = obj_tile_x*TILESIZE + TILESIZE/2 - temp_obj.get_size().width/2;
                temp_obj.set_position(st_position(new_pos_x, temp_obj.get_position().y));
            }

            object_list.push_back(temp_obj);
        }
    }
    //std::cout << "MapController::load_map_objects, count[" << object_list.size() << "]" << std::endl;
}





st_float_position MapController::get_last_scrolled() const
{
    return scrolled;
}

// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
void MapController::reset_scrolled()
{
    scrolled.x = 0;
    scrolled.y = 0;
}

bool MapController::value_in_range(int value, int min, int max) const
{
    return (value >= min) && (value <= max);
}

void MapController::create_dynamic_background_surfaces()
{
    unsigned int mapNumber = SharedData::get_instance()->v6_selected_area;
    if (SharedData::get_instance()->v6_area_list.size() <= mapNumber) {
        std::cout << "ERROR::map::loadMap - Invalid map number[" << mapNumber << "] for list.size[" << SharedData::get_instance()->v6_area_list.size() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }


    for (unsigned int i=0; i<LAYERS_COUNT; i++) {
        //std::cout << ">>>>>>>>>> MapController::create_dynamic_background_surfaces[" << i << "]], filename[" << bg_filename << "]" << std::endl;
        if (i < LAYERS_BG_COUNT) {
            addLayer(i, false);
        } else {
            addLayer(i, true);
        }
    }
}

st_imageData* MapController::get_dynamic_bg(int n)
{
    return &imageLayerMap.at(n).imageData;
}




int MapController::collision_rect_player_obj(st_rectangle player_rect, GameObject* temp_obj, const short int x_inc, const short int y_inc, const short obj_xinc, const short obj_yinc)
{
    int blocked = 0;
    int obj_y_reducer = 1;
    collision_detection rect_collision_obj;

// used to give char a small amount of pixels that he can enter inside object image

    st_position temp_obj_pos = temp_obj->get_position();

    st_rectangle obj_rect(temp_obj_pos.x+obj_xinc, temp_obj_pos.y+obj_yinc+obj_y_reducer, temp_obj->get_size().width, temp_obj->get_size().height);
    st_rectangle p_rect(player_rect.x+x_inc, player_rect.y+y_inc, player_rect.w, player_rect.h);

    // if only moving up/down, give one extra pivel free (otherwise in won't be able to jump next an object)

    if (x_inc == 0 && y_inc != 0) {
        p_rect.x++;
        p_rect.w -= 2;
    }

    if (temp_obj->get_type() == OBJ_ITEM_JUMP) {
        obj_rect.y += OBJ_JUMP_Y_ADJUST;
    } else {
        obj_rect.y += 1;
    }

    bool xObjOver = value_in_range(obj_rect.x, p_rect.x, p_rect.x + p_rect.w);
    bool xPlayerOver = value_in_range(p_rect.x, obj_rect.x, obj_rect.x + obj_rect.w);
    bool xOverlap = xObjOver == true || xPlayerOver == true;
    bool yOverlap = value_in_range(obj_rect.y, p_rect.y, p_rect.y + p_rect.h) || value_in_range(p_rect.y, obj_rect.y, obj_rect.y + obj_rect.h);

    // check if X is blocked
    bool before_collision = rect_collision_obj.rect_overlap(obj_rect, p_rect);
    if (before_collision == true && temp_obj->get_collision_mode() != COLlISION_MODE_Y) {
        blocked = BLOCK_X;
    }


    if (xOverlap == true && yOverlap == true) {
        if (blocked == 0) {
            blocked = BLOCK_Y;
        } else {
            blocked = BLOCK_XY;
        }
    }

    if (blocked != 0 && temp_obj->get_type() == OBJ_ACTIVE_OPENING_SLIM_PLATFORM) {
        if (abs(p_rect.y + p_rect.h - obj_rect.y) > y_inc || y_inc < 0) {
            std::cout << "SLIM - ignore block, y_inc[" << y_inc << "]" << std::endl;
            blocked = 0;
        }
    }

    //std::cout << "blocked: " << blocked << ", xOverlap: " << xOverlap << ", yOverlap: " << yOverlap << ", p.x: " << p_rect.x << ", p.y: " << p_rect.y << ", p.w: " << p_rect.w << ", p.h: " << p_rect.h << ", o.y: " << obj_rect.y << ", y_inc: " << y_inc << std::endl;


    return blocked;
}


// this method is used for npcs to ignore certain objects
bool MapController::is_obj_ignored_by_enemies(Uint8 obj_type)
{
    if (obj_type == OBJ_ENERGY_TANK) {
        return true;
    }
    if (obj_type == OBJ_ENERGY_PILL_BIG) {
        return true;
    }
    if (obj_type == OBJ_WEAPON_PILL_BIG) {
        return true;
    }
    if (obj_type == OBJ_ENERGY_PILL_SMALL) {
        return true;
    }
    if (obj_type == OBJ_WEAPON_PILL_SMALL) {
        return true;
    }
    if (obj_type == OBJ_ITEM_FLY) {
        return true;
    }
    if (obj_type == OBJ_ITEM_JUMP) {
        //std::cout << "IGNORE OBJ_ITEM_JUMP" << std::endl;
        return true;
    }
    if (obj_type == OBJ_BOSS_TELEPORTER) {
        return true;
    }
    if (obj_type == OBJ_STAGE_BOSS_TELEPORTER) {
        return true;
    }
    if (obj_type == OBJ_PLATFORM_TELEPORTER) {
        return true;
    }
    if (obj_type == OBJ_FRONT_DOOR_TELEPORTER) {
        return true;
    }
    if (obj_type == OBJ_FINAL_BOSS_TELEPORTER) {
        return true;
    }
    if (obj_type == OBJ_BOSS_DOOR) {
        return true;
    }
    if (obj_type == OBJ_CHECKPOINT) {
        return true;
    }
    if (obj_type == OBJ_DOOR_AREA_LINK) {
        return true;
    }
    if (obj_type == OBJ_DOOR_KEY) {
        return true;
    }
    if (obj_type == OBJ_DOOR_LOCKED) {
        return true;
    }
    if (obj_type == OBJ_GAME_ITEM_PICKEABLE || obj_type == OBJ_GAME_ITEM_STATIC) {
        return true;
    }
    return false;
}


void MapController::collision_char_object(character* charObj, const float x_inc, const short int y_inc)
{
    int blocked = 0;
    GameObject* res_obj = nullptr;

    //if (y_inc < 0) std::cout << "MAP::collision_player_object - y_inc: " << y_inc << std::endl;

    // ignore collision if teleporting
    if (charObj->get_anim_type() == ANIM_TYPE_TELEPORT) {
        return;
    }

    st_rectangle char_rect = charObj->get_hitbox();

    //std::cout << ">>>>>>>>> MapController::collision-player-object, char_rect.y[" << char_rect.y << "]" << std::endl;

    /// @TODO: isso aqui deveria mesmo estar aqui?
    if (charObj->get_platform() == nullptr) {
        for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {

            // check if list was changed to avoid crash //
            if (map_was_reloaded == true) {
                break;
            }

            GameObject& temp_obj = (*it);
            //std::cout << "### obj[" << temp_obj.get_name() << "] - CHECK #0 ###" << std::endl;

            if (temp_obj.is_hidden() == true) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #1" << std::endl;
                continue;
            }

            if (temp_obj.finished() == true) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave (finished)" << std::endl;
                continue;
            }

            if (temp_obj.is_on_screen() == false) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #2" << std::endl;
                continue;
            }

            if (temp_obj.finished() == true) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #3" << std::endl;
                continue;
            }

            if (charObj->is_player() == false && is_obj_ignored_by_enemies(temp_obj.get_type())) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #4" << std::endl;
                continue;
            }

            // slim platform won't collide if movement is from bottom to top
            if (temp_obj.get_type() == OBJ_ACTIVE_OPENING_SLIM_PLATFORM && y_inc < 0) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #5" << std::endl;
                continue;
            }

            if (temp_obj.is_teleporting()) {
                //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #6 [teleporting object]" << std::endl;
                continue;
            }

            // jumping from inside item-coil must not block player
            if (temp_obj.get_type() == OBJ_ITEM_JUMP && y_inc < 0) {
                continue;
            }

            st_rectangle stopped_char_rect = charObj->get_hitbox();
            stopped_char_rect.x+= CHAR_OBJ_COLlISION_KILL_ADJUST/2;
            stopped_char_rect.y+= CHAR_OBJ_COLlISION_KILL_ADJUST;
            stopped_char_rect.w-= CHAR_OBJ_COLlISION_KILL_ADJUST;
            stopped_char_rect.h-= CHAR_OBJ_COLlISION_KILL_ADJUST*2;

            //std::cout << "collision_rect_player_obj::CALL #1" << std::endl;
            // check if, without moving, player is inside object
            int no_move_blocked = collision_rect_player_obj(stopped_char_rect, &temp_obj, 0, 0, 0, 0);


            //if (charObj->is_player()) std::cout << "### obj[" << temp_obj.get_name() << "] - CHECK #1 ###" << std::endl;


            // some platforms can kill the player if he gets stuck inside it
            if (charObj->is_player() == true && (temp_obj.get_type() == OBJ_MOVING_PLATFORM_UPDOWN || temp_obj.get_type() == OBJ_FLY_PLATFORM)) {
                if (no_move_blocked == BLOCK_XY) {
                    _obj_collision = object_collision(BLOCK_INSIDE_OBJ, &temp_obj);
                    //if (charObj->is_player()) std::cout << "obj[" << temp_obj.get_name() << "] - leave #5" << std::endl;
                    return;
                }
            }

            // usar TEMP_BLOCKED aqui, para não zerar o blocked anterior, fazer merge dos valores
            int temp_blocked = 0;
            //std::cout << "collision_rect_player_obj::CALL #2" << std::endl;
            temp_blocked = collision_rect_player_obj(char_rect, &temp_obj, x_inc, y_inc, 0, 0);

            //if (charObj->is_player()) std::cout << "### obj[" << temp_obj.get_name() << "] - CHECK #A::temp_blocked[" << temp_blocked << "] ###" << std::endl;


            int temp_obj_y = temp_obj.get_position().y;
            if (temp_obj.get_type() == OBJ_ITEM_JUMP) {
                temp_obj_y += OBJ_JUMP_Y_ADJUST;
            }

            //if (charObj->is_player()) std::cout << "### obj[" << temp_obj.get_name() << "] - CHECK #2 - temp_blocked[" << temp_blocked << "], is_platform[" << temp_obj.is_platform() << "] ###" << std::endl;

            // to enter platform, player.x+player.h must not be much higher than obj.y
            if (temp_blocked != 0 && temp_obj.is_platform() == false) {

                //std::cout << "COLLISION WITH OBJECT, Type[" << (int)temp_obj.get_type() << "]" << std::endl;

                if (temp_obj.get_type() == OBJ_CHECKPOINT) {
                    if (temp_obj.is_started() == false) {
                        temp_obj.start();
                    }
                    SharedData::get_instance()->checkpoint.x = charObj->getPosition().x;
                    SharedData::get_instance()->checkpoint.y = (charObj->getPosition().y+charObj->get_size().height-1);
                    SharedData::get_instance()->checkpoint.map = SharedData::get_instance()->v6_selected_area;
                    SharedData::get_instance()->checkpoint.map_scroll_x = GameManager::get_instance()->get_current_map_obj()->getMapScrolling().x;
                    return;
                } else if (temp_obj.get_type() == OBJ_BOSS_DOOR) {
                    if (temp_obj.is_started() == false && subboss_alive_on_left(temp_obj.get_position().x/TILESIZE) == false) {
                        // check for sub-boss alive on the left
                        temp_obj.start();
                        if (charObj->get_int_position().x > temp_obj.get_position().x + temp_obj.get_size().width) {
                            temp_obj.set_direction(ANIM_DIRECTION_LEFT);
                        } else {
                            temp_obj.set_direction(ANIM_DIRECTION_RIGHT);
                        }
                    }
                } else if (temp_obj.get_type() == OBJ_DOOR_AREA_LINK) {
                    std::cout << ">>>>>>>>>>> OBJ_DOOR_AREA_LINK" << std::endl;
                    //gameManager::get_instance()->check_map_link(charObj->get_last_moved().x, charObj->get_last_moved().y);
                    GameManager::get_instance()->check_map_link(x_inc, y_inc);
                    if (map_was_reloaded == true) {
                        return;
                    }
                    if (temp_blocked == BLOCK_XY) {
                        temp_blocked = BLOCK_Y;
                    } else {
                        temp_blocked = BLOCK_UNBLOCKED;
                    }
                } else if (charObj->is_player() == true && temp_obj.get_type() == OBJ_DOOR_LOCKED) {
                    // player has key, start the door (animate/open)
                    if (SharedData::get_instance()->game_save.keys[temp_obj.get_key_n()] == true) {
                        if (temp_obj.is_started() == false) {
                            temp_obj.start();
                        }
                        // if opening animation is finished, can cross it
                        if (temp_obj.get_state() == 1) {
                            temp_blocked = BLOCK_UNBLOCKED;
                        }

                    }
                    //std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%% OBJ_DOOR_LOCKED" << std::endl;
                    // @TODO //
                } else if (charObj->is_player() == true && temp_obj.get_type() == OBJ_GAME_ITEM_PICKEABLE) {
                    temp_blocked = BLOCK_UNBLOCKED;
                    if (InputController::get_instance()->p1_input[BTN_ITEM] == 1) {
                        charObj->pick_game_item(temp_obj);
                        return;
                    }
                } else if (charObj->is_player() == true && temp_obj.get_type() == OBJ_GAME_ITEM_STATIC) {
                    temp_blocked = BLOCK_UNBLOCKED;
                } else if (charObj->is_player() == true && temp_obj.get_type() == OBJ_GAME_ITEM_STATIC) {
                    temp_blocked = BLOCK_UNBLOCKED;
                }
            }

            //std::cout << "### obj[" << temp_obj.get_name() << "] - CHECK #3 - temp_blocked[" << temp_blocked << "], ###" << std::endl;

            if (temp_blocked == BLOCK_Y || temp_blocked == BLOCK_XY) {

                //std::cout << "### obj[" << temp_obj.get_name() << "] - COLLISION #1" << std::endl;

                bool entered_platform = false;

                if (temp_obj.get_state() != 0 && temp_obj.get_type() == OBJ_TRACK_PLATFORM) {
                    continue;
                }

                if (charObj->is_player() == true && temp_obj.get_state() != 0 && (temp_obj.get_type() == OBJ_RAY_VERTICAL || temp_obj.get_type() == OBJ_RAY_HORIZONTAL)) {
                    //std::cout << "############# RAY.DAMAGE #############" << std::endl;
                    charObj->damage(TOUCH_DAMAGE_BIG, false);
                    continue;
                } else if (charObj->is_player() == true && temp_obj.get_state() != 0 && (temp_obj.get_type() == OBJ_DEATHRAY_VERTICAL || temp_obj.get_type() == OBJ_DEATHRAY_HORIZONTAL)) {
                    std::cout << "DEATHRAY(damage) - player.x: " << char_rect.x << ", map.scroll_x: " << scroll.x << ", pos.x: " << temp_obj.get_position().x << ", size.w: " << temp_obj.get_size().width << std::endl;
                    charObj->damage(999, false);
                    continue;
                }

                // if inside object and is disappearing block, move char above it
                if (no_move_blocked == BLOCK_XY && temp_blocked == BLOCK_XY && (charObj->is_player() && temp_obj.get_type() == OBJ_DISAPPEARING_BLOCK || charObj->is_player() && temp_obj.get_type() == OBJ_ACTIVE_DISAPPEARING_BLOCK)) {
                    charObj->set_position(st_position(charObj->get_int_position().x, temp_obj.get_position().y - charObj->get_size().height));
                }

                if (y_inc > 0 && char_rect.y <= temp_obj_y) {
                    entered_platform = true;
                }

                //std::cout << "### obj[" << temp_obj.get_name() << "] - COLLISION #3, entered_platform[" << entered_platform << "]" << std::endl;

                if (entered_platform == true) {
                    if (temp_obj.is_hidden() == false && (temp_obj.get_type() == OBJ_MOVING_PLATFORM_UPDOWN || temp_obj.get_type() == OBJ_MOVING_PLATFORM_LEFTRIGHT || temp_obj.get_type() == OBJ_DISAPPEARING_BLOCK)) {
                        if (charObj->get_platform() == nullptr && (temp_blocked == 2 || temp_blocked == 3)) {


                            charObj->set_platform(&temp_obj);
                            if (temp_obj.get_type() == OBJ_FALL_PLATFORM) {
                                temp_obj.set_direction(ANIM_DIRECTION_LEFT);
                            }
                        } else if (charObj->get_platform() == nullptr && temp_blocked == 1) {
                            charObj->set_platform(&temp_obj);
                        }
                        if (temp_blocked != 0) {
                            _obj_collision = object_collision(temp_blocked, &(*it));
                            return;
                        }
                    } else if (temp_obj.get_type() == OBJ_ITEM_FLY) {
                        if (charObj->get_platform() == nullptr && (temp_blocked == 2 || temp_blocked == 3) && y_inc > 0) {
                            charObj->set_platform(&temp_obj);
                            if (temp_obj.get_distance() == 0) {
                                temp_obj.start();
                                temp_obj.set_distance(1);
                                temp_obj.set_timer(TimerView::get_instance()->getTimer()+30);
                            }
                        }
                        if (temp_blocked != 0) {
                            _obj_collision = object_collision(temp_blocked, &(*it));
                            return;
                        }
                    } else if (temp_obj.get_type() == OBJ_ITEM_JUMP) {
                        if (charObj->get_platform() == nullptr && (temp_blocked == 2 || temp_blocked == 3) && y_inc > 0 && charObj->getPosition().y+charObj->get_size().height <= temp_obj_y+1) {
                            charObj->activate_super_jump();
                            charObj->activate_force_jump();
                            temp_obj.start();
                        }
                        if (temp_blocked != 0) {
                            if (y_inc > 0) {
                                //std::cout << ">>>> temp_blocked: " << temp_blocked << ", y_inc: " << y_inc << std::endl;
                                _obj_collision = object_collision(temp_blocked, &(*it));
                                return;
                            } else {
                                std::cout << ">>>> RESET BLOCKED" <<  std::endl;
                                temp_blocked = 0;
                            }
                        }
                    } else if (temp_obj.is_hidden() == false && temp_obj.is_started() == false && (temp_obj.get_type() == OBJ_ACTIVE_DISAPPEARING_BLOCK || temp_obj.get_type() == OBJ_ACTIVE_OPENING_SLIM_PLATFORM)) {
                        temp_obj.start();
                    } else if (temp_obj.get_type() == OBJ_FALL_PLATFORM || temp_obj.get_type() == OBJ_FLY_PLATFORM) {
                        if (charObj->get_platform() == nullptr) {
                            charObj->set_platform(&temp_obj);
                            if (temp_obj.get_state() == OBJ_STATE_STAND) {
                                temp_obj.set_state(OBJ_STATE_MOVE);
                                temp_obj.start();
                            }
                            temp_obj.set_timer(TimerView::get_instance()->getTimer()+30);
                            _obj_collision = object_collision(temp_blocked, &(*it));
                            return;
                        }
                    } else if (temp_obj.get_type() == OBJ_TRACK_PLATFORM) {
                        if (charObj->get_platform() == nullptr) {
                            charObj->set_platform(&temp_obj);
                            _obj_collision = object_collision(temp_blocked, &(*it));
                            return;
                        }
                    } else if (temp_obj.get_type() == OBJ_DAMAGING_PLATFORM) {
                        if (charObj->get_platform() == nullptr) {
                            charObj->set_platform(&temp_obj);
                            _obj_collision = object_collision(temp_blocked, &(*it));
                            temp_obj.start();
                            return;
                        }
                    }



                }

                if (temp_blocked != 0) {
                    res_obj = &(*it);
                }

            }



            // merge blocked + temp_blocked
            if (temp_blocked == BLOCK_X) {
                if (blocked == 0) {
                    blocked = BLOCK_X;
                } else if (blocked == BLOCK_Y) {
                    blocked = BLOCK_XY;
                }
            } else if (temp_blocked == BLOCK_Y) {
                if (blocked == 0) {
                    blocked = BLOCK_Y;
                } else if (blocked == BLOCK_X) {
                    blocked = BLOCK_XY;
                }
            } else if (temp_blocked == BLOCK_XY) {
                blocked = BLOCK_XY;
            }
        }




    // this part seems to be OK
    } else {
        GameObject* temp_obj = charObj->get_platform();
        if (temp_obj->is_hidden() == true) {
            std::cout << "obj[" << temp_obj->get_name() << "] - leave #2.1" << std::endl;
            charObj->set_platform(nullptr);
        } else if (temp_obj->get_type() == OBJ_TRACK_PLATFORM && temp_obj->get_state() != 0) {
            std::cout << "obj[" << temp_obj->get_name() << "] - leave #2.2" << std::endl;
            charObj->set_platform(nullptr);
        } else {
            //std::cout << "collision_rect_player_obj::CALL #3" << std::endl;
            blocked = collision_rect_player_obj(char_rect, temp_obj, x_inc, y_inc, 0, 0);
            if (blocked != 0) {
                res_obj = temp_obj;
            }
            //std::cout << "IN-PLATFORM[" << temp_obj->get_name() << "], blocked[" << blocked << "], y_inc[" << y_inc << "]" << std::endl;
        }
    }


    // got out of platform
    if (blocked == 0 && charObj->get_platform() != nullptr) {
        //  for player item, platform must only be removed only if the item was already adtivated
        if (charObj->get_platform()->get_type() == OBJ_ITEM_FLY || charObj->get_platform()->get_type() == OBJ_ITEM_JUMP) {
            //std::cout << "### DEBUG OBJ_ITEM_JUMP #1 ###" << std::endl;
            if (charObj->get_platform()->get_distance() > 0 && y_inc != 0) {
                std::cout << "CHAR::OUT-PLATFORM #1" << std::endl;
                charObj->set_platform(nullptr);
            } else {
                _obj_collision = object_collision(0, nullptr);
                return;
            }
        } else if (charObj->get_platform()->is_hidden() == true) {
            //std::cout << ">> OUT OF PLATFORM #2" << std::endl;
            charObj->set_platform(nullptr);
        } else {
            _platform_leave_counter++;
            if (_platform_leave_counter > 2) {
                //std::cout << ">> OUT OF PLATFORM #3" << std::endl;
                charObj->set_platform(nullptr);
                _platform_leave_counter = 0;
            }
        }
    } else if (blocked != 0 && charObj->get_platform() != nullptr) {
        _platform_leave_counter = 0;
    }


    _obj_collision = object_collision(blocked, res_obj);
}

object_collision MapController::get_obj_collision()
{
    return _obj_collision;
}




void MapController::clean_map_npcs_projectiles()
{
    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        npc_ref->clean_projectiles();
    }
}

void MapController::reset_beam_objects()
{
    // reset objects
    for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        GameObject& temp_obj = (*it);
        short obj_type = temp_obj.get_type();
        if (obj_type == OBJ_DEATHRAY_VERTICAL || obj_type == OBJ_DEATHRAY_HORIZONTAL || obj_type == OBJ_RAY_VERTICAL || obj_type == OBJ_RAY_HORIZONTAL) {
            temp_obj.reset();
        }
    }
}

void MapController::remove_temp_objects()
{
    // reset objects
    for (std::vector<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); it++) {
        GameObject& temp_obj = (*it);
        if (temp_obj.get_is_dropped() == true) {
            temp_obj.set_finished(true);
        }
    }
    clean_finished_objects();
}



bool MapController::get_map_point_wall_lock(int x)
{
    return isEdgeColumnLocked(0, x/TILESIZE);
}

void MapController::move_map(const short int move_x, const short int move_y)
{
    set_scrolling(st_float_position(scroll.x+move_x, scroll.y+move_y));
}


// ********************************************************************************************** //
//                                                                                                //
// ********************************************************************************************** //
GameEnemy* MapController::collision_player_npcs(character* playerObj, const short int x_inc, const short int y_inc)
{
    struct st_rectangle p_rect, npc_rect;

    p_rect = playerObj->get_hitbox();

    //std::cout << "collision_player_npcs - p1.x: " << p1.x << ", p1.y: " << p1.y << std::endl;
    collision_detection rect_collision_obj;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if (npc_ref->is_player_friend() == true) {
            //std::cout << "collision_player_npcs - FRIEND" << std::endl;
            continue;
        }
        if (npc_ref->is_dead() == true) {
            //std::cout << "collision_player_npcs - DEAD" << std::endl;
            continue;
        }
        if (npc_ref->is_invisible() == true) {
            //std::cout << "collision_player_npcs - INVISIBLE" << std::endl;
            continue;
        }

        if (npc_ref->is_on_visible_screen() == false) {
            continue;
        }

        if (npc_ref->is_intangible() == true) {
            continue;
        }


        // special-bosses have multiple hit areas, so we grab it instead of a single rectangle
        if (GameManager::get_instance()->is_special_boss(npc_ref->get_name())) {
            if (npc_ref->get_name() == "Rotate Test") {
                std::vector<st_rectangle> collision_list = npc_ref->get_collision_list_boss_001();
                for (unsigned int i=0; i<collision_list.size(); i++) {
                    if (rect_collision_obj.rect_overlap(collision_list.at(i), p_rect) == true) {
                        return npc_ref;
                    }
                }
            }
        } else {

            npc_rect = npc_ref->get_hitbox();

            if (rect_collision_obj.rect_overlap(npc_rect, p_rect) == true) {

                if (npc_ref->get_is_npc() == true) {
                    // NPC dialog //
                    if (InputController::get_instance()->p1_input[BTN_UP] == 1) {
                        std::vector<std::string> message1;
                        message1.push_back(std::string("Welcome to Corneria"));
                        st_dialog dialog1;
                        dialog1.msgs = message1;
                        GameManager::get_instance()->add_queue_dialog(dialog1);

                        std::vector<std::string> message2;
                        message2.push_back(std::string("I like Swords!"));
                        st_dialog dialog2;
                        dialog2.msgs = message2;
                        GameManager::get_instance()->add_queue_dialog(dialog2);
                    } else {
                        draw::get_instance()->draw_game_button(playerObj->get_real_position().x+playerObj->get_size().width/2, playerObj->get_real_position().y-20, INPUT_IMAGES_DPAD_UP);
                    }
                    return nullptr;
                }


                return npc_ref;
            }
        }
    }
    return nullptr;
}


// kills any NPC that touches player during player's special attack
void MapController::collision_player_special_attack(character* playerObj, const short int x_inc, const short int y_inc, short int reduce_x, short int reduce_y)
{
    struct st_rectangle p_rect, npc_rect;

    //reduce = abs((float)16-playerObj->sprite->w)*0.5;

    // ponto 3, topo/esquerda
    if (playerObj->get_direction() == ANIM_DIRECTION_LEFT) {
        p_rect.x = playerObj->getPosition().x + reduce_x;
        p_rect.w = playerObj->get_size().width;
    } else {
        p_rect.x = playerObj->getPosition().x;
        p_rect.w = playerObj->get_size().width - reduce_x;
    }
    p_rect.y = playerObj->getPosition().y + reduce_y;
    p_rect.h = playerObj->get_size().height;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if (npc_ref->is_player_friend() == true) {
            continue;
        }
        if (npc_ref->is_dead() == true) {
            continue;
        }
        if (npc_ref->is_invisible() == true) {
            continue;
        }

        if (npc_ref->is_on_visible_screen() == false) {
            continue;
        }


        npc_rect.x = npc_ref->getPosition().x;
        npc_rect.w = npc_ref->get_size().width;
        npc_rect.y = npc_ref->getPosition().y;
        npc_rect.h = npc_ref->get_size().height;

        if (npc_ref->get_size().width >= TILESIZE) { // why is this here??? O.o
            npc_rect.x = npc_ref->getPosition().x+PLAYER_NPC_COLLISION_REDUTOR;
            npc_rect.w = npc_ref->get_size().width-PLAYER_NPC_COLLISION_REDUTOR;
        }
        if (npc_ref->get_size().height >= TILESIZE) {
            npc_rect.y = npc_ref->getPosition().y+PLAYER_NPC_COLLISION_REDUTOR;
            npc_rect.h = npc_ref->get_size().height-PLAYER_NPC_COLLISION_REDUTOR;
        }
        collision_detection rect_collision_obj;
        if (rect_collision_obj.rect_overlap(npc_rect, p_rect) == true) {
            npc_ref->damage(12, false);
        }
    }
}

GameEnemy* MapController::find_nearest_npc(st_position pos)
{
    int min_dist = 9999;
    GameEnemy* min_dist_npc = nullptr;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if (npc_ref->is_player_friend() == true) {
            //std::cout << "collision_player_npcs - FRIEND" << std::endl;
            continue;
        }
        if (npc_ref->is_dead() == true) {
            //std::cout << "collision_player_npcs - DEAD" << std::endl;
            continue;
        }
        if (npc_ref->is_invisible() == true) {
            //std::cout << "collision_player_npcs - INVISIBLE" << std::endl;
            continue;
        }
        if (npc_ref->is_on_visible_screen() == false) {
            continue;
        }
        float dist = sqrt(pow((pos.x - npc_ref->getPosition().x), 2) + pow((pos.y - npc_ref->getPosition().y), 2));
        if (dist < min_dist) {
            min_dist_npc = npc_ref;
            min_dist = dist;
        }
    }
    return min_dist_npc;
}

GameEnemy *MapController::find_nearest_npc_on_direction(st_position pos, int direction)
{
    int lower_dist = 9999;
    GameEnemy* ret = nullptr;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if (npc_ref->is_on_visible_screen() == false) {
            continue;
        }
        if (npc_ref->is_dead() == true) {
            continue;
        }

        st_position npc_pos(npc_ref->getPosition().x*TILESIZE, npc_ref->getPosition().y*TILESIZE);
        npc_pos.x = (npc_pos.x + npc_ref->get_size().width/2)/TILESIZE;
        npc_pos.y = (npc_pos.y + npc_ref->get_size().height)/TILESIZE;

        // if facing left, ignore enemies with X greater than player x
        if (direction == ANIM_DIRECTION_LEFT && npc_pos.x > pos.x) {
            continue;
        }
        // if facing right, ignore enemies with X smaller than player x+width
        if (direction == ANIM_DIRECTION_RIGHT && npc_pos.x < pos.x) {
            continue;
        }

        // pitagoras: raiz[ (x2-x1)^2 + (y2-y1)^2 ]
        int dist = sqrt(pow((float)(pos.x - npc_pos.x), (float)2) + pow((float)(pos.y - npc_pos.y ), (float)2));
        if (dist < lower_dist) {
            lower_dist = dist;
            ret = npc_ref;
        }
    }
    return ret;
}



void MapController::add_animation(ANIMATION_TYPES pos_type, st_imageData* surface, const st_float_position &pos, st_position adjust_pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize)
{
    //animation(ANIMATION_TYPES pos_type,
    //int frames_n,
    //const st_float_position &pos,
    //unsigned int frame_time,
    //unsigned int repeat_times,
    //int direction,
    //st_size framesize)
    int frames_n = surface->surface->w / framesize.width;
    animation_list.push_back(animation(pos_type, frames_n, pos, frame_time, repeat_times, direction, framesize));
}

void MapController::add_animation(animation anim)
{
    animation_list.push_back(anim);
}

void MapController::clear_animations()
{
    animation_list.erase(animation_list.begin(), animation_list.end());
}

GameEnemy* MapController::spawn_map_npc(short npc_id, st_position npc_pos, short int direction, bool player_friend, bool progressive_span)
{

#ifdef ANDROID
    __android_log_print(ANDROID_LOG_INFO, "###ROCKDROID2###", "MAP::spawn_map_npc, id[%d]", npc_id);
#endif

    //std::cout << "$$$ MAP::SPAWN-NPC, pos[" << npc_pos.x << ", " << npc_pos.y << "], map.scroll.x[" << scroll.x << "]" << std::endl;

    GameEnemy new_npc(SharedData::get_instance()->v6_selected_area, npc_id, npc_pos, direction, player_friend);

    if (progressive_span == true) {
        new_npc.set_progressive_appear_pos(new_npc.get_size().height);
    }
    map_enemy_spawn_list.push_back(new_npc); // insert new npc at the list-end

    GameEnemy* npc_ref = &(map_enemy_spawn_list.back());

    int id = npc_ref->get_number();
    std::string npc_name = npc_ref->get_name();

    return npc_ref;
}

int MapController::child_npc_count(int parent_id)
{
    int count = 0;
    std::vector<GameEnemy>::iterator npc_it;
    int n = 0;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        //std::cout << "NPC[" << n << "][" << npc_ref->get_name() << "].parent[" << npc_ref->get_parent_id() << ", parent_id[" << parent_id << "]" << std::endl;
        if (npc_ref->is_dead() == false && npc_ref->get_parent_id() == parent_id) {
            count++;
        }
        n++;
    }
    for (npc_it = map_enemy_spawn_list.begin(); npc_it != map_enemy_spawn_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        //std::cout << "NPC.SPANWLIST[" << n << "][" << npc_ref->get_name() << "].parent[" << npc_ref->get_parent_id() << ", parent_id[" << parent_id << "]" << std::endl;
        if (npc_ref->is_dead() == false && npc_ref->get_parent_id() == parent_id) {
            count++;
        }
        n++;
    }
    return count;
}


void MapController::move_npcs() /// @TODO - check out of screen
{
    //std::cout << "*************** MapController::showMap - npc_list.size: " << _npc_list.size() << std::endl;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {

        GameEnemy* npc_ref = &(*npc_it);
        // check if NPC is outside the visible area
        st_position npc_pos = npc_ref->get_real_position();
        short dead_state = npc_ref->get_dead_state();

        //std::cout << "MapController::move_npcs[" << npc_ref->get_name() << "]" << std::endl;

        std::string name(npc_ref->get_name());


        if (npc_ref->is_on_screen() != true) {
            if (dead_state == 2 && npc_ref->is_boss() == false && npc_ref->is_subboss()) {
                npc_ref->revive();
            }
            npc_ref->move_projectiles();
            continue; // no need for moving NPCs that are out of sight
        } else if (dead_state == 2 && npc_ref->auto_respawn() == true && npc_ref->is_boss() == false) {
            npc_ref->reset_position();
            npc_ref->revive();
            continue;
        } else if (dead_state == 1 && npc_ref->is_spawn() == false && npc_ref->is_boss() == false) {// drop item
            drop_item(npc_ref);
        }

        npc_ref->execute(); // TODO: must pass scroll map to npcs somwhow...

        if (dead_state == 1) {
            if (npc_ref->is_stage_boss() == false) {
                npc_ref->execute_ai(); // to ensure death-reaction is run

                // sub-boss have a different explosion
                if (npc_ref->is_subboss()) {
                    SoundView::get_instance()->play_repeated_sfx(SFX_BIG_EXPLOSION, 1);
                    st_float_position pos1(npc_ref->getPosition().x+2, npc_ref->getPosition().y+20);
                    add_animation(ANIMATION_STATIC, &ImageView::get_instance()->bomb_explosion_surface, pos1, st_position(-8, -8), 80, 2, npc_ref->get_direction(), st_size(56, 56));
                    st_float_position pos2(pos1.x+50, pos1.y-30);
                    add_animation(ANIMATION_STATIC, &ImageView::get_instance()->bomb_explosion_surface, pos2, st_position(-8, -8), 80, 2, npc_ref->get_direction(), st_size(56, 56));
                } else if (npc_ref->getPosition().y < RES_H) { // don't add death explosion when dying out of screen
                    add_animation(ANIMATION_STATIC, &ImageView::get_instance()->explosion32, npc_ref->getPosition(), st_position(-8, -8), 80, 2, npc_ref->get_direction(), st_size(32, 32));
                }
                // check if boss flag wasn't passed to a spawn on dying reaction AI
                if (npc_ref->is_boss()) {
                    GameManager::get_instance()->check_player_return_teleport();
                }

                // all kinds of bosses need to remove projectiles once dying
                if (npc_ref->is_boss() || npc_ref->is_subboss() || npc_ref->is_stage_boss()) {
                    npc_ref->clean_projectiles();
                // regular enemies only remove effect-type projectiles (quake, wind, freeze, etc)
                } else {
                    npc_ref->clean_effect_projectiles();
                }
            } else {

                std::cout << "##### STAGE-BOSS IS DEAD (#1) #####" << std::endl;

                // run npc move one more time, so reaction is executed to test if it will spawn a new boss (replace-itself)
                for (int i=0; i<2; i++) {
                    npc_ref->execute_ai(); // to ensure death-reaction is run
                }


                if (npc_ref->is_stage_boss() == false) { // if now the NPC is not the stage boss anymore, continue
                    std::cout << "##### STAGE-BOSS IS DEAD (#2) #####" << std::endl;
                    GameManager::get_instance()->draw_explosion(npc_pos, true);
                    SoundView::get_instance()->play_boss_music();
                    ImageView::get_instance()->blink_screen(255, 255, 255);
                    continue;
                } else {
                    std::cout << "##### STAGE-BOSS IS DEAD (#3) #####" << std::endl;
                    GameManager::get_instance()->remove_all_projectiles();
                    std::cout << "MapController::showMap - killed stage boss" << std::endl;
                    /// @TODO - replace with game_data.final_boss_id
                    if (SharedData::get_instance()->game_data.final_boss_id == npc_ref->get_number()) {
                        SoundView::get_instance()->stop_music();
                        GameManager::get_instance()->draw_explosion(npc_pos, true);
                        ImageView::get_instance()->blink_screen(255, 255, 255);
                        ImageView::get_instance()->clearScreenArea(0, 0, RES_W, RES_H, 0, 0, 0);
                        ImageView::get_instance()->updateRender();
                        TimerView::get_instance()->delay(1000);
                        GameManager::get_instance()->show_ending();
                        return;
                    } else {
                        GameManager::get_instance()->draw_explosion(npc_pos, true);
                    }
                }
            }
            return;
        }
    }

    if (map_enemy_spawn_list.size() > 0) {
        std::vector<GameEnemy>::iterator npc_it;
        for (npc_it = map_enemy_spawn_list.begin(); npc_it != map_enemy_spawn_list.end(); npc_it++) {
            //std::cout << "(B) ######### _npc_list.add, size[" << _npc_list.size() << "]" << std::endl;
            map_enemy_list.push_back(*npc_it);
        }
        map_enemy_spawn_list.clear();
    }
}

void MapController::show_npcs() /// @TODO - check out of screen
{
    bool has_boss = false;

    //std::cout << "%%%%%%%%% MapController::show_npcs - _npc_list.size[" << _npc_list.size() << "]" << std::endl;

    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);

        //std::cout << "%%%%%%%%% MapController::show_npcs - show[" << npc_ref->get_name() << "], dead[" << npc_ref->is_dead() << "]" << std::endl;


        if (GameManager::get_instance()->must_show_boss_hp() && npc_ref->is_boss() && npc_ref->is_on_visible_screen() == true) {
            has_boss = true;
            draw::get_instance()->set_boss_hp(npc_ref->get_current_hp());
        }
        if (npc_ref->is_dead() == false) {
            npc_ref->show();
        }
        npc_ref->show_projectiles();
    }
    if (has_boss == false) {
        draw::get_instance()->set_boss_hp(-99);
    }
}

void MapController::show_npcs_to_left(int x)
{
    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        //std::cout << "MAP::show_npcs_to_left[" << npc_ref->get_name() << "], x[" << x << "], npc.x[" << npc_ref->getPosition().x << "]" << std::endl;
        if (npc_ref->is_dead() == false && npc_ref->is_on_visible_screen() && npc_ref->getPosition().x <= x) {
            npc_ref->show();
        }
        npc_ref->show_projectiles();
    }
    draw::get_instance()->set_boss_hp(-99);
}



void MapController::build_screen_area_object_list()
{
    on_screen_area_object_list.clear();
    std::vector<GameObject>::iterator object_it;
    //for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
    for (int i=0; i<object_list.size(); i++) {
        if (object_list.at(i).is_on_screen() == true) {
            if (object_list.at(i).finished() == true) {
                std::cout << "OBJ[" << object_list.at(i).get_name() << "] is finished, remove it from lists" << std::endl;
                object_list.erase(object_list.begin()+i);
                break;
            }
            on_screen_area_object_list.push_back(i);
        }
    }
    //std::cout << "MapController::build_screen_area_object_list - on_screen_area_object_list.size[" << on_screen_area_object_list.size() << "]" << std::endl;
}

void MapController::move_objects(bool paused)
{

    for (int i=0; i<on_screen_area_object_list.size(); i++) {
        GameObject *obj_ref = &object_list.at(on_screen_area_object_list.at(i));
        obj_ref->execute(paused);

        st_float_position obj_real_pos = obj_ref->get_relative_position();
        st_size obj_size = obj_ref->get_size();
        SharedData::get_instance()->lightpoint_list.push_back(st_light_point(obj_real_pos.x+obj_size.width/2, obj_real_pos.y+obj_size.height/2, LIGHT_POINT_COLOR_YELLOW));
    }

}

void MapController::clean_finished_objects()
{
    std::vector<GameObject>::iterator object_it;
    std::vector<GameObject> kept_object_list;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        if ((*object_it).finished() == false) {
            kept_object_list.push_back(*object_it);
        }
    }
    object_list = kept_object_list;
}

std::vector<GameObject*> MapController::check_collision_with_objects(st_rectangle collision_area)
{
    std::vector<GameObject*> res;

    for (unsigned int i=0; i<object_list.size(); i++) {
        GameObject* temp_obj = &object_list.at(i);
        collision_detection rect_collision_obj;
        bool res_collision = rect_collision_obj.rect_overlap(temp_obj->get_area(), collision_area);
        if (res_collision == true) {
            res.push_back(temp_obj);
        }
    }
    return res;
}

void MapController::show_objects(int adjust_y, int adjust_x)
{
    /// @TODO - update timers
    std::vector<GameObject>::iterator object_it;
    int n = 0;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        if ((*object_it).get_type() != OBJ_STAGE_BOSS_TELEPORTER && (*object_it).get_type() != OBJ_BOSS_TELEPORTER && (*object_it).get_type() != OBJ_FINAL_BOSS_TELEPORTER && (*object_it).get_type() != OBJ_DOOR_AREA_LINK && (*object_it).get_type() != OBJ_DOOR_LOCKED) { // teleporters are shown above
            //std::cout << ">>> MapController::show_objects - OBJ[" << n << "], name[" << (*object_it).get_name() << ", type[" << (int)(*object_it).get_type() << "]" << std::endl;
            (*object_it).show(adjust_y, adjust_x); // TODO: must pass scroll map to objects somwhow...
        }
        n++;
    }
}

void MapController::show_above_objects(int adjust_y, int adjust_x)
{
    std::vector<GameObject>::iterator object_it;
    for (object_it = object_list.begin(); object_it != object_list.end(); object_it++) {
        if ((*object_it).get_type() == OBJ_STAGE_BOSS_TELEPORTER || (*object_it).get_type() == OBJ_BOSS_TELEPORTER || (*object_it).get_type() == OBJ_FINAL_BOSS_TELEPORTER || (*object_it).get_type() == OBJ_BOSS_DOOR || (*object_it).get_type() == OBJ_DOOR_AREA_LINK || (*object_it).get_type() == OBJ_DOOR_LOCKED) { // teleporters are shown above
            (*object_it).show(adjust_y, adjust_x); // TODO: must pass scroll map to objects somwhow...
        }
    }
}

bool MapController::boss_hit_ground(GameEnemy* npc_ref)
{
    if (npc_ref->is_boss() == true && npc_ref->is_on_visible_screen() == true) {
        //std::cout << "MAP::boss_hit_ground - move boss to ground - pos.y: " << npc_ref->getPosition().y << std::endl;

        int limit_y = npc_ref->get_start_position().y - TILESIZE;

        //std::cout << "#### limit_y [" << limit_y << "]" << ", start.y[" << npc_ref->get_start_position().y << "]" << std::endl;


        if (limit_y > RES_H/2) {
            limit_y = RES_H/2;
        }
        if (npc_ref->get_can_fly()) {
            limit_y = RES_H/2 - npc_ref->get_size().height/2;
            //std::cout << "#### [FLY] y[" << npc_ref->getPosition().y << "], limit_y [" << limit_y << "]" << ", h/2[" << (npc_ref->get_size().height/2) << "]" << std::endl;
        }

        if (npc_ref->getPosition().y >= limit_y) {
            // flying boss can stop on middle of the screen
            if (npc_ref->get_can_fly() == true) {
                //std::cout << "BOSS-HIT-GROUND <<<<<<<<<<<<<<<<<<<<" << std::endl;
                npc_ref->set_animation_type(ANIM_TYPE_WALK_AIR);
                return true;
            // non-flying bosses need to hit gound to stop
            } else if (npc_ref->hit_ground() == true) {
                npc_ref->set_animation_type(ANIM_TYPE_STAND);
                return true;
            }
        }
    }
    return false;
}

GameEnemy *MapController::get_near_boss()
{
    std::vector<GameEnemy>::iterator npc_it;
    for (npc_it = map_enemy_list.begin(); npc_it != map_enemy_list.end(); npc_it++) {
        GameEnemy* npc_ref = &(*npc_it);
        if (npc_ref->is_boss() == true && npc_ref->is_on_visible_screen() == true) {
            return npc_ref;
        }
    }
    return nullptr;
}

void MapController::reset_map_npcs()
{
    load_map_npcs();
}



