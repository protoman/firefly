#include <iostream>
#include <stdio.h>
#include "editorarea.h"
#include <QApplication>
#include <QResource>
#include <QBrush>
#include <QDir>


#include "defines.h"


#include "../file/format.h"
#include "../file/file_io.h"

#include "data/shareddata.h"

// construtor
EditorArea::EditorArea(QWidget *parent) : QWidget(parent) {
	mouse_released = true;
    myParent = parent;
    temp = 0;
    editor_selectedTileX = 0;
    editor_selectedTileY = 0;
	tempX = -1;
	tempY = -1;
    editor_selected_object_pos = 0;
    editor_selected_object_pos_map = 0;
    selection_started = false;
    selection_start_x = 0;
    selection_start_y = 0;
    selection_current_x = 0;
    selection_current_y = 0;


    update_files();

    this->show();
}

void EditorArea::changeTile() {
    printf("DEBUG.changeTile in editor area\n");
    update_files();
    temp = 1;
    repaint();
}


void EditorArea::update_files()
{
    std::string filename_str = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/easymode.png";
    easy_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/hardmode.png";
    hard_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/tilesets/") + Mediator::get_instance()->getPallete();
    if (filename_str.length() <= 0) {
        return;
    }
    //std::cout << "EditorArea::paintEvent - filename: " << filename.toStdString() << std::endl;
    tileset_image = QPixmap(filename_str.c_str());
    if (tileset_image.isNull() != false) {
        tileset_bitmap.clear();
    }

    /*
    std::string bg1_filename = FILEPATH +"/images/map_backgrounds/"+ Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][SharedData::get_instance()->file_v5_selected_map].backgrounds[0].filename;
    if (bg1_filename.length() > 0) {
        if (bg1_filename.find(".png") != std::string::npos) {
            bg1_image = QPixmap(bg1_filename.c_str());
        } else {
            bg1_image = QPixmap();
        }
    } else {
        std::cout << "RESET BG1" << std::endl;
        bg1_image = QPixmap();
    }

    std::string fg_filename = FILEPATH +"/images/map_backgrounds/"+ Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][SharedData::get_instance()->file_v5_selected_map].backgrounds[1].filename;
    if (fg_filename.length() > 0) {
        if (fg_filename.find(".png") != std::string::npos) {
            fg_layer__image = QPixmap(fg_filename.c_str());
        } else {
            fg_layer__image = QPixmap();
        }
    } else {
        std::cout << "RESET BG1" << std::endl;
        fg_layer__image = QPixmap();
    }
    fg_opacity = (float)Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][SharedData::get_instance()->file_v5_selected_map].backgrounds[1].gfx/100;
    */

    preload_slope_images();

}

void EditorArea::preload_slope_images()
{
    slope_image_list.clear();
    for (int i=0; i<SharedData::get_instance()->slope_list.size(); i++) {
        file_v5_slope_tile* slope_data = &SharedData::get_instance()->slope_list.at(i);
        std::string full_filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/slope/" + slope_data->filename;

        QPixmap image(QString(full_filename.c_str()));
        if (image.isNull()) {
            continue;
        }
        slope_image_list.insert(std::pair<int, QPixmap>(i, image));
    }
}

void EditorArea::draw_slope_tile(int x, int y, int dest_x, int dest_y, QPainter *painter)
{
    std::cout << "########### EditorArea::draw_slope_tile" << std::endl;
    if (slope_image_list.find(x) == slope_image_list.end()) {
        std::cout << "slope x[" << x << "] not found" << std::endl;
        return;
    }
    QRectF source(QPoint(y*TILESIZE, 0), QSize(TILESIZE, TILESIZE));
    QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
    std::cout << "DRAW-SLOPE AT x[" << target.x() << "], y[" << target.y() << "]" << std::endl;
    painter->drawPixmap(target, slope_image_list.at(x), source);
}


void EditorArea::drawTileset(QPainter *painter)
{
    // regular tiles
    if (SharedData::get_instance()->file_v5_map_tile_map.find(SharedData::get_instance()->file_v5_selected_map) != SharedData::get_instance()->file_v5_map_tile_map.end()) {
        int map_w = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_w;
        int map_h = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_h;
        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                int n = j*map_w + i;

                if (SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).size() > 0) {
                    file_v5_map_tile tileItem = SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n);

                    //std::cout << "TILE AT [" << i << "][" << j << "], type[" << tileItem.tile_underlay.type << "]" << std::endl;

                    if (tileItem.tile_underlay.x >= 0 && tileItem.tile_underlay.y >= 0) {
                        QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                        QRectF source(QPoint((tileItem.tile_underlay.x*TILESIZE), (tileItem.tile_underlay.y*TILESIZE)), QSize(TILESIZE, TILESIZE));
                        // used images depends upon tile type

                        //std::cout << "TILE AT [" << i << "][" << j << "], type[" << tileItem.tile_underlay.type << "]" << std::endl;

                        if (tileItem.tile_underlay.type == TILE_TYPE_SOLID) {
                            //std::cout << "FOUND SOLID_TILE AT [" << i << "][" << j << "]" << std::endl;
                            painter->drawPixmap(target, tileset_image, source);
                        } else if (tileItem.tile_underlay.type == TILE_TYPE_SLOPE) {
                            /// @TODO ///
                            //std::cout << "FOUND SLOPE_TILE AT [" << i << "][" << j << "], with x[" << tileItem.tile_underlay.x << "], y[" << tileItem.tile_underlay.y << "]" << std::endl;
                            draw_slope_tile(tileItem.tile_underlay.x, tileItem.tile_underlay.y, i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom, painter);


                        } else if (tileItem.tile_underlay.type == TILE_TYPE_ANIM) {
                            int anim_tile_id = tileItem.tile_underlay.x;
                            //std::cout << "FOUND ANIM_TILE AT [" << i << "][" << j << "], with ID [" << anim_tile_id << "]" << std::endl;
                            if (Mediator::get_instance()->anim_block_list.size() > 0 && anim_tile_id < Mediator::get_instance()->anim_block_list.size()) {
                                file_anim_block anim_tile = Mediator::get_instance()->anim_block_list.at(anim_tile_id);
                                QString anim_tile_filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/anim/") + QString(anim_tile.filename);
                                QPixmap anim_image(anim_tile_filename);
                                if (anim_image.isNull() == false) {
                                    QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                                    QRectF source(QPoint(0, 0), QSize(TILESIZE, TILESIZE));
                                    painter->drawPixmap(target, anim_image, source);

                                    // @TODO (move to a function) - draw an green border border to indicate anim tile
                                    QPen pen(QColor(0, 200, 0), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
                                    painter->setPen(pen);
                                    if (Mediator::get_instance()->show_grid) {
                                        int anim_tile_x = i * TILESIZE * Mediator::get_instance()->zoom; // minus tilesize is because width starts in 1, not zero
                                        int anim_tile_y = j * TILESIZE *Mediator::get_instance()->zoom;
                                        painter->drawLine(anim_tile_x, anim_tile_y, anim_tile_x+(TILESIZE*Mediator::get_instance()->zoom), anim_tile_y);
                                        painter->drawLine(anim_tile_x, anim_tile_y, anim_tile_x, anim_tile_y+(TILESIZE*Mediator::get_instance()->zoom));
                                        painter->drawLine(anim_tile_x, anim_tile_y+(TILESIZE*Mediator::get_instance()->zoom), anim_tile_x+(TILESIZE*Mediator::get_instance()->zoom), anim_tile_y+(TILESIZE*Mediator::get_instance()->zoom));
                                        painter->drawLine(anim_tile_x+(TILESIZE*Mediator::get_instance()->zoom), anim_tile_y, anim_tile_x+(TILESIZE*Mediator::get_instance()->zoom), anim_tile_y+(TILESIZE*Mediator::get_instance()->zoom));
                                    }
                                } else {
                                    std::cout << ">>>>>>>> anim-file '" << anim_tile_filename.toStdString() << "' not found." << std::endl;
                                }
                            } else {
                                std::cout << "Invalid anim-tile-id: " << anim_tile_id << std::endl;
                            }



                        }

                    } else if (tileItem.tile_underlay.x == -2 && tileItem.tile_underlay.y == -2) {
                        //std::cout << "UNUSED TILE x[" << i << "], y[" << j << "], n[" << n << "]" << std::endl;
                        painter->setBrush(QColor(240, 240, 240, 255));
                        painter->drawRect(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    }
                }
            }
        }
    } else {
        std::cout << "EditorArea::drawTileset #99" << std::endl;
    }

}

void EditorArea::drawLockTileset(QPainter *painter)
{
    // regular tiles
    if (SharedData::get_instance()->file_v5_map_tile_map.find(SharedData::get_instance()->file_v5_selected_map) != SharedData::get_instance()->file_v5_map_tile_map.end()) {
        //SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).size()
        int map_w = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_w;
        int map_h = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_h;
        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                int n = j*map_w + i;
                if (SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).size() > 0) {
                    file_v5_map_tile tileItem = SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n);

                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(QColor(255, 0, 0, 255));
                    painter->drawRect(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    // terrain type icon
                    QString terrainIcon;
                    QResource::registerResource("resources/icons/icons.qrc");
                    if (tileItem.locked == TERRAIN_SOLID) {
                        terrainIcon = QString::fromUtf8(":/toolbar_icons/Lock"); // solid
                    } else if (tileItem.locked == TERRAIN_STAIR) {
                        terrainIcon = QString(":/toolbar_icons/stairs.png"); // stairs
                    } else if (tileItem.locked == TERRAIN_SPIKE) {
                        terrainIcon = QString(":/toolbar_icons/edit-delete.png"); // spikes
                    } else if (tileItem.locked == TERRAIN_WATER) {
                        terrainIcon = QString(":/toolbar_icons/flag-blue.png"); // water
                    } else if (tileItem.locked == TERRAIN_ICE) {
                        terrainIcon = QString(":/toolbar_icons/flag-green.png"); // ice
                    } else if (tileItem.locked == TERRAIN_MOVE_LEFT) {
                        terrainIcon = QString(":/toolbar_icons/arrow-left.png"); // move left
                    } else if (tileItem.locked == TERRAIN_MOVE_RIGHT) {
                        terrainIcon = QString(":/toolbar_icons/arrow-right.png");
                    } else if (tileItem.locked == TERRAIN_SAND) {
                        terrainIcon = QString(":/toolbar_icons/arrow-down.png");
                    } else if (tileItem.locked == TERRAIN_HSCROLL_LOCK) {
                        terrainIcon = QString(":/toolbar_icons/system-switch-user.png");
                    } else if (tileItem.locked == TERRAIN_VSCROLL_LOCK) {
                        terrainIcon = QString(":/toolbar_icons/v_scroll_lock.png");
                    } else if (tileItem.locked == TERRAIN_SLOPE) {
                        terrainIcon = QString(":/toolbar_icons/draw-triangle.png"); // diagonal left
                    } else if (tileItem.locked == -2) {
                        terrainIcon = QString(":/toolbar_icons/dialog-cancel.png"); // diagonal left
                    }
                    if (terrainIcon.length() > 0) {
                        QPixmap terrainImage(terrainIcon);
                        if (terrainImage.isNull()) {
                            printf("ERROR: EditorArea::paintEvent - terrainType - Could not load image file '%s'\n", qPrintable(terrainIcon));
                        } else {
                            terrainIcon.resize(TILESIZE);
                            painter->setOpacity(0.7);
                            QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                            QRectF source(QPoint(0, 0), QSize(terrainImage.width (), terrainImage.height ()));
                            painter->drawPixmap(target, terrainImage, source);
                        }
                    }
                    painter->setOpacity(1.0);



                }
            }
        }
    }
}

void EditorArea::drawMapEnemies(QPainter *painter)
{
    int currentMap = SharedData::get_instance()->file_v5_selected_map;

    if (SharedData::get_instance()->file_v5_map_npc_map.find(currentMap) == SharedData::get_instance()->file_v5_map_npc_map.end()) {
        return;
    }

    // DRAW ENEMIES BACKGROUNDS //
    if (Mediator::get_instance()->show_npcs_flag == true) {
        /// draw NPCs
        std::cout << "EditorArea::drawMapEnemies currentMap[" << currentMap << "], npc-size[" << SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).size() << "]" << std::endl;
        for (int i=0; i<SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).size(); i++) {
            file_v5_map_npc map_npc = SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i);
            //std::cout << "EditorArea::paintEvent #5.0.A [" << i << "]" << std::endl;
            int npc_id = map_npc.id_npc;
            if (npc_id >= Mediator::get_instance()->enemy_list.size() || npc_id < 0) {
                map_npc.id_npc = -1;
                continue;
            }
            std::string npc_bg_file(Mediator::get_instance()->enemy_list.at(npc_id).bg_graphic_filename);
            if (npc_bg_file.length() > 0) {
                std::string _bg_graphic_filename = SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/backgrounds/" + npc_bg_file;
                QPixmap bg_image(_bg_graphic_filename.c_str());

                // calculate total image size of background exists
                if (!bg_image.isNull()) {
                    int total_w = bg_image.width();
                    int total_h = bg_image.height();
                    QRectF bg_target(QPoint(map_npc.start_point.x*TILESIZE*Mediator::get_instance()->zoom, map_npc.start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(total_w*Mediator::get_instance()->zoom, total_h*Mediator::get_instance()->zoom));
                    QRectF bg_source(QRectF(QPoint(0, 0), QSize(bg_image.width(), bg_image.height())));
                    painter->drawPixmap(bg_target, bg_image, bg_source);
                }
            }

            // DRAW ENEMY IMAGE
            std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/" + Mediator::get_instance()->enemy_list.at(npc_id).graphic_filename;
            if (filename.length() <= 0) {
                continue;
            }
            QPixmap temp_image(filename.c_str());
            if (!temp_image.isNull()) {
                int total_w = Mediator::get_instance()->enemy_list.at(npc_id).frame_size.width*Mediator::get_instance()->zoom;
                int total_h = Mediator::get_instance()->enemy_list.at(npc_id).frame_size.height*Mediator::get_instance()->zoom;
                int sprite_adjust_x = Mediator::get_instance()->enemy_list.at(npc_id).sprites_pos_bg.x;
                int sprite_adjust_y = Mediator::get_instance()->enemy_list.at(npc_id).sprites_pos_bg.y;



                QRectF target(QPoint((map_npc.start_point.x*TILESIZE+sprite_adjust_x)*Mediator::get_instance()->zoom, (map_npc.start_point.y*TILESIZE+sprite_adjust_y)*Mediator::get_instance()->zoom), QSize(total_w, total_h));
                QRectF source;
                if (map_npc.direction != ANIM_DIRECTION_RIGHT || temp_image.height() <= Mediator::get_instance()->enemy_list.at(npc_id).frame_size.height) {
                    source = QRectF(QPoint(0, 0), QSize(Mediator::get_instance()->enemy_list.at(npc_id).frame_size.width, Mediator::get_instance()->enemy_list.at(npc_id).frame_size.height));
                } else {
                    source = QRectF(QPoint(0, Mediator::get_instance()->enemy_list.at(npc_id).frame_size.height), QSize(Mediator::get_instance()->enemy_list.at(npc_id).frame_size.width, Mediator::get_instance()->enemy_list.at(npc_id).frame_size.height));
                }

                // TODO //
                /*
                if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].boss.id_npc == npc_id) {
                    // translucid green rectangle
                    painter->setBrush(QColor(0, 255, 0, 180));
                    painter->drawRect(target);
                } else if (Mediator::get_instance()->enemy_list.at(npc_id).is_boss) {
                    // translucid orange rectangle
                    painter->setBrush(QColor(255, 128, 35, 200));
                    painter->drawRect(target);
                } else if (Mediator::get_instance()->enemy_list.at(npc_id).is_sub_boss) {
                    // translucid bright-orange rectangle
                    painter->setBrush(QColor(241, 188, 87, 200));
                    painter->drawRect(target);
                }
                if (Mediator::get_instance()->game_data.final_boss_id == npc_id) {
                    // golden border
                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(QColor(255, 215, 0, 255));
                    painter->drawRect(target);
                }
                */


                int direction = map_npc.direction;
                //std::cout << "direction: " << direction << ", ANIM_DIRECTION_LEFT: " << std::endl;
                if (direction == ANIM_DIRECTION_LEFT) {
                    QImage temp_img(filename.c_str());
                    QImage mirror_image = temp_img.copy(source.x(), source.y(), source.width(), source.height());
                    mirror_image = mirror_image.mirrored(true, false);
                    QPixmap mirror_image_pixmap = QPixmap().fromImage(mirror_image);
                    painter->drawPixmap(target, mirror_image_pixmap, source);
                } else {
                    painter->drawPixmap(target, temp_image, source);
                }
            }
        }
    }


}

void EditorArea::drawMapObjects(QPainter *painter)
{
    /// draw objects
    //std::cout << "################### START LOOP" << std::endl;
    unsigned int current_map = SharedData::get_instance()->file_v5_selected_map;
    if (SharedData::get_instance()->file_v5_map_object_map.find(current_map) == SharedData::get_instance()->file_v5_map_object_map.end()) {
        return;
    }
    for (int i=0; i<SharedData::get_instance()->file_v5_map_object_map.at(current_map).size(); i++) {
        int obj_id = (int)SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).id_object;

        //std::cout << "OBJ[" << i << "][" << Mediator::get_instance()->object_list.at(obj_id).name << "].stage[" << (int)SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).stage_id << "].map[" << (int)SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).map_id << "], currentStage[" << Mediator::get_instance()->currentStage << "], currentMap[" << SharedData::get_instance()->file_v5_selected_map << "]" << std::endl;


        if (obj_id != -1) {
            //if (obj_id == 20) { std::cout << "************************** paintEvent - draw_objects[" << i << "].id: " << obj_id << std::endl; }
            std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/objects/" + Mediator::get_instance()->object_list.at(obj_id).graphic_filename;
            QPixmap temp_image(filename.c_str());
            if (temp_image.isNull()) {
                std::cout << "****************** Could not load file '" << filename.c_str() << "'" << std::endl;
                painter->setBrush(QColor(255, 255, 255, 180));
                painter->drawRect(SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
            } else {
                int obj_type = Mediator::get_instance()->object_list.at(obj_id).type;
                int obj_direction = SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).direction;
                if (obj_type == OBJ_RAY_HORIZONTAL) {
                    int graphic_pos_x = 0;
                    int graphic_pos_y = Mediator::get_instance()->object_list.at(obj_id).size.height;
                    if (obj_direction == ANIM_DIRECTION_LEFT) {
                        graphic_pos_x = Mediator::get_instance()->object_list.at(obj_id).size.width - TILESIZE;
                        graphic_pos_y = 0;
                    }
                    //std::cout << "OBJ_RAY_HORIZONTAL - obj_direction: " << obj_direction << ", graphic_pos_x: " << graphic_pos_x << ", graphic_pos_y: " << graphic_pos_y << std::endl;
                    QRectF target(QPoint(SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(graphic_pos_x, graphic_pos_y), QSize(TILESIZE, Mediator::get_instance()->object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else if (obj_type == OBJ_RAY_VERTICAL) {
                    int graphic_pos_y = Mediator::get_instance()->object_list.at(obj_id).size.height - TILESIZE;
                    //std::cout << "OBJ_RAY_HORIZONTAL - graphic_pos_y: " << graphic_pos_y << std::endl;
                    QRectF target(QPoint(SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(0, graphic_pos_y), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else if (obj_type == OBJ_DEATHRAY_HORIZONTAL && obj_direction == ANIM_DIRECTION_LEFT) {
                    QRectF target(QPoint(SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom - (Mediator::get_instance()->object_list.at(obj_id).size.width-TILESIZE)*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(0, 0), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else {
                    QRectF target(QPoint(SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source;
                    if (obj_direction == ANIM_DIRECTION_RIGHT && temp_image.height() >= Mediator::get_instance()->object_list.at(obj_id).size.height) {
                        source = QRectF(QPoint(0, Mediator::get_instance()->object_list.at(obj_id).size.height), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                    } else {
                        source = QRectF(QPoint(0, 0), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                    }
                    painter->drawPixmap(target, temp_image, source);
                }
            }
            // draw object-teleporter origin
            if (SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).teleporter_data.map_dest != -1) {
                int obj_w = Mediator::get_instance()->object_list.at(obj_id).size.width;
                int obj_h = Mediator::get_instance()->object_list.at(obj_id).size.height;
                //std::cout << "OBJ.w: " << obj_w << ", obj_h: " << obj_h << std::endl;
                int dest_x = SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom + (obj_w/2)*Mediator::get_instance()->zoom - TILESIZE/2;
                int dest_y = SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom + (obj_h/2)*Mediator::get_instance()->zoom - TILESIZE/2;
                //std::cout << "DRAW OBJECT TELEPORTER ORIGIN[" << i << "] - map: " << (int)SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).map_dest  << ", x: " << SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.x << ", y: " << SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(i).start_point.y << std::endl;
                if (Mediator::get_instance()->object_list.at(obj_id).type == OBJ_FINAL_BOSS_TELEPORTER) {
                    painter->setBrush(QColor(160, 60, 60, 180));
                } else {
                    painter->setBrush(QColor(60, 160, 60, 180));
                }
                painter->drawEllipse(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                // cyan border
                painter->setBrush(Qt::NoBrush);
                painter->setPen(QColor(0, 255, 255, 255));
                painter->drawEllipse(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                painter->setPen(QColor(0, 0, 0, 255));
                painter->drawText(dest_x+3, dest_y+TILESIZE-3, QString::number(i));
            }
        }
    }

}


void EditorArea::paintEvent(QPaintEvent *)
{
    if (tileset_image.isNull()) {
        std::cout << "ERROR: EditorArea::paintEvent - Could not load palette image file." << std::endl;
        return;
    }


    int pos;
    QPainter painter(this);
    QLineF line;
    QString filename;

    unsigned int mapNumber = SharedData::get_instance()->file_v5_selected_map;
    if (SharedData::get_instance()->file_v5_map_header_list.size() > mapNumber) {

        file_v5_map_header& header = SharedData::get_instance()->file_v5_map_header_list.at(mapNumber);

        // @ TODO //
        // draw background-color
        if (Mediator::get_instance()->show_background_color == true) {
            painter.fillRect(QRectF(0.0, 0.0, header.tiles_w*TILESIZE*Mediator::get_instance()->zoom, header.tiles_h*TILESIZE*Mediator::get_instance()->zoom), QColor(header.background_color.r, header.background_color.g, header.background_color.b, 255));
        }
        // draw backgrounds
        if (Mediator::get_instance()->show_bg1 == true) {
            for (unsigned int i=0; i<BACKGROUND_LAYERS_MAX; i++) {
                std::string bg1_filename(SharedData::get_instance()->FILEPATH + std::string("/images/map_backgrounds/") + header.backgrounds[i].filename);
                if (bg1_filename.length() > 0) {
                    bg1_image = QPixmap(bg1_filename.c_str());
                    if (!bg1_image.isNull()) {
                        int max_repeat = ((header.tiles_w*TILESIZE)/bg1_image.width())*Mediator::get_instance()->zoom+1;
                        for (int k=0; k<max_repeat; k++) {
                            QRectF pos_source(QPoint(0, 0), QSize(bg1_image.width(), bg1_image.height()));
                            QRectF pos_dest(QPoint(k*bg1_image.width()*Mediator::get_instance()->zoom, header.backgrounds[i].adjust_y*Mediator::get_instance()->zoom), QSize(bg1_image.width()*Mediator::get_instance()->zoom, bg1_image.height()*Mediator::get_instance()->zoom));
                            painter.drawPixmap(pos_dest, bg1_image, pos_source);
                        }
                    }
                }
            }
        }
    }


    drawTileset(&painter);
    if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
        drawLockTileset(&painter);
    }
    drawMapEnemies(&painter);


    //std::cout << "=============" << std::endl;
    if (Mediator::get_instance()->show_grid) {
        // DRAW GRID //
        QPen pen(QColor(120, 120, 120), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        int currentMap = SharedData::get_instance()->file_v5_selected_map;

        int limit_h = SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_h+1;
        int limit_w = SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_w;
        if (limit_w < 0 || limit_w > 1000) {
            std::cout << "IGNORE BAD MAP-DATA w[" << limit_w << "]" << std::endl;
            return;
        }
        if (limit_h < 0 || limit_h > 1000) {
            std::cout << "IGNORE BAD MAP-DATA h[" << limit_h << "]" << std::endl;
            return;
        }

        for (unsigned int i=1; i<limit_w; i++) {
            pos = i*TILESIZE*Mediator::get_instance()->zoom-1;
            //QLineF line(0, 800, 16, 800);
            // linhas horizontais
            line = QLineF(pos, 0, pos, SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_h*TILESIZE*Mediator::get_instance()->zoom-1);
            if (i % 20 == 0) {
                painter.setPen(pen_red);
            } else {
                painter.setPen(pen);
            }
            painter.drawLine(line);
        }
        painter.setPen(pen);

        std::cout << ">>>>>>>>>>>> limit_h[" << limit_h << "]" << std::endl;
        for (unsigned int i=1; i<limit_h; i++) {
            pos = i*TILESIZE*Mediator::get_instance()->zoom-1;
            //QLineF line(0, 800, 16, 800);
            // linhas verticais
            line = QLineF(0, pos, SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_w*TILESIZE*Mediator::get_instance()->zoom-1, pos);
            painter.drawLine(line);
        }
    }


    // DRAW LINKS //
    if (Mediator::get_instance()->show_teleporters_flag == true) {
        // draw links
        //printf("editoMode: %d, EDITMODE_NORMAL: %d, editTool: %d, EDITMODE_LINK_DEST: %d\n", Mediator::get_instance()->editMode, EDITMODE_NORMAL, Mediator::get_instance()->editTool, EDITMODE_LINK_DEST);
        if (Mediator::get_instance()->editMode == EDITMODE_LINK && (Mediator::get_instance()->editTool == EDITMODE_LINK_DEST || Mediator::get_instance()->editTool == EDITMODE_LINK)) {
            for (int i=0; i<SharedData::get_instance()->file_v5_map_link_list.size(); i++) {
                file_v5_map_link* link_data = &SharedData::get_instance()->file_v5_map_link_list.at(i);
                // link is from and to the same map, draw in a different color

                std::cout << "currentMap[" << SharedData::get_instance()->file_v5_selected_map << "], origin[" << (int)link_data->id_map_origin << "], dest[" << (int)link_data->id_map_destiny << "]" << std::endl;

                if (SharedData::get_instance()->file_v5_selected_map == link_data->id_map_origin) {
                    // translucid blue rectangle
                    if (link_data->type == LINK_TELEPORTER || link_data->type == LINK_TELEPORT_LEFT_LOCK || link_data->type == LINK_TELEPORT_RIGHT_LOCK || link_data->type == LINK_FADE_TELEPORT) {
                        painter.setBrush(QColor(0, 255, 0, 180));
                    } else {
                        // if origin and destiny are the same map, use yellow
                        if (link_data->id_map_origin == link_data->id_map_destiny) {
                            painter.setBrush(QColor(231, 209, 58, 180));
                        } else {
                            painter.setBrush(QColor(0, 0, 255, 180));
                        }
                    }

                    //std::cout << "DRAW-LINK - zoom[" << Mediator::get_instance()->zoom << ", TILESIZE[" << TILESIZE << ", w[" << link_data->pos_origin.w << "], h[" << link_data->pos_origin.h << "]" << std::endl;

                    painter.drawRect(link_data->pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, link_data->pos_origin.y *TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.w, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.h);
                    // blue border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 0, 255, 255));
                    painter.drawRect(link_data->pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, link_data->pos_origin.y*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.w, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.h);
                    painter.setPen(QColor(255, 255, 255, 255));
                    painter.drawText(link_data->pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, (link_data->pos_origin.y+1)*TILESIZE*Mediator::get_instance()->zoom, QString::number(i));
                }
                if (SharedData::get_instance()->file_v5_selected_map == link_data->id_map_destiny) {
                    // translucid cyan rectangle
                    if (link_data->type == LINK_TELEPORTER || link_data->type == LINK_TELEPORT_LEFT_LOCK || link_data->type == LINK_TELEPORT_RIGHT_LOCK || link_data->type == LINK_FADE_TELEPORT) {
                        painter.setBrush(QColor(60, 160, 60, 180));
                    } else {
                        // if origin and destiny are the same map, use yellow
                        if (link_data->id_map_origin == link_data->id_map_destiny) {
                            painter.setBrush(QColor(184, 171, 84, 180));
                        } else {
                            painter.setBrush(QColor(0, 255, 255, 180));
                        }
                    }
                    painter.drawRect(link_data->pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, link_data->pos_destiny.y*TILESIZE*Mediator::get_instance()->zoom,Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.w, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.h);
                    // cyan border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 255, 255, 255));
                    painter.drawRect(link_data->pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, link_data->pos_destiny.y*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.w, Mediator::get_instance()->zoom*TILESIZE*link_data->pos_origin.h);
                    painter.setPen(QColor(0, 0, 0, 255));
                    painter.drawText(link_data->pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, (link_data->pos_destiny.y+1)*TILESIZE*Mediator::get_instance()->zoom, QString::number(i));
                }
            }
        }
    }


    // DRAW OBJECTS //

    if (Mediator::get_instance()->show_objects_flag == true) {
        drawMapObjects(&painter);

    }

        /*

        // DRAW OBJECT TELEPORT CIRCLES
        for (int k=0; k<FS_STAGE_MAX_MAPS; k++) {
            for (int m=0; m<Mediator::get_instance()->maps_data_object_list.size(); m++) {
                if (Mediator::get_instance()->maps_data_object_list[m].stage_id != Mediator::get_instance()->currentStage) {
                    continue; // only show enemies from current stage/map
                }
                file_map_object_v2 map_obj = Mediator::get_instance()->maps_data_object_list[m];
                int obj_id = map_obj.id_object;
                if (obj_id == -1 || obj_id >= Mediator::get_instance()->object_list.size()) { // old format style or invalid object
                    continue;
                }

                // draw teleport destiny links
                //std::cout << "OBJ[" << i << "].map_dest: " << (int)map_obj.map_dest << ", currentMap: " << k << std::endl;
                if (map_obj.map_dest == SharedData::get_instance()->file_v5_selected_map) {

                    //std::cout << "## EDITORAREA::paintEvent - teleporter_obj - x: " << (int)map_obj.link_dest.x << ", y: " << (int)map_obj.link_dest.y << std::endl;

                    if (Mediator::get_instance()->object_list.at(obj_id).type == OBJ_FINAL_BOSS_TELEPORTER) {
                        painter.setBrush(QColor(160, 60, 60, 180));
                    } else {
                        painter.setBrush(QColor(60, 160, 60, 180));
                    }

                    painter.drawEllipse(map_obj.link_dest.x*TILESIZE*Mediator::get_instance()->zoom, map_obj.link_dest.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    // cyan border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 255, 255, 255));

                    painter.drawEllipse(map_obj.link_dest.x*TILESIZE*Mediator::get_instance()->zoom, map_obj.link_dest.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);

                    painter.setPen(QColor(0, 0, 0, 255));
                    painter.drawText(map_obj.link_dest.x*TILESIZE*Mediator::get_instance()->zoom + 3*Mediator::get_instance()->zoom, (map_obj.link_dest.y+1)*TILESIZE*Mediator::get_instance()->zoom -2*Mediator::get_instance()->zoom, QString::number(m));
                }
            }
        }
    }

    // === FOREGROUND LAYER IMAGE == //
    if (Mediator::get_instance()->show_fg_layer == true) {
        if (!fg_layer__image.isNull()) {
            //std::cout << "DRAW BG1" << std::endl;
            int max_repeat = ((MAP_W*TILESIZE)/fg_layer__image.width())*Mediator::get_instance()->zoom+1;
            //std::cout << "fg_layer__image.width(): " << fg_layer__image.width() << ", max_repeat: " << max_repeat << std::endl;
            for (int k=0; k<max_repeat; k++) {
                QRectF pos_source(QPoint(0, 0), QSize(fg_layer__image.width(), fg_layer__image.height()));
                QRectF pos_dest(QPoint(k*fg_layer__image.width()*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][SharedData::get_instance()->file_v5_selected_map].backgrounds[1].adjust_y*Mediator::get_instance()->zoom), QSize(fg_layer__image.width()*Mediator::get_instance()->zoom, fg_layer__image.height()*Mediator::get_instance()->zoom));
                painter.setOpacity(fg_opacity);
                painter.drawPixmap(pos_dest, fg_layer__image, pos_source);
                painter.setOpacity(1);
            }
        }
    }
    */


    // === draw selection === //
    if (Mediator::get_instance()->editMode == EDITMODE_SELECT) {
        std::cout << "PAINT::EDITMODE_SELECT" << std::endl;
        painter.setBrush(QColor(0, 0, 255, 180));
        painter.drawRect(selection_start_x*TILESIZE*Mediator::get_instance()->zoom, selection_start_y*TILESIZE*Mediator::get_instance()->zoom, abs(selection_current_x-selection_start_x)*TILESIZE*Mediator::get_instance()->zoom, abs(selection_current_y-selection_start_y)*TILESIZE*Mediator::get_instance()->zoom);
    }


    QSize resizeMe(SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_w*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_w*TILESIZE*Mediator::get_instance()->zoom);
    this->resize(resizeMe);
    myParent->adjustSize();

}

void EditorArea::mouseMoveEvent(QMouseEvent *event) {
	QPoint pnt = event->pos();

    if (Mediator::get_instance()->editMode != EDITMODE_SELECT) {
        // forces "click" when moving
        int pointX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom);
        int pointY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom);
        if (editor_selectedTileX != pointX || editor_selectedTileY != pointY) {
            std::cout << ">>>>>>>>> EditorArea::mouseMoveEvent::CLICK, editor.x[" << editor_selectedTileX << "], editor.y[" << editor_selectedTileY << "], pointX[" << pointX << "], pointY[" << pointY << "]" << std::endl;
            mousePressEvent(event);
        }
    } else {
        selection_current_x = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom) + 1;
        selection_current_y = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom) + 1;
        repaint();
    }
}

void EditorArea::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if (numSteps > 0) {
        if (Mediator::get_instance()->zoom < 3.0) {
            if (Mediator::get_instance()->zoom < 1.0) {
                Mediator::get_instance()->zoom += 0.25;
            } else {
                Mediator::get_instance()->zoom++;
            }
            std::cout << "### INC-SET ZOOM TO[" << Mediator::get_instance()->zoom << "]" << std::endl;
            repaint();
        }
    } else if (numSteps < 0) {
        if (Mediator::get_instance()->zoom > 1) {
            Mediator::get_instance()->zoom -= 1.0;
        } else if (Mediator::get_instance()->zoom == 1.0) {
            Mediator::get_instance()->zoom = 0.5;
        } else if (Mediator::get_instance()->zoom == 0.5) {
            Mediator::get_instance()->zoom = 0.25;
        }
        std::cout << "### DEC-SET ZOOM TO[" << Mediator::get_instance()->zoom << "]" << std::endl;
        repaint();
    }
    event->accept();
}



void EditorArea::mousePressEvent(QMouseEvent *event) {
    if (mouse_released == false && (Mediator::get_instance()->editTool == EDITMODE_LINK || Mediator::get_instance()->editTool == EDITMODE_LINK_DEST || Mediator::get_instance()->editMode == EDITMODE_NPC || Mediator::get_instance()->editMode == EDITMODE_OBJECT || Mediator::get_instance()->editMode == EDITMODE_SET_BOSS || Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE)) {
        std::cout << "EDITORAREA::mousePressEvent - IGNORED" << std::endl;
		return;
	}

    std::cout << "EDITORAREA::mousePressEvent - EXECUTE, mode[" << Mediator::get_instance()->editTool << "]" << std::endl;
    std::cout << "SET-SLOPE-MODE [" << Mediator::get_instance()->editMode << "]" << std::endl;

    QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom);
    editor_selectedTileY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom);


    if (Mediator::get_instance()->editMode == EDITMODE_NORMAL || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE || Mediator::get_instance()->editMode == EDITMODE_SLOPE) {
        // V5-FILE //
        if (SharedData::get_instance()->file_v5_map_tile_map.find(SharedData::get_instance()->file_v5_selected_map) != SharedData::get_instance()->file_v5_map_tile_map.end()) {

            int map_w = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_w;
            int map_h = SharedData::get_instance()->file_v5_map_header_list.at(SharedData::get_instance()->file_v5_selected_map).tiles_h;

            if (editor_selectedTileX > map_w || editor_selectedTileY > map_h) {
                return;
            }

            int n = editor_selectedTileY*map_w + editor_selectedTileX;
            std::cout << "mousePressEvent.DEBUG#1 - map_w[" << map_w << "], map_h[" << map_h << "], n[" << n << "]" << std::endl;

            // out of map area
            if (n >= SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).size()) {
                return;
            }

            // unused tile
            if (SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_underlay.x == -2 && SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_underlay.y == -2) {
                std::cout << ">>>>>>>>>>> UNUSED TILE <<<<<<<<<<<<<" << std::endl;
                return;
            }

            if (Mediator::get_instance()->editTool == EDITMODE_NORMAL || Mediator::get_instance()->editTool == EDITMODE_ERASER || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE || Mediator::get_instance()->editMode == EDITMODE_SLOPE) {
                std::cout << "mousePressEvent.DEBUG#2" << std::endl;
                int valueType = TILE_TYPE_SOLID;
                int valueX = Mediator::get_instance()->getPalleteX();
                int valueY = Mediator::get_instance()->getPalleteY();

                if (Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE) {
                    valueX = Mediator::get_instance()->selectedAnimTileset;
                    valueY = 0;
                    valueType = TILE_TYPE_ANIM;
                } else if (Mediator::get_instance()->layerLevel == 1 && Mediator::get_instance()->editMode == EDITMODE_SLOPE) {
                    valueType = TILE_TYPE_SLOPE;
                    std::cout << "PLACE-SLOPE #1" << std::endl;
                }
                if (Mediator::get_instance()->editTool == EDITMODE_ERASER) {
                    valueX = -1;
                    valueY = -1;
                }

                if (SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).size() > 0) {

                    if (Mediator::get_instance()->layerLevel == 1) {
                        std::cout << "############ set tile[" << n << "], at[" << editor_selectedTileX << "][" << editor_selectedTileY << "] with [" << valueX << "][" << valueY << "][" << valueType << "]" << std::endl;
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_underlay.x = valueX;
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_underlay.y = valueY;
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_underlay.type = valueType;
                    } else if (Mediator::get_instance()->layerLevel == 3) {
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_overlay.x = valueX;
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_overlay.y = valueY;
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).tile_overlay.type = valueType;
                    }
                    if (Mediator::get_instance()->layerLevel == 1 && Mediator::get_instance()->editMode == EDITMODE_SLOPE) {
                        SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).locked = TERRAIN_SLOPE;
                        std::cout << "PLACE-SLOPE #2" << std::endl;
                    }
                }
            } else if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
                if (SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).locked == Mediator::get_instance()->terrainType) {
                    SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).locked = TERRAIN_UNBLOCKED;
                } else {
                    SharedData::get_instance()->file_v5_map_tile_map.at(SharedData::get_instance()->file_v5_selected_map).at(n).locked = Mediator::get_instance()->terrainType;
                }
            } else {
                std::cout << "mousePressEvent.DEBUG -- ELSE#1" << std::endl;
            }
        } else {
            std::cout << "mousePressEvent.DEBUG -- ELSE#2" << std::endl;
        }




    } else if (Mediator::get_instance()->editMode == EDITMODE_LINK) {
        if (Mediator::get_instance()->editTool == EDITMODE_LINK && tempX == -1) {
                    // checks if a link in this position already exits to remove it
                    for (int i=0; i<SharedData::get_instance()->file_v5_map_link_list.size(); i++) {
                        file_v5_map_link* link_data = &SharedData::get_instance()->file_v5_map_link_list.at(i);
                        if (link_data->id_map_origin == SharedData::get_instance()->file_v5_selected_map) {
                            if (link_data->pos_origin.x == editor_selectedTileX && link_data->pos_origin.y == editor_selectedTileY) {
                                SharedData::get_instance()->file_v5_map_link_list.erase(SharedData::get_instance()->file_v5_map_link_list.begin()+i);
                                repaint();
                                return;
                            }
                        } else if (link_data->id_map_destiny == SharedData::get_instance()->file_v5_selected_map) {
                            if (link_data->pos_destiny.x == editor_selectedTileX && link_data->pos_destiny.y == editor_selectedTileY) {
                                SharedData::get_instance()->file_v5_map_link_list.erase(SharedData::get_instance()->file_v5_map_link_list.begin()+i);
                                repaint();
                                return;
                            }
                        }
                    }
                    tempX = editor_selectedTileX;
                    tempY = editor_selectedTileY;
                } else if (Mediator::get_instance()->editTool == EDITMODE_LINK_DEST) {

                    SharedData::get_instance()->file_v5_map_link_list.push_back(file_v5_map_link());
                    file_v5_map_link* link_data = &SharedData::get_instance()->file_v5_map_link_list.at(SharedData::get_instance()->file_v5_map_link_list.size()-1);

                    std::cout << "########### -> editorArea::mousePress - adding link DESTINY - PART 3 - place link" << std::endl;
                    link_data->id_map_origin = link_map_origin;
                    link_data->pos_origin.x = link_pos_x;
                    link_data->pos_origin.y = link_pos_y;




                    if (link_direction == LINK_DIRECTION_HORIZONTAL) {
                        link_data->pos_origin.w = link_size;
                        link_data->pos_origin.h = 1;
                    } else {
                        link_data->pos_origin.w = 1;
                        link_data->pos_origin.h = link_size;
                    }

                    link_data->id_map_destiny = SharedData::get_instance()->file_v5_selected_map;
                    link_data->pos_destiny.x = editor_selectedTileX;
                    link_data->pos_destiny.y = editor_selectedTileY;
                    link_data->link_direction = link_direction;

                    Mediator::get_instance()->editTool = EDITMODE_LINK;
                    repaint();
                    QApplication::setOverrideCursor(Qt::ArrowCursor);
                    return;
                }




    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_BOSS) {
    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS) {
    } else if (Mediator::get_instance()->editMode == EDITMODE_NPC) {
		printf(">> EditorArea::mousePressEvent - EDITMODE_NPC\n");

		int found_npc = -1;
        int currentMap = SharedData::get_instance()->file_v5_selected_map;
        if (SharedData::get_instance()->file_v5_map_npc_map.find(currentMap) == SharedData::get_instance()->file_v5_map_npc_map.end()) {
            SharedData::get_instance()->file_v5_map_npc_map.insert(std::pair<int, std::vector<file_v5_map_npc>>(currentMap, std::vector<file_v5_map_npc>()));
        }

		// search if there is an existing NPC in ths position, and if yes, remove it
        for (int i=0; i<SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).size(); i++) {
            if (SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).id_npc != -1 && SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.x == editor_selectedTileX && SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.y == editor_selectedTileY) {
				found_npc = i;
				break;
			}
		}

        if (Mediator::get_instance()->editTool == EDITMODE_ERASER && found_npc != -1) {
			std::cout << "remove npc - slot: " << found_npc << std::endl;
            SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(found_npc).id_npc = -1;
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_npc == -1 && Mediator::get_instance()->selectedNPC != -1) {
            file_v5_map_npc new_npc;
            new_npc.id_npc = Mediator::get_instance()->selectedNPC;
            new_npc.start_point.x = editor_selectedTileX;
            new_npc.start_point.y = editor_selectedTileY;
            new_npc.direction = Mediator::get_instance()->npc_direction;
            SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).push_back(new_npc);
            std::cout << "EditorArea::mousePressEvent - ADDED NPC in map[" << currentMap << "], pos[" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;
            repaint();
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_npc != -1 && Mediator::get_instance()->selectedNPC != -1) {
			printf(">> EditorArea::mousePressEvent - Adding NPC - place already taken\n");
		}



    } else if (Mediator::get_instance()->editMode == EDITMODE_OBJECT) {
        std::cout << ">> EditorArea::mousePressEvent - EDITMODE_OBJECT" << std::endl;

        std::cout << ">> EditorArea::mousePressEvent - EDITMODE_OBJECT" << std::endl;
        int found_object = -1;

        // search if there is an existing object in ths position, and if yes, remove it
        unsigned int current_map = SharedData::get_instance()->file_v5_selected_map;
        if (SharedData::get_instance()->file_v5_map_object_map.find(current_map) == SharedData::get_instance()->file_v5_map_object_map.end()) {
            SharedData::get_instance()->file_v5_map_object_map.insert(std::pair<unsigned int, std::vector<file_v5_map_object>>(current_map, std::vector<file_v5_map_object>()));
        } else {
            for (int m=0; m<SharedData::get_instance()->file_v5_map_object_map.at(current_map).size(); m++) {
                if (SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(m).id_object != -1 && SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(m).start_point.x == editor_selectedTileX && SharedData::get_instance()->file_v5_map_object_map.at(current_map).at(m).start_point.y == editor_selectedTileY) {
                    found_object = m;
                    break;
                }
            }
        }

        if (Mediator::get_instance()->editTool == EDITMODE_ERASER && found_object != -1) {
            std::cout << "remove object - slot: " << found_object << std::endl;
            SharedData::get_instance()->file_v5_map_object_map.at(current_map).erase(SharedData::get_instance()->file_v5_map_object_map.at(current_map).begin()+found_object);
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_object == -1 && Mediator::get_instance()->selectedNPC != -1) {
                file_v5_map_object new_obj;
                new_obj.id_object = Mediator::get_instance()->selectedNPC;
                new_obj.start_point.x = editor_selectedTileX;
                new_obj.start_point.y = editor_selectedTileY;
                new_obj.direction = Mediator::get_instance()->object_direction;
                // se item é teleportador, deve entrar no modo de colocar link de object
                int obj_type = Mediator::get_instance()->object_list.at(Mediator::get_instance()->selectedNPC).type;
                if (obj_type == OBJ_BOSS_TELEPORTER || obj_type == OBJ_FINAL_BOSS_TELEPORTER || obj_type == OBJ_PLATFORM_TELEPORTER || obj_type == OBJ_STAGE_BOSS_TELEPORTER) {
                    editor_selected_object_pos = SharedData::get_instance()->file_v5_map_object_map.size();
                    std::cout << "SET editor_selected_object_pos: " << editor_selected_object_pos << std::endl;
                    Mediator::get_instance()->editTool = EDITMODE_OBJECT_LINK_PLACING;
                    QApplication::setOverrideCursor(Qt::CrossCursor);
                    // @TODO - desabilita todos os modos, não pode sair no meio de object-link-placing
                    // @TODO - desabilitar mudar de estágio também (e isso tem que ser feito no link tb)
                }
                SharedData::get_instance()->file_v5_map_object_map.at(current_map).push_back(new_obj);
                repaint();
        }



    } else if (Mediator::get_instance()->editMode == EDITMODE_SELECT) {
        std::cout << "EDITMODE_SELECT - selection_started: " << selection_started << std::endl;
        if (selection_started == false) {
            std::cout << "Start Selection." << std::endl;
            selection_started = true;
            selection_start_x = editor_selectedTileX;
            selection_start_y = editor_selectedTileY;
        } else {
            std::cout << "selection already started, ignore mousePress..." << std::endl;
        }


    } else if (Mediator::get_instance()->editMode == EDITMODE_PASTE) {
    }
    temp = 1;
    repaint();
}


void EditorArea::mouseReleaseEvent(QMouseEvent *event) {
	QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom) + 1;
    editor_selectedTileY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom) + 1;



    if (Mediator::get_instance()->editMode == EDITMODE_SELECT) {
        // copies points in the selection to the selection matrix


        selection_started = false;
        if (selection_start_x == editor_selectedTileX || selection_start_y == editor_selectedTileY) {
            mouse_released = true;
            return;
        }
        int start_x = selection_start_x;
        int end_x = editor_selectedTileX;
        if (selection_start_x > editor_selectedTileX) {
            start_x = editor_selectedTileX;
            end_x = selection_start_x;
        }
        int start_y = selection_start_y;
        int end_y = editor_selectedTileY;
        if (selection_start_y > editor_selectedTileY) {
            start_y = editor_selectedTileY;
            end_y = selection_start_y;
        }
        selection_matrix.clear();

        std::cout << "### EDITMODE_SELECT - start_x: " << start_x << ", end_x: " << end_x << ", start_y: " << start_y << ", end_y: " << end_y << std::endl;

    } else if (Mediator::get_instance()->editTool == EDITMODE_LINK && tempX != -1) {// && Mediator::get_instance()->link_type != LINK_FINAL_BOSS_ROOM) {
        std::cout << "########### -> editorArea::mouseReleaseEvent - adding link - PART 2" << std::endl;
		// TODO: add link

        link_map_origin = SharedData::get_instance()->file_v5_selected_map;
		link_pos_x = tempX;
		link_pos_y = tempY;


        int link_size_x = abs(editor_selectedTileX - tempX);
        int link_size_y = abs(editor_selectedTileY - tempY);
        if (link_size_x != 1 && link_size_y != 1) {
            std::cout << "Invalid selection #1, w[" << link_size_x << "], h[" << link_size_y << "]" << std::endl;
            return;
        }
        if (link_size_x == 1 && link_size_y == 1) {
            std::cout << "Invalid selection #2, w[" << link_size_x << "], h[" << link_size_y << "]" << std::endl;
            return;
        }



        Mediator::get_instance()->editTool = EDITMODE_LINK_DEST;
        QApplication::setOverrideCursor(Qt::CrossCursor);
        if (link_size_x != 1) {
            link_size = link_size_x;
            link_direction = LINK_DIRECTION_HORIZONTAL;
        } else if (link_size_y != 1) {
            link_size = link_size_y;
            link_direction = LINK_DIRECTION_VERTICAL;
        }

        std::cout << "link_size_x[" << link_size_x << "], link_size_y[" << link_size_y << "], link_size[" << link_size << "], link_direction[" << link_direction << "]" << std::endl;


        /*
		if (tempSize < 1 || tempSize > 20) {
			link_size = 1;
		} else {
            link_size = abs(editor_selectedTileX - tempX);
		}
        */
        //printf(">>>>>>>> added link in map: %d, pos: (%d, %d) with size: %d <<<<<<<<<<\n", link_map_origin, link_pos_x, link_pos_y, link_size);

		tempX = -1;
		tempY = -1;
		repaint();

    }
	mouse_released = true;
}









