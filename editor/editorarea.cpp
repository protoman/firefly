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

extern std::string FILEPATH;

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
    std::string filename_str = FILEPATH + "images/tilesets/blocks/easymode.png";
    easy_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = FILEPATH + "images/tilesets/blocks/hardmode.png";
    hard_mode_tile = QPixmap(QString(filename_str.c_str()));

    filename_str = FILEPATH + std::string("/images/tilesets/") + Mediator::get_instance()->getPallete();
    if (filename_str.length() <= 0) {
        return;
    }
    //std::cout << "EditorArea::paintEvent - filename: " << filename.toStdString() << std::endl;
    tileset_image = QPixmap(filename_str.c_str());
    if (tileset_image.isNull() != false) {
        tileset_bitmap.clear();
    }

    std::string bg1_filename = FILEPATH +"/images/map_backgrounds/"+ Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].filename;
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

    std::string fg_filename = FILEPATH +"/images/map_backgrounds/"+ Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].filename;
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
    fg_opacity = (float)Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].gfx/100;
}


void EditorArea::drawTileset(QPainter *painter)
{

    // regular tiles
    if (Mediator::get_instance()->file_v5_map_tile_map.find(Mediator::get_instance()->file_v5_selected_map) != Mediator::get_instance()->file_v5_map_tile_map.end()) {
        //Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).size()
        int map_w = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w;
        int map_h = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h;
        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                int n = j*map_w + i;
                if (Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).size() > 0) {
                    file_v5_map_tile tileItem = Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n);

                    if (tileItem.tile_underlay.x >= 0 && tileItem.tile_underlay.y >= 0) {
                        QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                        QRectF source(QPoint((tileItem.tile_underlay.x*TILESIZE), (tileItem.tile_underlay.y*TILESIZE)), QSize(TILESIZE, TILESIZE));
                        // used images depends upon tile type
                        if (tileItem.tile_underlay.type == TILE_TYPE_SOLID) {
                            //std::cout << "FOUND SOLID_TILE AT [" << i << "][" << j << "]" << std::endl;
                            painter->drawPixmap(target, tileset_image, source);
                        } else if (tileItem.tile_underlay.type == TILE_TYPE_SLOPE) {
                            /// @TODO ///
                            //std::cout << "FOUND SLOPE_TILE AT [" << i << "][" << j << "], with ID [TODO]" << std::endl;
                        } else if (tileItem.tile_underlay.type == TILE_TYPE_ANIM) {
                            int anim_tile_id = tileItem.tile_underlay.x;
                            //std::cout << "FOUND ANIM_TILE AT [" << i << "][" << j << "], with ID [" << anim_tile_id << "]" << std::endl;
                            if (Mediator::get_instance()->anim_block_list.size() > 0 && anim_tile_id < Mediator::get_instance()->anim_block_list.size()) {
                                CURRENT_FILE_FORMAT::file_anim_block anim_tile = Mediator::get_instance()->anim_block_list.at(anim_tile_id);
                                QString anim_tile_filename = QString(FILEPATH.c_str()) + QString("/images/tilesets/anim/") + QString(anim_tile.filename);
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

                    }
                    // EASY-mode tiles
                    if (tileItem.locked == TERRAIN_EASYMODEBLOCK) {
                        //std::cout << "TERRAIN_EASYMODEBLOCK" << std::endl;

                        QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                        QRectF source(QPoint(0, 0), QSize(TILESIZE, TILESIZE));
                        painter->drawPixmap(target, easy_mode_tile, source);

                        painter->setBrush(QColor(220, 210, 50, 100));
                        painter->drawRect(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);

                    // HARD-mode tiles
                    } else if (tileItem.locked == TERRAIN_HARDMODEBLOCK) {
                        QRectF target(QPoint(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom));
                        QRectF source(QPoint(0, 0), QSize(TILESIZE, TILESIZE));
                        painter->drawPixmap(target, hard_mode_tile, source);

                        painter->setBrush(QColor(190, 36, 230, 100));
                        painter->drawRect(i*TILESIZE*Mediator::get_instance()->zoom, j*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    }

                }
            }
        }
    }

}

void EditorArea::drawLockTileset(QPainter *painter)
{
    // regular tiles
    if (Mediator::get_instance()->file_v5_map_tile_map.find(Mediator::get_instance()->file_v5_selected_map) != Mediator::get_instance()->file_v5_map_tile_map.end()) {
        //Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).size()
        int map_w = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w;
        int map_h = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h;
        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                int n = j*map_w + i;
                if (Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).size() > 0) {
                    file_v5_map_tile tileItem = Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n);

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
                    } else if (tileItem.locked == TERRAIN_SCROLL_LOCK) {
                        terrainIcon = QString(":/toolbar_icons/system-switch-user.png");
                    } else if (tileItem.locked == TERRAIN_EASYMODEBLOCK) {
                        terrainIcon = QString(":/toolbar_icons/draw-polygon.png"); // easy block
                    } else if (tileItem.locked == TERRAIN_HARDMODEBLOCK) {
                        terrainIcon = QString(":/toolbar_icons/draw-square-inverted-corners.png"); // hard block


                    } else if (tileItem.locked == TERRAIN_DIAGONAL_LEFT) {
                        terrainIcon = QString(":/toolbar_icons/draw-triangle.png"); // diagonal left
                    } else if (tileItem.locked == TERRAIN_DIAGONAL_RIGHT) {
                        terrainIcon = QString(":/toolbar_icons/games-difficult.png"); // diagonal right

                    }
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
                    painter->setOpacity(1.0);



                }
            }
        }
    }
}

void EditorArea::drawMapEnemies(QPainter *painter)
{
    int currentMap = Mediator::get_instance()->file_v5_selected_map;

    if (Mediator::get_instance()->file_v5_map_npc_map.find(currentMap) == Mediator::get_instance()->file_v5_map_npc_map.end()) {
        return;
    }

    // DRAW ENEMIES BACKGROUNDS //
    if (Mediator::get_instance()->show_npcs_flag == true) {
        /// draw NPCs
        std::cout << "EditorArea::drawMapEnemies currentMap[" << currentMap << "], npc-size[" << Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).size() << "]" << std::endl;
        for (int i=0; i<Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).size(); i++) {
            file_v5_map_npc map_npc = Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).at(i);
            if (map_npc.difficulty_mode == DIFFICULTY_MODE_GREATER && map_npc.difficulty_level > Mediator::get_instance()->currentDifficulty) {
                std::cout << "EditorArea::drawMapEnemies - IGNORE due to difficulty #1" << std::endl;
                continue; // only show enemies with equal or lower difficulty
            } else if (map_npc.difficulty_mode == DIFFICULTY_MODE_EQUAL && map_npc.difficulty_level != Mediator::get_instance()->currentDifficulty) {
                std::cout << "EditorArea::drawMapEnemies - IGNORE due to difficulty #2" << std::endl;
                continue;
            }
            //std::cout << "EditorArea::paintEvent #5.0.A [" << i << "]" << std::endl;
            int npc_id = map_npc.id_npc;
            if (npc_id >= Mediator::get_instance()->enemy_list.size() || npc_id < 0) {
                map_npc.id_npc = -1;
                continue;
            }
            std::string npc_bg_file(Mediator::get_instance()->enemy_list.at(npc_id).bg_graphic_filename);
            if (npc_bg_file.length() > 0) {
                std::string _bg_graphic_filename = FILEPATH + "/images/sprites/enemies/backgrounds/" + npc_bg_file;
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
            std::string filename = FILEPATH + "/images/sprites/enemies/" + Mediator::get_instance()->enemy_list.at(npc_id).graphic_filename;
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

}


void EditorArea::paintEvent(QPaintEvent *) {

    if (Mediator::get_instance()->currentStage < 0) {
        return;
    }

    if (tileset_image.isNull()) {
        std::cout << "ERROR: EditorArea::paintEvent - Could not load palette image file." << std::endl;
        return;
    }


    int i=0, j, pos;
    QPainter painter(this);
    QLineF line;
    QString filename;


	// draw background-color
    if (Mediator::get_instance()->show_background_color == true) {
        //painter.setPen(QColor(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.r, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.g, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.b, 255));
        painter.fillRect(QRectF(0.0, 0.0, MAP_W*TILESIZE*Mediator::get_instance()->zoom, MAP_H*TILESIZE*Mediator::get_instance()->zoom), QColor(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.r, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.g, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.b, 255));
	}
	// draw background1
    std::string bg1_filename(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].filename);
    if (Mediator::get_instance()->show_bg1 == true) {
        if (!bg1_image.isNull()) {
            //std::cout << "DRAW BG1" << std::endl;
            int max_repeat = ((MAP_W*TILESIZE)/bg1_image.width())*Mediator::get_instance()->zoom+1;
            //std::cout << "bg1_image.width(): " << bg1_image.width() << ", max_repeat: " << max_repeat << std::endl;
            for (int k=0; k<max_repeat; k++) {
                QRectF pos_source(QPoint(0, 0), QSize(bg1_image.width(), bg1_image.height()));
                QRectF pos_dest(QPoint(k*bg1_image.width()*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].adjust_y*Mediator::get_instance()->zoom), QSize(bg1_image.width()*Mediator::get_instance()->zoom, bg1_image.height()*Mediator::get_instance()->zoom));
                painter.drawPixmap(pos_dest, bg1_image, pos_source);
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
        QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        for (i=1; i<MAP_W; i++) {
            pos = i*TILESIZE*Mediator::get_instance()->zoom-1;
            //QLineF line(0, 800, 16, 800);
            // linhas horizontais
            line = QLineF(pos, 0, pos, MAP_H*TILESIZE*Mediator::get_instance()->zoom-1);
            if (i % 20 == 0) {
                painter.setPen(pen_red);
            } else {
                painter.setPen(pen);
            }
            painter.drawLine(line);
        }
        painter.setPen(pen);
        for (i=1; i<MAP_H; i++) {
            pos = i*TILESIZE*Mediator::get_instance()->zoom-1;
            //QLineF line(0, 800, 16, 800);
            // linhas verticais
            line = QLineF(0, pos, MAP_W*TILESIZE*Mediator::get_instance()->zoom-1, pos);
            painter.drawLine(line);
        }
    }


    // DRAW LINKS //
    if (Mediator::get_instance()->show_teleporters_flag == true) {
        // draw links
        int link_type = Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].type;
        //printf("editoMode: %d, EDITMODE_NORMAL: %d, editTool: %d, EDITMODE_LINK_DEST: %d\n", Mediator::get_instance()->editMode, EDITMODE_NORMAL, Mediator::get_instance()->editTool, EDITMODE_LINK_DEST);
        if (Mediator::get_instance()->editMode == EDITMODE_LINK && (Mediator::get_instance()->editTool == EDITMODE_LINK_DEST || Mediator::get_instance()->editTool == EDITMODE_LINK)) {
            for (int i=0; i<STAGE_MAX_LINKS; i++) {
                // link is from and to the same map, draw in a different color
                if (Mediator::get_instance()->currentMap == Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin) {
                    // translucid blue rectangle
                    if (link_type == LINK_TELEPORTER || link_type == LINK_TELEPORT_LEFT_LOCK || link_type == LINK_TELEPORT_RIGHT_LOCK || link_type == LINK_FADE_TELEPORT) {
                        painter.setBrush(QColor(0, 255, 0, 180));
                    } else {
                        // if origin and destiny are the same map, use yellow
                        if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin == Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny) {
                            painter.setBrush(QColor(231, 209, 58, 180));
                        } else {
                            painter.setBrush(QColor(0, 0, 255, 180));
                        }
                    }
                    painter.drawRect(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.y *TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom*Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].size, TILESIZE*Mediator::get_instance()->zoom);
                    // blue border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 0, 255, 255));
                    painter.drawRect(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom*Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].size, TILESIZE*Mediator::get_instance()->zoom);
                    painter.setPen(QColor(255, 255, 255, 255));
                    painter.drawText(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.x*TILESIZE*Mediator::get_instance()->zoom, (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.y+1)*TILESIZE*Mediator::get_instance()->zoom, QString::number(i));
                }
                if (Mediator::get_instance()->currentMap == Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny) {
                    // translucid cyan rectangle
                    if (link_type == LINK_TELEPORTER || link_type == LINK_TELEPORT_LEFT_LOCK || link_type == LINK_TELEPORT_RIGHT_LOCK || link_type == LINK_FADE_TELEPORT) {
                        painter.setBrush(QColor(60, 160, 60, 180));
                    } else {
                        // if origin and destiny are the same map, use yellow
                        if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin == Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny) {
                            painter.setBrush(QColor(184, 171, 84, 180));
                        } else {
                            painter.setBrush(QColor(0, 255, 255, 180));
                        }
                    }
                    painter.drawRect(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom*Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].size, TILESIZE*Mediator::get_instance()->zoom);
                    // cyan border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 255, 255, 255));
                    painter.drawRect(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom*Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].size, TILESIZE*Mediator::get_instance()->zoom);
                    painter.setPen(QColor(0, 0, 0, 255));
                    painter.drawText(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.x*TILESIZE*Mediator::get_instance()->zoom, (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.y+1)*TILESIZE*Mediator::get_instance()->zoom, QString::number(i));
                }
            }
        }
    }





    if (Mediator::get_instance()->show_objects_flag == true) {
        /// draw objects
        //std::cout << "################### START LOOP" << std::endl;
        for (int i=0; i<Mediator::get_instance()->maps_data_object_list.size(); i++) {
            int obj_stage_id = (int)Mediator::get_instance()->maps_data_object_list[i].stage_id;
            int obj_map_id = (int)Mediator::get_instance()->maps_data_object_list[i].map_id;
            if (obj_stage_id != Mediator::get_instance()->currentStage || obj_map_id != Mediator::get_instance()->currentMap) {
                continue;
            }


            if (Mediator::get_instance()->maps_data_object_list[i].difficulty_mode == DIFFICULTY_MODE_GREATER && Mediator::get_instance()->maps_data_object_list[i].difficulty_level > Mediator::get_instance()->currentDifficulty) {
                continue; // only show enemies with equal or lower difficulty
            } else if (Mediator::get_instance()->maps_data_object_list[i].difficulty_mode == DIFFICULTY_MODE_EQUAL && Mediator::get_instance()->maps_data_object_list[i].difficulty_level != Mediator::get_instance()->currentDifficulty) {
                continue;
            }

            int obj_id = (int)Mediator::get_instance()->maps_data_object_list[i].id_object;

            //std::cout << "OBJ[" << i << "][" << Mediator::get_instance()->object_list.at(obj_id).name << "].stage[" << (int)Mediator::get_instance()->maps_data_object_list[i].stage_id << "].map[" << (int)Mediator::get_instance()->maps_data_object_list[i].map_id << "], currentStage[" << Mediator::get_instance()->currentStage << "], currentMap[" << Mediator::get_instance()->currentMap << "]" << std::endl;


            if (obj_id != -1) {
                //if (obj_id == 20) { std::cout << "************************** paintEvent - draw_objects[" << i << "].id: " << obj_id << std::endl; }
                std::string filename = FILEPATH + "/images/sprites/objects/" + Mediator::get_instance()->object_list.at(obj_id).graphic_filename;
                QPixmap temp_image(filename.c_str());
                if (temp_image.isNull()) {
                    std::cout << "****************** Could not load file '" << filename.c_str() << "'" << std::endl;
                    painter.setBrush(QColor(255, 255, 255, 180));
                    painter.drawRect(Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                } else {
                    int obj_type = Mediator::get_instance()->object_list.at(obj_id).type;
                    int obj_direction = Mediator::get_instance()->maps_data_object_list[i].direction;
                    if (obj_type == OBJ_RAY_HORIZONTAL) {
                        int graphic_pos_x = 0;
                        int graphic_pos_y = Mediator::get_instance()->object_list.at(obj_id).size.height;
                        if (obj_direction == ANIM_DIRECTION_LEFT) {
                            graphic_pos_x = Mediator::get_instance()->object_list.at(obj_id).size.width - TILESIZE;
                            graphic_pos_y = 0;
                        }
                        //std::cout << "OBJ_RAY_HORIZONTAL - obj_direction: " << obj_direction << ", graphic_pos_x: " << graphic_pos_x << ", graphic_pos_y: " << graphic_pos_y << std::endl;
                        QRectF target(QPoint(Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                        QRectF source(QPoint(graphic_pos_x, graphic_pos_y), QSize(TILESIZE, Mediator::get_instance()->object_list.at(obj_id).size.height));
                        painter.drawPixmap(target, temp_image, source);
                    } else if (obj_type == OBJ_RAY_VERTICAL) {
                        int graphic_pos_y = Mediator::get_instance()->object_list.at(obj_id).size.height - TILESIZE;
                        //std::cout << "OBJ_RAY_HORIZONTAL - graphic_pos_y: " << graphic_pos_y << std::endl;
                        QRectF target(QPoint(Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                        QRectF source(QPoint(0, graphic_pos_y), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                        painter.drawPixmap(target, temp_image, source);
                    } else if (obj_type == OBJ_DEATHRAY_HORIZONTAL && obj_direction == ANIM_DIRECTION_LEFT) {
                        QRectF target(QPoint(Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom - (Mediator::get_instance()->object_list.at(obj_id).size.width-TILESIZE)*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                        QRectF source(QPoint(0, 0), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                        painter.drawPixmap(target, temp_image, source);
                    } else {
                        QRectF target(QPoint(Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width*Mediator::get_instance()->zoom, Mediator::get_instance()->object_list.at(obj_id).size.height*Mediator::get_instance()->zoom));
                        QRectF source;
                        if (obj_direction == ANIM_DIRECTION_RIGHT && temp_image.height() >= Mediator::get_instance()->object_list.at(obj_id).size.height) {
                            source = QRectF(QPoint(0, Mediator::get_instance()->object_list.at(obj_id).size.height), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                        } else {
                            source = QRectF(QPoint(0, 0), QSize(Mediator::get_instance()->object_list.at(obj_id).size.width, Mediator::get_instance()->object_list.at(obj_id).size.height));
                        }
                        painter.drawPixmap(target, temp_image, source);
                    }
                }
                // draw object-teleporter origin
                if (Mediator::get_instance()->maps_data_object_list[i].map_dest != -1) {
                    int obj_w = Mediator::get_instance()->object_list.at(obj_id).size.width;
                    int obj_h = Mediator::get_instance()->object_list.at(obj_id).size.height;
                    //std::cout << "OBJ.w: " << obj_w << ", obj_h: " << obj_h << std::endl;
                    int dest_x = Mediator::get_instance()->maps_data_object_list[i].start_point.x*TILESIZE*Mediator::get_instance()->zoom + (obj_w/2)*Mediator::get_instance()->zoom - TILESIZE/2;
                    int dest_y = Mediator::get_instance()->maps_data_object_list[i].start_point.y*TILESIZE*Mediator::get_instance()->zoom + (obj_h/2)*Mediator::get_instance()->zoom - TILESIZE/2;
                    //std::cout << "DRAW OBJECT TELEPORTER ORIGIN[" << i << "] - map: " << (int)Mediator::get_instance()->maps_data_object_list[i].map_dest  << ", x: " << Mediator::get_instance()->maps_data_object_list[i].start_point.x << ", y: " << Mediator::get_instance()->maps_data_object_list[i].start_point.y << std::endl;
                    if (Mediator::get_instance()->object_list.at(obj_id).type == OBJ_FINAL_BOSS_TELEPORTER) {
                        painter.setBrush(QColor(160, 60, 60, 180));
                    } else {
                        painter.setBrush(QColor(60, 160, 60, 180));
                    }
                    painter.drawEllipse(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    // cyan border
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QColor(0, 255, 255, 255));
                    painter.drawEllipse(dest_x, dest_y, TILESIZE*Mediator::get_instance()->zoom, TILESIZE*Mediator::get_instance()->zoom);
                    painter.setPen(QColor(0, 0, 0, 255));
                    painter.drawText(dest_x+3, dest_y+TILESIZE-3, QString::number(i));
                }
            }
        }


        // DRAW OBJECT TELEPORT CIRCLES
        for (int k=0; k<FS_STAGE_MAX_MAPS; k++) {
            for (int m=0; m<Mediator::get_instance()->maps_data_object_list.size(); m++) {
                if (Mediator::get_instance()->maps_data_object_list[m].stage_id != Mediator::get_instance()->currentStage) {
                    continue; // only show enemies from current stage/map
                }
                CURRENT_FILE_FORMAT::file_map_object_v2 map_obj = Mediator::get_instance()->maps_data_object_list[m];
                int obj_id = map_obj.id_object;
                if (obj_id == -1 || obj_id >= Mediator::get_instance()->object_list.size()) { // old format style or invalid object
                    continue;
                }

                // draw teleport destiny links
                //std::cout << "OBJ[" << i << "].map_dest: " << (int)map_obj.map_dest << ", currentMap: " << k << std::endl;
                if (map_obj.map_dest == Mediator::get_instance()->currentMap) {

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
                QRectF pos_dest(QPoint(k*fg_layer__image.width()*Mediator::get_instance()->zoom, Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].adjust_y*Mediator::get_instance()->zoom), QSize(fg_layer__image.width()*Mediator::get_instance()->zoom, fg_layer__image.height()*Mediator::get_instance()->zoom));
                painter.setOpacity(fg_opacity);
                painter.drawPixmap(pos_dest, fg_layer__image, pos_source);
                painter.setOpacity(1);
            }
        }
    }


    // === draw selection === //
    if (Mediator::get_instance()->editMode == EDITMODE_SELECT) {
        std::cout << "PAINT::EDITMODE_SELECT" << std::endl;
        painter.setBrush(QColor(0, 0, 255, 180));
        painter.drawRect(selection_start_x*TILESIZE*Mediator::get_instance()->zoom, selection_start_y*TILESIZE*Mediator::get_instance()->zoom, abs(selection_current_x-selection_start_x)*TILESIZE*Mediator::get_instance()->zoom, abs(selection_current_y-selection_start_y)*TILESIZE*Mediator::get_instance()->zoom);
    }


    QSize resizeMe(MAP_W*TILESIZE*Mediator::get_instance()->zoom, MAP_H*TILESIZE*Mediator::get_instance()->zoom);
    this->resize(resizeMe);
    myParent->adjustSize();

}

void EditorArea::mouseMoveEvent(QMouseEvent *event) {
	QPoint pnt = event->pos();

    if (Mediator::get_instance()->editMode != EDITMODE_SELECT) {
        // forces "click" when moving
        if (editor_selectedTileX != pnt.x()/(TILESIZE*Mediator::get_instance()->zoom) || editor_selectedTileY != pnt.y()/(TILESIZE*Mediator::get_instance()->zoom)) {
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
}



void EditorArea::mousePressEvent(QMouseEvent *event) {
    if (mouse_released == false && (Mediator::get_instance()->editTool == EDITMODE_LINK || Mediator::get_instance()->editTool == EDITMODE_LINK_DEST || Mediator::get_instance()->editMode == EDITMODE_NPC || Mediator::get_instance()->editMode == EDITMODE_OBJECT || Mediator::get_instance()->editMode == EDITMODE_SET_BOSS || Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE)) {
        std::cout << "EDITORAREA::mousePressEvent - IGNORED" << std::endl;
		return;
	}

    std::cout << "EDITORAREA::mousePressEvent - EXECUTE" << std::endl;

    QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/(TILESIZE*Mediator::get_instance()->zoom);
    editor_selectedTileY = pnt.y()/(TILESIZE*Mediator::get_instance()->zoom);

    if (Mediator::get_instance()->editMode == EDITMODE_NORMAL || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE) {
        // V5-FILE //
        if (Mediator::get_instance()->file_v5_map_tile_map.find(Mediator::get_instance()->file_v5_selected_map) != Mediator::get_instance()->file_v5_map_tile_map.end()) {

            int map_w = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w;
            int map_h = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h;
            int n = editor_selectedTileY*map_w + editor_selectedTileX;
            std::cout << "mousePressEvent.DEBUG#1 - map_w[" << map_w << "], map_h[" << map_h << "], n[" << n << "]" << std::endl;

            if (Mediator::get_instance()->editTool == EDITMODE_NORMAL || Mediator::get_instance()->editTool == EDITMODE_ERASER || Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE || Mediator::get_instance()->editMode == EDITMODE_SLOPE_TILE) {
                std::cout << "mousePressEvent.DEBUG#2" << std::endl;
                int valueType = TILE_TYPE_SOLID;
                int valueX = Mediator::get_instance()->getPalleteX();
                int valueY = Mediator::get_instance()->getPalleteY();

                if (Mediator::get_instance()->editMode == EDITMODE_ANIM_TILE) {
                    valueX = Mediator::get_instance()->selectedAnimTileset;
                    valueY = 0;
                    valueType = TILE_TYPE_ANIM;
                } else if (Mediator::get_instance()->layerLevel == 1 && Mediator::get_instance()->editMode == EDITMODE_SLOPE_TILE) {
                    valueType = TILE_TYPE_SLOPE;
                }
                if (Mediator::get_instance()->editTool == EDITMODE_ERASER) {
                    valueX = -1;
                    valueY = -1;
                }

                if (Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).size() > 0) {
                    if (Mediator::get_instance()->layerLevel == 1) {
                        std::cout << "############ set tile[" << n << "], at[" << editor_selectedTileX << "][" << editor_selectedTileY << "] with [" << valueX << "][" << valueY << "][" << valueType << "]" << std::endl;
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_underlay.x = valueX;
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_underlay.y = valueY;
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_underlay.type = valueType;
                    } else if (Mediator::get_instance()->layerLevel == 3) {
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_overlay.x = valueX;
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_overlay.y = valueY;
                        Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).tile_overlay.type = valueType;
                    }
                }
            } else if (Mediator::get_instance()->editTool == EDITMODE_LOCK) {
                Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).at(n).locked  = Mediator::get_instance()->terrainType;
            } else {
                std::cout << "mousePressEvent.DEBUG -- ELSE#1" << std::endl;
            }
        } else {
            std::cout << "mousePressEvent.DEBUG -- ELSE#2" << std::endl;
        }




    } else if (Mediator::get_instance()->editMode == EDITMODE_LINK) {
		// first click on origin link
        if (Mediator::get_instance()->editTool == EDITMODE_LINK && tempX == -1) {
			// checks if a link in this position already exits to remove it
			bool removed_link = false;
			for (int i=0; i<STAGE_MAX_LINKS; i++) {
                if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin == Mediator::get_instance()->currentMap) {
                    if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.x == editor_selectedTileX && Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_origin.y == editor_selectedTileY) {
                        Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny = -1;
                        Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin = -1;
						std::cout << "######## -> editorArea::mousePress - removed link 1" << std::endl;
						removed_link = true;
						break;
					}
                } else if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny == Mediator::get_instance()->currentMap) {
                    if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.x == editor_selectedTileX && Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].pos_destiny.y == editor_selectedTileY) {
                        Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny = -1;
                        Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin = -1;
						std::cout << "######## -> editorArea::mousePress - removed link 2" << std::endl;
						removed_link = true;
						break;
					}
				}
			}
			if (removed_link == false) {
				// check if there is a slot free
				int link_n = -1;
				for (int i=0; i<STAGE_MAX_LINKS; i++) {
                    std::cout << ">> DEBUG - links[" << i << "].id_map_destiny: " << Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny << ", id_map_origin: " << Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin << std::endl;
                    if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny == -1 && Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin == -1) {
						link_n = i;
						break;
					}
				}
				if (link_n == -1) {
					QMessageBox msgBox;
					msgBox.setText("########### -> Error: there is no slot free to add a new link.");
					msgBox.exec();
					return;
				}
                // last boss room doesen't need destination
                std::cout << "########### -> editorArea::mousePress - adding link ORIGIN at slot[" << link_n << "] - PART 1" << std::endl;
                tempX = editor_selectedTileX;
                tempY = editor_selectedTileY;
			}
        } else if (Mediator::get_instance()->editTool == EDITMODE_LINK_DEST) {
			std::cout << "########### -> editorArea::mousePress - adding link - PART 3" << std::endl;
			int link_n = -1;
			for (int i=0; i<STAGE_MAX_LINKS; i++) {
                if (Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_destiny == -1 && Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[i].id_map_origin == -1) {
					link_n = i;
					break;
				}
			}
			if (link_n == -1) {
				QMessageBox msgBox;
				msgBox.setText("Error: there is no slot free to add a new link.");
				msgBox.exec();
				return;
			}
			std::cout << "########### -> editorArea::mousePress - adding link DESTINY at slot[" << link_n << "] - PART 3" << std::endl;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].id_map_origin = link_map_origin;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].pos_origin.x = link_pos_x;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].pos_origin.y = link_pos_y;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].size = link_size;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].id_map_destiny = Mediator::get_instance()->currentMap;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].pos_destiny.x = editor_selectedTileX;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].pos_destiny.y = editor_selectedTileY;
			/// @TODO: this must come from editor properties
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].bidirecional = Mediator::get_instance()->link_bidi;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].is_door = Mediator::get_instance()->link_is_door;
            Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].links[link_n].type = Mediator::get_instance()->link_type;

            Mediator::get_instance()->editTool = EDITMODE_LINK;
			repaint();
            QApplication::setOverrideCursor(Qt::ArrowCursor);
			return;
		}



    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_BOSS) {
        std::cout << ">> EditorArea::mousePressEvent - EDITMODE_SET_BOSS" << std::endl;
        // search if there is an existing NPC in ths position, and if yes, set as boss
        for (int i=0; i<Mediator::get_instance()->maps_data_npc_list.size(); i++) {
            // only show enemies from current stage/map
            if (Mediator::get_instance()->maps_data_npc_list[i].stage_id != Mediator::get_instance()->currentStage || Mediator::get_instance()->maps_data_npc_list[i].map_id != Mediator::get_instance()->currentMap) {
                continue;
            }
            int npc_id = Mediator::get_instance()->maps_data_npc_list[i].id_npc;
            if (npc_id != -1 && Mediator::get_instance()->maps_data_npc_list[i].start_point.x == editor_selectedTileX && Mediator::get_instance()->maps_data_npc_list[i].start_point.y == editor_selectedTileY) {
                Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].boss.id_npc = Mediator::get_instance()->maps_data_npc_list[i].id_npc;
                sprintf(Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].boss.name, "%s", Mediator::get_instance()->enemy_list.at(Mediator::get_instance()->maps_data_npc_list[i].id_npc).name);
                break;
            }
        }


    } else if (Mediator::get_instance()->editMode == EDITMODE_SET_SUBBOSS) {
        std::cout << ">> EditorArea::mousePressEvent - EDITMODE_SET_SUBBOSS" << std::endl;
        // search if there is an existing NPC in ths position, and if yes, set as sub-boss
        for (int i=0; i<Mediator::get_instance()->maps_data_npc_list.size(); i++) {
            if (Mediator::get_instance()->maps_data_npc_list[i].stage_id != Mediator::get_instance()->currentStage || Mediator::get_instance()->maps_data_npc_list[i].map_id != Mediator::get_instance()->currentMap) {
                continue; // only show enemies from current stage/map
            }
            if (Mediator::get_instance()->maps_data_npc_list[i].id_npc != -1 && Mediator::get_instance()->maps_data_npc_list[i].start_point.x == editor_selectedTileX && Mediator::get_instance()->maps_data_npc_list[i].start_point.y == editor_selectedTileY) {
                std::cout << ">> EditorArea::mousePressEvent - EDITMODE_SET_SUBBOSS - FOUND NPC" << std::endl;
                Mediator::get_instance()->enemy_list.at(Mediator::get_instance()->maps_data_npc_list[i].id_npc).is_sub_boss = !Mediator::get_instance()->enemy_list.at(Mediator::get_instance()->maps_data_npc_list[i].id_npc).is_sub_boss;
                break;
            }
        }


    } else if (Mediator::get_instance()->editMode == EDITMODE_NPC) {
		printf(">> EditorArea::mousePressEvent - EDITMODE_NPC\n");

		int found_npc = -1;
        int currentMap = Mediator::get_instance()->file_v5_selected_map;
        if (Mediator::get_instance()->file_v5_map_npc_map.find(currentMap) == Mediator::get_instance()->file_v5_map_npc_map.end()) {
            Mediator::get_instance()->file_v5_map_npc_map.insert(std::pair<int, std::vector<file_v5_map_npc>>(currentMap, std::vector<file_v5_map_npc>()));
        }

		// search if there is an existing NPC in ths position, and if yes, remove it
        for (int i=0; i<Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).size(); i++) {
            if (Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).id_npc != -1 && Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.x == editor_selectedTileX && Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).at(i).start_point.y == editor_selectedTileY) {
				found_npc = i;
				break;
			}
		}

        if (Mediator::get_instance()->editTool == EDITMODE_ERASER && found_npc != -1) {
			std::cout << "remove npc - slot: " << found_npc << std::endl;
            Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).at(found_npc).id_npc = -1;
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_npc == -1 && Mediator::get_instance()->selectedNPC != -1) {
            file_v5_map_npc new_npc;
            new_npc.id_npc = Mediator::get_instance()->selectedNPC;
            new_npc.start_point.x = editor_selectedTileX;
            new_npc.start_point.y = editor_selectedTileY;
            new_npc.direction = Mediator::get_instance()->npc_direction;
            new_npc.difficulty_level = Mediator::get_instance()->currentDifficulty;
            new_npc.difficulty_mode = Mediator::get_instance()->currentDifficultyMode;
            Mediator::get_instance()->file_v5_map_npc_map.at(currentMap).push_back(new_npc);
            std::cout << "EditorArea::mousePressEvent - ADDED NPC in map[" << currentMap << "], pos[" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;
            repaint();
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_npc != -1 && Mediator::get_instance()->selectedNPC != -1) {
			printf(">> EditorArea::mousePressEvent - Adding NPC - place already taken\n");
		}



    } else if (Mediator::get_instance()->editMode == EDITMODE_OBJECT) {
        std::cout << ">> EditorArea::mousePressEvent - EDITMODE_OBJECT" << std::endl;
		int found_object = -1;

		// search if there is an existing object in ths position, and if yes, remove it
        for (int m=0; m<Mediator::get_instance()->maps_data_object_list.size(); m++) {
            if (Mediator::get_instance()->maps_data_object_list[m].stage_id != Mediator::get_instance()->currentStage || Mediator::get_instance()->maps_data_object_list[m].map_id != Mediator::get_instance()->currentMap) {
                continue; // only show enemies from current stage/map
            }

            if (Mediator::get_instance()->maps_data_object_list[m].id_object != -1 && Mediator::get_instance()->maps_data_object_list[m].start_point.x == editor_selectedTileX && Mediator::get_instance()->maps_data_object_list[m].start_point.y == editor_selectedTileY) {
                found_object = m;
				break;
			}
		}
		// clean old-format trash
        for (int m=0; m<Mediator::get_instance()->maps_data_object_list.size(); m++) {
            if (Mediator::get_instance()->maps_data_object_list[m].stage_id != Mediator::get_instance()->currentStage || Mediator::get_instance()->maps_data_object_list[m].map_id != Mediator::get_instance()->currentMap) {
                continue; // only show enemies from current stage/map
            }

            if (Mediator::get_instance()->maps_data_object_list[m].start_point.x == -1 && Mediator::get_instance()->maps_data_object_list[m].start_point.y == -1) {
                std::cout << "reseting free-slot - m: " << m << ", id: " << Mediator::get_instance()->maps_data_object_list[m].id_object << ", x: " << Mediator::get_instance()->maps_data_object_list[m].start_point.x << ", y: " << Mediator::get_instance()->maps_data_object_list[m].start_point.y << std::endl;
                Mediator::get_instance()->maps_data_object_list[m].id_object = -1;
			}
		}

        if (Mediator::get_instance()->editTool == EDITMODE_ERASER && found_object != -1) {
			std::cout << "remove object - slot: " << found_object << std::endl;
            Mediator::get_instance()->maps_data_object_list.erase(Mediator::get_instance()->maps_data_object_list.begin()+found_object);
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_object == -1 && Mediator::get_instance()->selectedNPC != -1) {
                CURRENT_FILE_FORMAT::file_map_object_v2 new_obj;
                new_obj.id_object = Mediator::get_instance()->selectedNPC;
                new_obj.start_point.x = editor_selectedTileX;
                new_obj.start_point.y = editor_selectedTileY;
                new_obj.direction = Mediator::get_instance()->object_direction;
                new_obj.stage_id = Mediator::get_instance()->currentStage;
                new_obj.map_id = Mediator::get_instance()->currentMap;
                new_obj.difficulty_level = Mediator::get_instance()->currentDifficulty;
                new_obj.difficulty_mode = Mediator::get_instance()->currentDifficultyMode;
                editor_selected_object_pos_map = Mediator::get_instance()->currentMap;
                // se item é teleportador, deve entrar no modo de colocar link de object
                int obj_type = Mediator::get_instance()->object_list.at(Mediator::get_instance()->selectedNPC).type;
                if (obj_type == OBJ_BOSS_TELEPORTER || obj_type == OBJ_FINAL_BOSS_TELEPORTER || obj_type == OBJ_PLATFORM_TELEPORTER || obj_type == OBJ_STAGE_BOSS_TELEPORTER) {
                    editor_selected_object_pos = Mediator::get_instance()->maps_data_object_list.size();
                    std::cout << "SET editor_selected_object_pos: " << editor_selected_object_pos << std::endl;
                    Mediator::get_instance()->editTool = EDITMODE_OBJECT_LINK_PLACING;
                    QApplication::setOverrideCursor(Qt::CrossCursor);
                    // @TODO - desabilita todos os modos, não pode sair no meio de object-link-placing
                    // @TODO - desabilitar mudar de estágio também (e isso tem que ser feito no link tb)
                } else {
                    new_obj.map_dest = -1;
                }
                Mediator::get_instance()->maps_data_object_list.push_back(new_obj);
                repaint();
        } else if (Mediator::get_instance()->editTool == EDITMODE_NORMAL && found_object != -1 && Mediator::get_instance()->selectedNPC != -1) {
			printf(">> EditorArea::mousePressEvent - Adding object - place already taken\n");
        } else if (Mediator::get_instance()->editTool == EDITMODE_OBJECT_LINK_PLACING) {
            std::cout << ">> EditorArea::mousePressEvent - EDITMODE_OBJECT_LINK_PLACING" << std::endl;
            std::cout << "USE editor_selected_object_pos_map: " << editor_selected_object_pos_map << ", editor_selected_object_pos: " << editor_selected_object_pos << ", editor_selectedTileX: " << editor_selectedTileX << ", editor_selectedTileY: " << editor_selectedTileY <<    std::endl;
            Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].link_dest.x = editor_selectedTileX;
            Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].link_dest.y = editor_selectedTileY;
            Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].map_dest = Mediator::get_instance()->currentMap;
            Mediator::get_instance()->editTool = EDITMODE_NORMAL;
            std::cout << "[TELEPORTER] SET map: " << (int)Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].map_dest << ", x: " << (int)Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].link_dest.x << ", y: " << (int)Mediator::get_instance()->maps_data_object_list[editor_selected_object_pos].link_dest.y << std::endl;
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        } else {
            std::cout << ">> EditorArea::mousePressEvent - EDITMODE UNKNOWN!!!!" << std::endl;
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
        std::cout << "EDITMODE_PASTE, matrix size: " << selection_matrix.size() << std::endl;
        if (selection_matrix.size() > 0) {
            for (int i=0; i<selection_matrix.size(); i++) {
                std::cout << "EDITMODE_PASTE[" << i << "].size: " << selection_matrix.at(i).size() << std::endl;
                for (int j=0; j<selection_matrix.at(i).size(); j++) {
                    st_tile_point tile_point = selection_matrix.at(i).at(j);
                    int x = editor_selectedTileX+i;
                    int y = editor_selectedTileY+j;
                    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[x][y].tile1 = tile_point.tile1;
                    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[x][y].tile3 = tile_point.tile3;
                    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[x][y].locked = tile_point.locked;

                    //Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[x][y].locked
                }
            }
            repaint();
        }
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

        for (int i=start_x; i<end_x; i++) {
            std::vector<st_tile_point> temp;
            for (int j=start_y; j<end_y; j++) {
                temp.push_back(st_tile_point(
                     Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1,
                     Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3,
                     Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].locked
                   ));
            }
            selection_matrix.push_back(temp);
        }
    } else if (Mediator::get_instance()->editTool == EDITMODE_LINK && tempX != -1) {// && Mediator::get_instance()->link_type != LINK_FINAL_BOSS_ROOM) {
        std::cout << "########### -> editorArea::mouseReleaseEvent - adding link - PART 2" << std::endl;
		// TODO: add link
        Mediator::get_instance()->editTool = EDITMODE_LINK_DEST;
        QApplication::setOverrideCursor(Qt::CrossCursor);

        link_map_origin = Mediator::get_instance()->currentMap;
		link_pos_x = tempX;
		link_pos_y = tempY;


		int tempSize = abs(editor_selectedTileX - tempX);
		if (tempSize < 1 || tempSize > 20) {
			link_size = 1;
		} else {
            link_size = abs(editor_selectedTileX - tempX);
		}
        //printf(">>>>>>>> added link in map: %d, pos: (%d, %d) with size: %d <<<<<<<<<<\n", link_map_origin, link_pos_x, link_pos_y, link_size);

		tempX = -1;
		tempY = -1;
		repaint();

    }
	mouse_released = true;
}

void EditorArea::fill_area() {
	int i, j;
	int filled1=0;
	int filled2=0;
	int originalX, originalY;
    short int searchX, searchY;

    if (Mediator::get_instance()->layerLevel == 1) {
        originalX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[editor_selectedTileX][editor_selectedTileY].tile1.x;
        originalY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[editor_selectedTileX][editor_selectedTileY].tile1.y;
    } else if (Mediator::get_instance()->layerLevel == 3) {
        originalX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[editor_selectedTileX][editor_selectedTileY].tile3.x;
        originalY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[editor_selectedTileX][editor_selectedTileY].tile3.y;
	}
	// área a esquerda
	for (i=editor_selectedTileX; i>=0; i--) {
		// área acima
		filled1 = 0;
		filled2 = 0;
		for (j=editor_selectedTileY; j>=0; j--) {
            if (Mediator::get_instance()->layerLevel == 1) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.y;
            } else if (Mediator::get_instance()->layerLevel == 3) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.y;
			}
			//printf("DEBUG.EditorArea::i: %d, j: %d, fill_area - tile.x: %d, tile;y: %d, originalX: %d, originalY: %d\n", i, j, map.tiles[i][j].tile1.x, map.tiles[i][j].tile1.y, originalX, originalY);
            if (searchX == originalX && searchY == originalY) {
                searchX = Mediator::get_instance()->getPalleteX();
                searchY = Mediator::get_instance()->getPalleteY();
				filled1 = 1;
			} else {
				printf("1.not filling\n");
				break;
			}
		}
		// área abaixo
		for (j=editor_selectedTileY+1; j<MAP_H; j++) {
            if (Mediator::get_instance()->layerLevel == 1) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.y;
            } else if (Mediator::get_instance()->layerLevel == 3) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.y;
			}
            if (searchX == originalX && searchY == originalY) {
                searchX = Mediator::get_instance()->getPalleteX();
                searchY = Mediator::get_instance()->getPalleteY();
				filled2 = 1;
			} else {
				printf("2.not filling\n");
				break;
			}
		}
		if (filled1 == 0 && filled2 == 0) {
			break;
		}

	}
	// area a direita
	for (i=editor_selectedTileX+1; i<MAP_W; i++) {
		filled1 = 0;
		filled2 = 0;
		for (j=editor_selectedTileY; j>=0; j--) {
			//printf("DEBUG.EditorArea::i: %d, j: %d, fill_area - tile.x: %d, tile;y: %d, originalX: %d, originalY: %d\n", i, j, map.tiles[i][j].tile1.x, map.tiles[i][j].tile1.y, originalX, originalY);
            if (Mediator::get_instance()->layerLevel == 1) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.y;
            } else if (Mediator::get_instance()->layerLevel == 3) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.y;
			}
            if (searchX == originalX && searchY == originalY) {
                searchX = Mediator::get_instance()->getPalleteX();
                searchY = Mediator::get_instance()->getPalleteY();
				filled1 = 1;
			} else {
				printf("3.not filling\n");
				break;
			}
		}
		// área abaixo
		for (j=editor_selectedTileY+1; j<MAP_H; j++) {
            if (Mediator::get_instance()->layerLevel == 1) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile1.y;
            } else if (Mediator::get_instance()->layerLevel == 3) {
                searchX = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.x;
                searchY = Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].tiles[i][j].tile3.y;
			}
            if (searchX == originalX && searchY == originalY) {
                searchX = Mediator::get_instance()->getPalleteX();
                searchY = Mediator::get_instance()->getPalleteY();
				filled2 = 1;
			} else {
				printf("4.not filling\n");
				break;
			}
		}
		if (filled1 == 0 || filled2 == 0) {
			break;
		}
	}
}







