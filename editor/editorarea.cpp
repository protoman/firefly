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

void EditorArea::update_area_data()
{
    update_map_data();
}


void EditorArea::update_files()
{
    if (SharedData::get_instance()->v6_area_list.size() <= SharedData::get_instance()->v6_selected_area) {
        return;
    }


    std::string filename_str = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/easymode.png";
    easy_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = SharedData::get_instance()->FILEPATH + "images/tilesets/blocks/hardmode.png";
    hard_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/tilesets/") + SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).tileset_filename;
    if (filename_str.length() == 0) {
        tileset_image = QPixmap();
    } else {
        tileset_image = QPixmap(filename_str.c_str());
    }

    for (int i=0; i<LAYERS_COUNT; i++) {
        std::string layer_filename = std::string(SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].filename);
        if (layer_filename.length() == 0) {
            layer_pixmap_list[i] = QPixmap();
        } else {
            std::string filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/map_backgrounds/") + SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].filename;
            filename_str = StringUtils::clean_filename(filename_str);
            //std::cout << ">>>>>>>> update_files.filename-bg[" << i << "]: [" << filename_str << "]" << std::endl;
            layer_pixmap_list[i] = QPixmap(filename_str.c_str());
        }
    }

    preload_slope_images();

}

void EditorArea::update_map_data()
{
    if (SharedData::get_instance()->v6_level_map.size() <= SharedData::get_instance()->v6_selected_level) {
        return;
    }
    if (SharedData::get_instance()->v6_area_list.size() <= SharedData::get_instance()->v6_selected_area) {
        return;
    }
    std::cout << "########### EditorArea::update_map_data - level[" << SharedData::get_instance()->v6_selected_level << "], area[" << SharedData::get_instance()->v6_selected_area << "]" << std::endl;

    topmost_point = 99999;
    bottommost_point = -1;
    leftmost_point = 99999;
    rightmost_point = -1;
    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {
        if (it->first.x > rightmost_point) {
            rightmost_point = it->first.x;
        }
        if (it->first.x < leftmost_point) {
            leftmost_point = it->first.x;
        }
        if (it->first.y > bottommost_point) {
            bottommost_point = it->first.y;
        }
        if (it->first.y < topmost_point) {
            topmost_point = it->first.y;
        }
    }
    total_editarea_w = (rightmost_point - leftmost_point) + 1; // plus 1 because we start the count in zero
    total_editarea_h = (bottommost_point - topmost_point) +1;

    std::cout << "total_editarea_w[" << total_editarea_w << "], rightmost_point[" << rightmost_point << "], leftmost_point[" << leftmost_point << "]";
    std::cout << ", total_editarea_h[" << total_editarea_h << "], bottommost_point[" << bottommost_point << "], topmost_point[" << topmost_point << "]" << std::endl;

    update_editarea_size();

}

void EditorArea::update_editarea_size()
{
    QSize resizeMe(total_editarea_w*AREA_ROOM_W*TILESIZE*Mediator::get_instance()->zoom, total_editarea_h*AREA_ROOM_H*TILESIZE*Mediator::get_instance()->zoom);
    this->resize(resizeMe);
    myParent->adjustSize();
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



    // draw backgrounds
    if (Mediator::get_instance()->show_bg1 == true) {
/*
        //std::cout << "############# EditorArea::paintEvent - LOOP" << std::endl;
        for (int room_y=topmost_room; room_y<=bottommost_room; room_y++) {
            for (int room_x=leftmost_room; room_x<=rightmost_room; room_x++) {
                //std::cout << "############# EditorArea::paintEvent - level[" << SharedData::get_instance()->v6_selected_level << "], room[" << room_x << "][" << room_y << "].area_n[" << SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].area_n << "], current_area_n[" << SharedData::get_instance()->v6_selected_area << "]" << std::endl;
                if (SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].area_n == SharedData::get_instance()->v6_selected_area) {
                    for (int i=0; i<LAYERS_COUNT; i++) {
                        if (!layer_pixmap_list[i].isNull()) {
                            int k = room_x-leftmost_room;
                            int l = room_y-topmost_room;
                            int bg_pos_y = AREA_H*l+ SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].adjust_y;
                            /// @TODO: repeat backgrounds in X and Y axis, if set in mode and smaller than RES_W and AREA_H //

                            int origin_h = layer_pixmap_list[i].height();
                            if ((layer_pixmap_list[i].height() + bg_pos_y) > AREA_H) {
                                origin_h -= AREA_H - (layer_pixmap_list[i].height() + bg_pos_y);
                            }

                            QRectF pos_source(QPoint(0, 0), QSize(layer_pixmap_list[i].width(), origin_h));
                            QRectF pos_dest(QPoint(k*RES_W*Mediator::get_instance()->zoom, bg_pos_y*Mediator::get_instance()->zoom), QSize(layer_pixmap_list[i].width()*Mediator::get_instance()->zoom, origin_h*Mediator::get_instance()->zoom));
                            //std::cout << "BG[" << i << "].total_h[" << (pos_dest.y() + pos_dest.height()) << "]" << std::endl;
                            painter.drawPixmap(pos_dest, layer_pixmap_list[i], pos_source);
                        }
                    }
                }
            }
        }
*/
    }





    //std::cout << "=============" << std::endl;
    if (Mediator::get_instance()->show_grid) {
        // DRAW GRID //
        QPen pen(QColor(120, 120, 120), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

        for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {


            int map_pos_x = (it->first.x-leftmost_point)*AREA_ROOM_W*TILESIZE*Mediator::get_instance()->zoom;
            int map_pos_y = (it->first.y-topmost_point)*AREA_ROOM_H*TILESIZE*Mediator::get_instance()->zoom;
            int map_size_w = AREA_ROOM_W*TILESIZE*Mediator::get_instance()->zoom;
            int map_size_h = AREA_ROOM_H*TILESIZE*Mediator::get_instance()->zoom;

            //std::cout << ">>>> room.x[" << it->first.x << "], room.y[" << it->first.y << "], map_pos_x[" << map_pos_x << "], map_pos_y[" << map_pos_y << "], zoom[" << Mediator::get_instance()->zoom << "]" << std::endl;

            if (Mediator::get_instance()->show_background_color == true) {
                st_color bg_color = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).background_color;
                QColor qbg_color = QColor(bg_color.r, bg_color.g, bg_color.b, 255);
                painter.fillRect(QRectF(map_pos_x, map_pos_y, map_size_w, map_size_h), qbg_color);
            }


            painter.setPen(pen);

            int limit_h = AREA_ROOM_H;
            int limit_w = AREA_ROOM_W;


            for (unsigned int i=1; i<limit_w+1; i++) { // linhas VERTICAIS
                pos = map_pos_x + i*TILESIZE*Mediator::get_instance()->zoom;
                line = QLineF(pos, map_pos_y, pos, map_pos_y+map_size_h);
                if (i == 0 || i % AREA_ROOM_W == 0) {
                    painter.setPen(pen_red);
                } else {
                    painter.setPen(pen);
                }
                painter.drawLine(line);
            }
            painter.setPen(pen);

            for (unsigned int i=1; i<limit_h+1; i++) { // linhas HORIZONTAIS
                pos = map_pos_y + i*TILESIZE*Mediator::get_instance()->zoom;
                if (i == 0 || i % AREA_ROOM_H == 0) {
                    painter.setPen(pen_red);
                } else {
                    painter.setPen(pen);
                }
                line = QLineF(map_pos_x, pos, map_pos_x+map_size_w, pos);
                painter.drawLine(line);
            }
        }
    }

    drawTileset(&painter);
    if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
        drawLockTileset(&painter);
    }
    //drawMapEnemies(&painter);


}


void EditorArea::preload_slope_images()
{
    slope_image_list.clear();
    for (unsigned int i=0; i<SharedData::get_instance()->slope_list.size(); i++) {
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
    //std::cout << "########### EditorArea::draw_slope_tile" << std::endl;
    if (slope_image_list.find(x) == slope_image_list.end()) {
        std::cout << "slope x[" << x << "] not found" << std::endl;
        return;
    }
    QRectF source(QPoint(y*TILESIZE, 0), QSize(TILESIZE, TILESIZE));
    QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
    //std::cout << "DRAW-SLOPE AT x[" << target.x() << "], y[" << target.y() << "]" << std::endl;
    painter->drawPixmap(target, slope_image_list.at(x), source);
}


void EditorArea::drawTileset(QPainter *painter)
{
    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {
        int map_pos_x = (it->first.x-leftmost_point)*AREA_ROOM_W*TILESIZE*Mediator::get_instance()->zoom;
        int map_pos_y = (it->first.y-topmost_point)*AREA_ROOM_H*TILESIZE*Mediator::get_instance()->zoom;

        /*
        for (int x=0; x<AREA_ROOM_W; x++) {
            for (int y=0; y<AREA_ROOM_H; y++) {
                file_v6_room_tile tileItem = it->second.tiles[x][y];
                if (tileItem.tile_underlay.x != -1 && tileItem.tile_underlay.y != -1) {
                    int dest_x = map_pos_x + x*TILESIZE*Mediator::get_instance()->zoom;
                    int dest_y = map_pos_y + y*TILESIZE*Mediator::get_instance()->zoom;

                    QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                    QRectF source(QPoint((tileItem.tile_underlay.x*TILESIZE), (tileItem.tile_underlay.y*TILESIZE)), QSize(TILESIZE, TILESIZE));

                    painter->drawPixmap(target, tileset_image, source);
                }
            }
        }
        */

        for (int x=0; x<AREA_ROOM_W; x++) {
            for (int y=0; y<AREA_ROOM_H; y++) {
                file_v6_room_tile tileItem = it->second.tiles[x][y];

                if (tileItem.tile_underlay.x != -1 && tileItem.tile_underlay.y != -1) {

                    int dest_x = map_pos_x + x*TILESIZE*Mediator::get_instance()->zoom;
                    int dest_y = map_pos_y + y*TILESIZE*Mediator::get_instance()->zoom;


                    std::cout << "TILE AT [" << x << "][" << y << "], with x[" << tileItem.tile_underlay.x << "], y[" << tileItem.tile_underlay.y << "] type is [" << tileItem.tile_underlay.type << "]" << std::endl;
                    if (tileItem.tile_underlay.type == TILE_TYPE_SOLID) {
                        QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                        QRectF source(QPoint((tileItem.tile_underlay.x*TILESIZE), (tileItem.tile_underlay.y*TILESIZE)), QSize(TILESIZE, TILESIZE));
                        painter->drawPixmap(target, tileset_image, source);
                    } else if (tileItem.tile_underlay.type == TILE_TYPE_SLOPE) {
                        std::cout << "FOUND SLOPE_TILE AT [" << x << "][" << y << "], with x[" << tileItem.tile_underlay.x << "], y[" << tileItem.tile_underlay.y << "]" << std::endl;
                        draw_slope_tile(tileItem.tile_underlay.x, tileItem.tile_underlay.y, dest_x, dest_y, painter);
                    } else if (tileItem.tile_underlay.type == TILE_TYPE_ANIM) {
                        int anim_tile_id = tileItem.tile_underlay.x;
                        //std::cout << "FOUND ANIM_TILE AT [" << k << "][" << m << "], with ID [" << anim_tile_id << "]" << std::endl;
                        if (Mediator::get_instance()->anim_block_list.size() > 0 && anim_tile_id < Mediator::get_instance()->anim_block_list.size()) {
                            file_anim_block anim_tile = Mediator::get_instance()->anim_block_list.at(anim_tile_id);
                            QString anim_tile_filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/anim/") + QString(anim_tile.filename);
                            QPixmap anim_image(anim_tile_filename);
                            if (anim_image.isNull() == false) {
                                QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                                QRectF source(QPoint(0, 0), QSize(TILESIZE, TILESIZE));
                                painter->drawPixmap(target, anim_image, source);

                                // @TODO (move to a function) - draw an green border border to indicate anim tile
                                QPen pen(QColor(0, 200, 0), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
                                painter->setPen(pen);
                                if (Mediator::get_instance()->show_grid) {
                                    int anim_tile_x = map_pos_x * TILESIZE * Mediator::get_instance()->zoom; // minus tilesize is because width starts in 1, not zero
                                    int anim_tile_y = map_pos_y * TILESIZE *Mediator::get_instance()->zoom;
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
                    std::cout << "UNUSED TILE x[" << x << "], y[" << y << "]" << std::endl;
                    painter->setBrush(QColor(240, 240, 240, 255));
                    painter->drawRect(map_pos_x*TILESIZE*Mediator::get_instance()->zoom, map_pos_y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                }
                if (Mediator::get_instance()->show_fg_layer == true && tileItem.tile_overlay.x >= 0 && tileItem.tile_overlay.y >= 0) {
                    std::cout << ">>>>>>>>>>>>>>>>>> overlay.draw[" << map_pos_x << "][" << map_pos_y << "]" << std::endl;
                }
            }
        }

    }
}

void EditorArea::drawLockTileset(QPainter *painter)
{
    for (std::map<st_position, file_v6_room>::iterator it = SharedData::get_instance()->v6_area_room_list.begin(); it != SharedData::get_instance()->v6_area_room_list.end(); ++it) {


        int map_pos_x = (it->first.x-leftmost_point)*AREA_ROOM_W*TILESIZE*Mediator::get_instance()->zoom;
        int map_pos_y = (it->first.y-topmost_point)*AREA_ROOM_H*TILESIZE*Mediator::get_instance()->zoom;

        for (int x=0; x<AREA_ROOM_W; x++) {
            for (int y=0; y<AREA_ROOM_H; y++) {
                file_v6_room_tile tileItem = it->second.tiles[x][y];
                int dest_x = map_pos_x + x*TILESIZE*Mediator::get_instance()->zoom;
                int dest_y = map_pos_y + y*TILESIZE*Mediator::get_instance()->zoom;

                QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                QRectF source(QPoint((tileItem.tile_underlay.x*TILESIZE), (tileItem.tile_underlay.y*TILESIZE)), QSize(TILESIZE, TILESIZE));



                if (tileItem.locked != TERRAIN_UNBLOCKED) {

                    // used images depends upon tile type

                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(QColor(255, 0, 0, 255));
                    painter->drawRect(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
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
                    } else if (tileItem.locked == TERRAIN_SCROLL_LOCK) {
                        terrainIcon = QString(":/toolbar_icons/view-pim-notes.png"); // diagonal left
                    } else if (tileItem.locked == -2) {
                        terrainIcon = QString(":/toolbar_icons/dialog-cancel.png"); // diagonal left
                    }


                    if (terrainIcon.length() > 0) {
                        QPixmap terrainImage(terrainIcon);
                        if (terrainImage.isNull()) {
                            printf("ERROR: EditorArea::paintEvent - terrainType - Could not load image file '%s'\n", qPrintable(terrainIcon));
                        } else {
                            terrainIcon.resize(TILESIZE);
                            //painter->setOpacity(0.7);
                            QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                            QRectF source(QPoint(0, 0), QSize(terrainImage.width(), terrainImage.height ()));
                            painter->drawPixmap(target, terrainImage, source);
                        }
                    }
                    painter->setOpacity(1.0);
                }

            }
        }
    }


/*
    for (int i=leftmost_room; i<=rightmost_room; i++) {
        for (int j=topmost_room; j<=bottommost_room; j++) {
            if (SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[i][j].area_n == SharedData::get_instance()->v6_selected_area) {
                file_v6_room room_data = SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[i][j];
                for (int k=0; k<AREA_ROOM_W; k++) {
                    for (int m=0; m<AREA_ROOM_H; m++) {
                        file_v6_room_tile tileItem = room_data.tiles[k][m];
                        if (tileItem.locked != TERRAIN_UNBLOCKED) {

                            int virtual_room_x = i-leftmost_room;
                            int virtual_room_y = j-topmost_room;

                            int dest_x = (k+virtual_room_x*AREA_ROOM_W)*TILESIZE*Mediator::get_instance()->zoom;
                            int dest_y = (m+virtual_room_y*AREA_ROOM_H)*TILESIZE*Mediator::get_instance()->zoom;

                            // used images depends upon tile type

                            std::cout << "LOCK IN MAP[" << i << "][" << j << "], AT [" << k << "][" << m << "], type[" << tileItem.tile_underlay.type << "]" << std::endl;

                            painter->setBrush(Qt::NoBrush);
                            painter->setPen(QColor(255, 0, 0, 255));
                            painter->drawRect(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
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
                            } else if (tileItem.locked == TERRAIN_SCROLL_LOCK) {
                                terrainIcon = QString(":/toolbar_icons/view-pim-notes.png"); // diagonal left
                            } else if (tileItem.locked == -2) {
                                terrainIcon = QString(":/toolbar_icons/dialog-cancel.png"); // diagonal left
                            }


                            if (terrainIcon.length() > 0) {
                                QPixmap terrainImage(terrainIcon);
                                if (terrainImage.isNull()) {
                                    printf("ERROR: EditorArea::paintEvent - terrainType - Could not load image file '%s'\n", qPrintable(terrainIcon));
                                } else {
                                    terrainIcon.resize(TILESIZE);
                                    //painter->setOpacity(0.7);
                                    QRectF target(QPoint(dest_x, dest_y), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                                    QRectF source(QPoint(0, 0), QSize(terrainImage.width(), terrainImage.height ()));
                                    painter->drawPixmap(target, terrainImage, source);
                                }
                            }
                            painter->setOpacity(1.0);
                        }
                    }
                }
            }
        }
    }
*/
}

void EditorArea::drawMapEnemies(QPainter *painter)
{
    int currentMap = SharedData::get_instance()->v6_selected_area;

    if (SharedData::get_instance()->file_v5_map_npc_map.find(currentMap) == SharedData::get_instance()->file_v5_map_npc_map.end()) {
        return;
    }

    // DRAW ENEMIES BACKGROUNDS //
    if (Mediator::get_instance()->show_npcs_flag == true) {
        /// draw NPCs
        //std::cout << "EditorArea::drawMapEnemies currentMap[" << currentMap << "], npc-size[" << SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).size() << "]" << std::endl;
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
                if (Mediator::get_instance()->enemy_list.at(npc_id).is_boss) {
                    // translucid orange rectangle
                    painter->setBrush(QColor(255, 128, 35, 200));
                    painter->drawRect(target);
                } else if (Mediator::get_instance()->enemy_list.at(npc_id).is_sub_boss) {
                    // translucid bright-orange rectangle
                    painter->setBrush(QColor(241, 188, 87, 200));
                    painter->drawRect(target);
                }
                if (SharedData::get_instance()->game_data.final_boss_id == npc_id) {
                    // golden border
                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(QColor(255, 215, 0, 255));
                    painter->drawRect(target);
                }


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
    //std::cout << "################### EditorArea::drawMapObjects - START LOOP" << std::endl;
    unsigned int current_map = SharedData::get_instance()->v6_selected_area;
    if (SharedData::get_instance()->file_v6_map_object_map.find(current_map) == SharedData::get_instance()->file_v6_map_object_map.end()) {
        std::cout << "EditorArea::drawMapObjects - no objects in this area[" << current_map << "]" << std::endl;
        return;
    }
    for (int i=0; i<SharedData::get_instance()->file_v6_map_object_map.at(current_map).size(); i++) {
        int obj_id = (int)SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).id_object;



        if (obj_id != -1 && SharedData::get_instance()->v6_object_list.size() > obj_id) {
            //std::cout << "EditorArea::drawMapObjects - OBJ[" << i << "][" << SharedData::get_instance()->v6_object_list.at(obj_id).name << "]" << std::endl;
            //if (obj_id == 20) { std::cout << "************************** paintEvent - draw_objects[" << i << "].id: " << obj_id << std::endl; }
            std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/objects/" + SharedData::get_instance()->v6_object_list.at(obj_id).graphic_filename;
            QPixmap temp_image(filename.c_str());
            if (temp_image.isNull()) {
                std::cout << "****************** Could not load file '" << filename.c_str() << "'" << std::endl;
                painter->setBrush(QColor(255, 255, 255, 180));
                painter->drawRect(SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
            } else {
                int obj_type = SharedData::get_instance()->v6_object_list.at(obj_id).type;
                int obj_direction = SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).direction;
                if (obj_type == OBJ_RAY_HORIZONTAL) {
                    int graphic_pos_x = 0;
                    int graphic_pos_y = SharedData::get_instance()->v6_object_list.at(obj_id).size.height;
                    if (obj_direction == ANIM_DIRECTION_LEFT) {
                        graphic_pos_x = SharedData::get_instance()->v6_object_list.at(obj_id).size.width - TILESIZE;
                        graphic_pos_y = 0;
                    }
                    //std::cout << "OBJ_RAY_HORIZONTAL - obj_direction: " << obj_direction << ", graphic_pos_x: " << graphic_pos_x << ", graphic_pos_y: " << graphic_pos_y << std::endl;
                    QRectF target(QPoint(SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(graphic_pos_x, graphic_pos_y), QSize(TILESIZE, SharedData::get_instance()->v6_object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else if (obj_type == OBJ_RAY_VERTICAL) {
                    int graphic_pos_y = SharedData::get_instance()->v6_object_list.at(obj_id).size.height - TILESIZE;
                    //std::cout << "OBJ_RAY_HORIZONTAL - graphic_pos_y: " << graphic_pos_y << std::endl;
                    QRectF target(QPoint(SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(0, graphic_pos_y), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width, SharedData::get_instance()->v6_object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else if (obj_type == OBJ_DEATHRAY_HORIZONTAL && obj_direction == ANIM_DIRECTION_LEFT) {
                    QRectF target(QPoint(SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom - (SharedData::get_instance()->v6_object_list.at(obj_id).size.width-TILESIZE)*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source(QPoint(0, 0), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width, SharedData::get_instance()->v6_object_list.at(obj_id).size.height));
                    painter->drawPixmap(target, temp_image, source);
                } else {
                    QRectF target(QPoint(SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom, SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                    QRectF source;
                    if (obj_direction == ANIM_DIRECTION_RIGHT && temp_image.height() >= SharedData::get_instance()->v6_object_list.at(obj_id).size.height) {
                        source = QRectF(QPoint(0, SharedData::get_instance()->v6_object_list.at(obj_id).size.height), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width, SharedData::get_instance()->v6_object_list.at(obj_id).size.height));
                    } else {
                        source = QRectF(QPoint(0, 0), QSize(SharedData::get_instance()->v6_object_list.at(obj_id).size.width, SharedData::get_instance()->v6_object_list.at(obj_id).size.height));
                    }
                    painter->drawPixmap(target, temp_image, source);
                }
            }
            // draw object-teleporter origin
            if (SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).dest_map != -1) {
                int obj_w = SharedData::get_instance()->v6_object_list.at(obj_id).size.width;
                int obj_h = SharedData::get_instance()->v6_object_list.at(obj_id).size.height;
                //std::cout << "OBJ.w: " << obj_w << ", obj_h: " << obj_h << std::endl;
                int dest_x = SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x*TILESIZE*Mediator::get_instance()->zoom;
                int dest_y = SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y*TILESIZE*Mediator::get_instance()->zoom;
                //std::cout << "DRAW OBJECT TELEPORTER ORIGIN[" << i << "] - map: " << (int)SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).map_dest  << ", x: " << SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.x << ", y: " << SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(i).start_point.y << std::endl;
                if (SharedData::get_instance()->v6_object_list.at(obj_id).type == OBJ_FINAL_BOSS_TELEPORTER) {
                    painter->setBrush(QColor(160, 60, 60, 180));
                } else {
                    painter->setBrush(QColor(60, 160, 60, 180));
                }
                painter->drawEllipse(dest_x, dest_y, SharedData::get_instance()->v6_object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom);
                // cyan border
                painter->setBrush(Qt::NoBrush);
                painter->setPen(QColor(0, 255, 255, 255));
                painter->drawEllipse(dest_x, dest_y, SharedData::get_instance()->v6_object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, SharedData::get_instance()->v6_object_list.at(obj_id).size.height*Mediator::get_instance()->zoom);
                painter->setPen(QColor(0, 0, 0, 255));
                painter->drawText(dest_x+3, dest_y+TILESIZE-3, QString::number(i));
            }
        }
    }

}




void EditorArea::mouseMoveEvent(QMouseEvent *event) {
	QPoint pnt = event->pos();

    if (Mediator::get_instance()->editMode != EDITMODE_SELECT) {
        // forces "click" when moving
        int pointX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom);
        int pointY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom);
        if (editor_selectedTileX != pointX || editor_selectedTileY != pointY) {
            //std::cout << ">>>>>>>>> EditorArea::mouseMoveEvent::CLICK, editor.x[" << editor_selectedTileX << "], editor.y[" << editor_selectedTileY << "], pointX[" << pointX << "], pointY[" << pointY << "]" << std::endl;
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
    update_editarea_size();
    event->accept();
}

int EditorArea::find_npc_in_position(int x, int y)
{
    int currentMap = SharedData::get_instance()->v6_selected_area;
    if (SharedData::get_instance()->file_v5_map_npc_map.find(currentMap) == SharedData::get_instance()->file_v5_map_npc_map.end()) {
        return -1;
    }

    // search if there is an existing NPC in ths position, and if yes, remove it
    for (int i=0; i<SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).size(); i++) {
        if (SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).id_npc != -1 && SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.x == x && SharedData::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.y == y) {
            return i;
        }
    }
    return -1;
}



void EditorArea::mousePressEvent(QMouseEvent *event) {
    if (mouse_released == false && (Mediator::get_instance()->editTool == EDITMODE_LINK || Mediator::get_instance()->editTool == EDITMODE_LINK_DEST || Mediator::get_instance()->editMode == EDITMODE_NPC || Mediator::get_instance()->editMode == EDITMODE_OBJECT || Mediator::get_instance()->editMode == EDITMODE_SET_BOSS || Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE)) {
        std::cout << "EDITORAREA::mousePressEvent - IGNORED" << std::endl;
		return;
	}

    //std::cout << "EDITORAREA::mousePressEvent - EXECUTE, mode[" << Mediator::get_instance()->editTool << "]" << std::endl;
    //std::cout << "SET-SLOPE-MODE [" << Mediator::get_instance()->editMode << "]" << std::endl;

    QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom);
    editor_selectedTileY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom);


    int room_x = leftmost_point+editor_selectedTileX/AREA_ROOM_W;
    int room_y = topmost_point+editor_selectedTileY/AREA_ROOM_H;
    int tile_x = (leftmost_point*AREA_ROOM_W)+editor_selectedTileX - (room_x*AREA_ROOM_W);
    int tile_y = (topmost_point*AREA_ROOM_H)+editor_selectedTileY - (room_y*AREA_ROOM_H);

    std::cout << "### editMode[" << Mediator::get_instance()->editMode << "], editTool[" << Mediator::get_instance()->editTool << "], room_x[" << room_x << "], room_y[" << room_y << "]" << std::endl;

    st_position index = st_position(room_x, room_y);

    if (SharedData::get_instance()->v6_area_room_list.find(index) == SharedData::get_instance()->v6_area_room_list.end()) {
        return;
    }

    if (Mediator::get_instance()->editMode == EDITMODE_NORMAL) {
        if (Mediator::get_instance()->editTool == EDITMODE_NORMAL) {
            //std::cout << "### tile_x[" << tile_x << "], tile_y[" << tile_y << "], palette_x[" << Mediator::get_instance()->getPalleteX() << "], palette_y[" << Mediator::get_instance()->getPalleteY() << "]" << std::endl;
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.x = Mediator::get_instance()->getPalleteX();
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.y = Mediator::get_instance()->getPalleteY();
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.type = TILE_TYPE_SOLID;
            repaint();
            return;
        } else if (Mediator::get_instance()->editTool == EDITMODE_ERASER) {
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.x = -1;
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.y = -1;
            SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.type = TILE_TYPE_SOLID;
            repaint();
            return;
        } else if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
            if (SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].locked != TERRAIN_UNBLOCKED) {
                SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].locked = TERRAIN_UNBLOCKED;
            } else {
                std::cout << "SET tile_x[" << tile_x << "], tile_y[" << tile_y << "] lock to [" << Mediator::get_instance()->terrainType << "]" << std::endl;
                SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].locked = Mediator::get_instance()->terrainType;
            }
            repaint();
            return;
        } else {
            QMessageBox msgBox;
            msgBox.setText(QString("Edit Tool [") + QString::number(Mediator::get_instance()->editTool) + QString("] not implemented"));
            msgBox.exec();
        }
    } else if (Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE) {
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.x = Mediator::get_instance()->selectedAnimTileset;
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.y = 0;
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.type = TILE_TYPE_ANIM;
        repaint();
        return;
    } else if (Mediator::get_instance()->layerLevel == 1 && Mediator::get_instance()->editMode == EDITMODE_SLOPE) {
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.x = Mediator::get_instance()->getPalleteX();
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.y = Mediator::get_instance()->getPalleteY();
        SharedData::get_instance()->v6_area_room_list.at(index).tiles[tile_x][tile_y].tile_underlay.type = TILE_TYPE_SLOPE;
        std::cout << "###### PLACE SLOPE [" << tile_x << "][" << tile_y << "] with [" << Mediator::get_instance()->getPalleteX() << "][" << Mediator::get_instance()->getPalleteY() << "]" << std::endl;
        repaint();
        return;
    }

    /*

    if (Mediator::get_instance()->editMode == EDITMODE_NORMAL || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE || Mediator::get_instance()->editMode == EDITMODE_SLOPE) {

        // FILE-V6 //

        int room_x = leftmost_room+editor_selectedTileX/AREA_ROOM_W;
        int room_y = topmost_room+editor_selectedTileY/AREA_ROOM_H;

        std::cout << "EDITORAREA::mousePressEvent - DEBUG #1 x[" << editor_selectedTileX << "] y[" << editor_selectedTileY << "], room_x[" << room_x << "], room_y[" << room_y << "]" << std::endl;


        if (SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].area_n == SharedData::get_instance()->v6_selected_area) {
            //std::cout << "EDITORAREA::mousePressEvent - DEBUG #2" << std::endl;

            int tile_x = (leftmost_room*AREA_ROOM_W)+editor_selectedTileX - (room_x*AREA_ROOM_W);
            int tile_y = (topmost_room*AREA_ROOM_H)+editor_selectedTileY - (room_y*AREA_ROOM_H);

            if (Mediator::get_instance()->editTool == EDITMODE_NORMAL || Mediator::get_instance()->editTool == EDITMODE_ERASER || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE || Mediator::get_instance()->editMode == EDITMODE_SLOPE) {

                std::cout << "mousePressEvent.DEBUG#2 - x[" << editor_selectedTileX << "], y[" << editor_selectedTileY << "]" << std::endl;
                e_TILE_TYPE valueType = TILE_TYPE_SOLID;
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


                if (Mediator::get_instance()->layerLevel == 1) {
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_underlay.type = valueType;
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_underlay.x = valueX;
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_underlay.y = valueY;
                } else {
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_overlay.type = valueType;
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_overlay.x = valueX;
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].tile_overlay.y = valueY;
                }

                std::cout << "SET LEVEL[" << SharedData::get_instance()->v6_selected_level << "].AREA[" << SharedData::get_instance()->v6_selected_area << "].ROOM[" << room_x << "][" << room_y << "].TILE[" << tile_x << "][" << tile_y << "] to [" << valueType << "][" << valueX << "][" << valueY << "]" << std::endl;
            } else if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
                if (SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].locked == Mediator::get_instance()->terrainType) {
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].locked = TERRAIN_UNBLOCKED;
                } else {
                    SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].locked = Mediator::get_instance()->terrainType;
                }
                std::cout << "EDITORAREA::mousePressEvent - DEBUG #3 [EDITMODE_LOCK] x[" << editor_selectedTileX << "] y[" << editor_selectedTileY << "], room_x[" << room_x << "], room_y[" << room_y << "] SET TO [" << SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_selected_level).rooms[room_x][room_y].tiles[tile_x][tile_y].locked << "]" << std::endl;

            }
        }
    } else if (Mediator::get_instance()->editMode == EDITMODE_LINK) {
        if (Mediator::get_instance()->editTool == EDITMODE_LINK && tempX == -1) {
                    // checks if a link in this position already exits to remove it
                    for (int i=0; i<SharedData::get_instance()->file_v5_map_link_list.size(); i++) {
                        file_v5_map_link* link_data = &SharedData::get_instance()->file_v5_map_link_list.at(i);
                        if (link_data->id_map_origin == SharedData::get_instance()->v6_selected_area) {
                            if (link_data->pos_origin.x == editor_selectedTileX && link_data->pos_origin.y == editor_selectedTileY) {
                                SharedData::get_instance()->file_v5_map_link_list.erase(SharedData::get_instance()->file_v5_map_link_list.begin()+i);
                                repaint();
                                return;
                            }
                        } else if (link_data->id_map_destiny == SharedData::get_instance()->v6_selected_area) {
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

                    link_data->id_map_destiny = SharedData::get_instance()->v6_selected_area;
                    link_data->pos_destiny.x = editor_selectedTileX;
                    link_data->pos_destiny.y = editor_selectedTileY;
                    link_data->link_direction = link_direction;

                    Mediator::get_instance()->editTool = EDITMODE_LINK;
                    repaint();
                    QApplication::setOverrideCursor(Qt::ArrowCursor);
                    return;
                }




    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_BOSS) {
        int found_npc = find_npc_in_position(editor_selectedTileX, editor_selectedTileY);
        if (found_npc != -1) {
            int npc_id = SharedData::get_instance()->file_v5_map_npc_map.at(SharedData::get_instance()->v6_selected_area).at(found_npc).id_npc;
            Mediator::get_instance()->enemy_list.at(npc_id).is_boss = true;
        }
    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS) {
        int found_npc = find_npc_in_position(editor_selectedTileX, editor_selectedTileY);
        if (found_npc != -1) {
            int npc_id = SharedData::get_instance()->file_v5_map_npc_map.at(SharedData::get_instance()->v6_selected_area).at(found_npc).id_npc;
            Mediator::get_instance()->enemy_list.at(npc_id).is_sub_boss = true;
        }
    } else if (Mediator::get_instance()->editMode == EDITMODE_NPC) {
		printf(">> EditorArea::mousePressEvent - EDITMODE_NPC\n");

		int found_npc = -1;
        int currentMap = SharedData::get_instance()->v6_selected_area;
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
        unsigned int current_map = SharedData::get_instance()->v6_selected_area;
        if (SharedData::get_instance()->file_v6_map_object_map.find(current_map) == SharedData::get_instance()->file_v6_map_object_map.end()) {
            SharedData::get_instance()->file_v6_map_object_map.insert(std::pair<unsigned int, std::vector<v6_map_object>>(current_map, std::vector<v6_map_object>()));
        } else {
            for (int m=0; m<SharedData::get_instance()->file_v6_map_object_map.at(current_map).size(); m++) {
                if (SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(m).id_object != -1 && SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(m).start_point.x == editor_selectedTileX && SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(m).start_point.y == editor_selectedTileY) {
                    found_object = m;
                    break;
                }
            }
        }

        if (Mediator::get_instance()->editTool == EDITMODE_ERASER && found_object != -1) {
            std::cout << "remove object - slot: " << found_object << std::endl;
            SharedData::get_instance()->file_v6_map_object_map.at(current_map).erase(SharedData::get_instance()->file_v6_map_object_map.at(current_map).begin()+found_object);
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_object == -1 && Mediator::get_instance()->selectedNPC != -1) {
                v6_map_object new_obj;
                new_obj.id_object = Mediator::get_instance()->selectedNPC;
                new_obj.start_point.x = editor_selectedTileX;
                new_obj.start_point.y = editor_selectedTileY;
                new_obj.direction = Mediator::get_instance()->object_direction;
                std::cout << "$$$$$$$$$$$$$$ game_data.uuid[" << SharedData::get_instance()->game_data.obj_uuid << "]" << std::endl;
                new_obj.uuid = SharedData::get_instance()->game_data.obj_uuid;
                // se item é teleportador, deve entrar no modo de colocar link de object
                int obj_type = SharedData::get_instance()->v6_object_list.at(Mediator::get_instance()->selectedNPC).type;
                if (obj_type == OBJ_BOSS_TELEPORTER || obj_type == OBJ_FINAL_BOSS_TELEPORTER || obj_type == OBJ_PLATFORM_TELEPORTER || obj_type == OBJ_STAGE_BOSS_TELEPORTER || obj_type == OBJ_FRONT_DOOR_TELEPORTER) {
                    editor_selected_object_pos = SharedData::get_instance()->file_v6_map_object_map.size();
                    std::cout << "SET editor_selected_object_pos: " << editor_selected_object_pos << std::endl;
                    Mediator::get_instance()->editTool = EDITMODE_OBJECT_LINK_PLACING;
                    QApplication::setOverrideCursor(Qt::CrossCursor);
                    // @TODO - desabilita todos os modos, não pode sair no meio de object-link-placing
                    // @TODO - desabilitar mudar de estágio também (e isso tem que ser feito no link tb)
                }
                SharedData::get_instance()->file_v6_map_object_map.at(current_map).push_back(new_obj);
                obj_ref = &SharedData::get_instance()->file_v6_map_object_map.at(current_map).at(SharedData::get_instance()->file_v6_map_object_map.at(current_map).size()-1);
                SharedData::get_instance()->game_data.obj_uuid++; // increase unique counter
                repaint();
        } else if (Mediator::get_instance()->editTool == EDITMODE_OBJECT_LINK_PLACING) {
            std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>> place teleport link" << std::endl;
            if (obj_ref != nullptr) {
                obj_ref->dest_map = SharedData::get_instance()->v6_selected_area;
                obj_ref->dest_position.x  = editor_selectedTileX;
                obj_ref->dest_position.y  = editor_selectedTileY;
            } else {
                std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>> place teleport link error: object is null" << std::endl;
            }
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            obj_ref = nullptr;
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
    */
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

        link_map_origin = SharedData::get_instance()->v6_selected_area;
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









