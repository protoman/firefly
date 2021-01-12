#include "worldmapeditorArea.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QResource>


#include "defines.h"
#include "data/shareddata.h"

#define TILE_SHOW_SIZE 16

worldMapEditorArea::worldMapEditorArea(QWidget *parent) : QWidget(parent)
{
    QString terrainIcon;
    QResource::registerResource("resources/icons/icons.qrc");
    terrainIcon = QString::fromUtf8(":/toolbar_icons/arrow_down.png");
    arrow_down = QPixmap(terrainIcon);

    terrainIcon = QString::fromUtf8(":/toolbar_icons/arrow_up.png");
    arrow_up = QPixmap(terrainIcon);

    terrainIcon = QString::fromUtf8(":/toolbar_icons/arrow_left.png");
    arrow_left = QPixmap(terrainIcon);

    terrainIcon = QString::fromUtf8(":/toolbar_icons/arrow_right.png");
    arrow_right = QPixmap(terrainIcon);

}

void worldMapEditorArea::setCurrentArea(int area_n)
{
    currentArea = area_n;
}

void worldMapEditorArea::setCurrentMap(int map_n)
{
    currentMap = map_n;
}

void worldMapEditorArea::set_edit_mode(e_AREA_EDIT_MODE mode)
{
    edit_mode = mode;
    if (edit_mode == AREA_EDIT_MODE_HLINK) {
        QApplication::setOverrideCursor(Qt::SizeHorCursor);
    } else if (edit_mode == AREA_EDIT_MODE_VLINK) {
        QApplication::setOverrideCursor(Qt::SizeVerCursor);
    } else {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }
}

void worldMapEditorArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QLineF line;

    /*
    // draw map points
    for (int i=0; i<FILE_AREA_W; i++) {
        for (int j=0; j<FILE_AREA_H; j++) {
            // TODO - make current make different color
            //std::cout << "area[" << i << "][" << j << "]: [" << SharedData::get_instance()->v6_level_list.at(currentArea).rooms[i][j].area_n << "]" << std::endl;
            if (SharedData::get_instance()->v6_level_list.size() > currentArea && SharedData::get_instance()->v6_level_list.at(currentArea).rooms[i][j].area_n != -1) {
                file_v6_level level = SharedData::get_instance()->v6_level_list.at(currentArea);
                file_v6_room room = level.rooms[i][j];
                if (SharedData::get_instance()->v6_level_list.at(currentArea).rooms[i][j].area_n == currentMap) {
                    painter.setBrush(QColor(0, 0, 255, 180));
                    painter.setPen(QColor(0, 0, 180, 255));
                } else {
                    painter.setBrush(QColor(99, 173, 230, 180));
                    painter.setPen(QColor(99, 117, 230, 255));
                }
                painter.drawRect(i*TILE_SHOW_SIZE, j*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);
            }
        }
    }

    // draw map-link point1
    if (link_p1.x != -1 && link_p1.y != -1) {
        painter.setBrush(QColor(255, 0, 0, 180));
        painter.setPen(QColor(180, 0, 0, 255));
        painter.drawRect(link_p1.x*TILE_SHOW_SIZE, link_p1.y*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);

    }

    // draw links
    if (SharedData::get_instance()->file_v5_area_link_map.find(currentArea) != SharedData::get_instance()->file_v5_area_link_map.end()) {
        std::cout << "MAP-LINKS #1"<< std::endl;
        for (int i=0; i<SharedData::get_instance()->file_v5_area_link_map.at(currentArea).size(); i++) {
            std::cout << "MAP-LINKS #2 ["<< i << "]" << std::endl;
            painter.setBrush(QColor(0, 255, 0, 180));
            painter.setPen(QColor(0, 180, 0, 255));
            struct_file_v5_area_link &link_data = SharedData::get_instance()->file_v5_area_link_map.at(currentArea).at(i);

            std::cout << "p1.x[" << link_data.p1.x << "], p1.y[" << link_data.p1.y << "], p2.x[" << link_data.p2.x << "], p2.y[" << link_data.p2.y << "]" << std::endl;

            //painter.drawRect(link_data.p1.x*TILE_SHOW_SIZE, link_data.p1.y*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);
            //painter.drawRect(link_data.p2.x*TILE_SHOW_SIZE, link_data.p2.y*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);


            painter.setOpacity(0.7);
            QRectF source(QPoint(0, 0), QSize(TILE_SHOW_SIZE, TILE_SHOW_SIZE));
            QRectF target1(QPoint(link_data.p1.x*TILE_SHOW_SIZE, link_data.p1.y*TILE_SHOW_SIZE), QSize(TILE_SHOW_SIZE, TILE_SHOW_SIZE));
            QRectF target2(QPoint(link_data.p2.x*TILE_SHOW_SIZE, link_data.p2.y*TILE_SHOW_SIZE), QSize(TILE_SHOW_SIZE, TILE_SHOW_SIZE));
            if (link_data.link_type == AREA_EDIT_MODE_HLINK) {
                if (link_data.p1.x < link_data.p2.x) {
                    painter.drawPixmap(target1, arrow_left, source);
                    painter.drawPixmap(target2, arrow_right, source);
                } else {
                    painter.drawPixmap(target2, arrow_left, source);
                    painter.drawPixmap(target1, arrow_right, source);
                }
            } else if (link_data.link_type == AREA_EDIT_MODE_VLINK) {
                if (link_data.p1.y < link_data.p2.y) {
                    painter.drawPixmap(target1, arrow_up, source);
                    painter.drawPixmap(target2, arrow_down, source);
                } else {
                    painter.drawPixmap(target2, arrow_up, source);
                    painter.drawPixmap(target1, arrow_down, source);
                }
            }

        }
    } else {
        std::cout << "MAP-LINKS #3"<< std::endl;
    }
    */

    // draw areas-points
    int area_quantity = SharedData::get_instance()->v6_area_list.size();
    for (unsigned int area_n=0; area_n<SharedData::get_instance()->v6_area_list.size(); area_n++) {
        if (SharedData::get_instance()->v6_level_map.find(area_n) != SharedData::get_instance()->v6_level_map.end()) {
            for (unsigned int i=0; i<SharedData::get_instance()->v6_level_map.at(area_n).size(); i++) {

                //std::cout << "area_n[" << area_n << "], i[" << i << "]" << std::endl;

                //std::cout << ">>>>>>>> room[" << i << "][" << SharedData::get_instance()->v6_level_map.at(area_n).at(i).x << "][" << SharedData::get_instance()->v6_level_map.at(area_n).at(i).y << "].area[" << SharedData::get_instance()->v6_level_map.at(area_n).at(i).area_number << "]" << std::endl;
                if (SharedData::get_instance()->v6_level_map.at(area_n).at(i).area_number == currentArea) {
                    painter.setBrush(QColor(0, 0, 255, 180));
                    painter.setPen(QColor(0, 0, 180, 255));
                } else {
                    painter.setBrush(QColor(99, 173, 230, 180));
                    painter.setPen(QColor(99, 117, 230, 255));
                }
                painter.drawRect(SharedData::get_instance()->v6_level_map.at(area_n).at(i).x*TILE_SHOW_SIZE, SharedData::get_instance()->v6_level_map.at(area_n).at(i).y*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);
            }
        }
    }


    // draw grid //
    painter.setPen(pen);
    int rows = this->height()/TILE_SHOW_SIZE;
    int cols = this->width()/TILE_SHOW_SIZE;
    for (int i=0; i<=rows; i++) {
        line = QLineF(0, i*TILE_SHOW_SIZE, cols*TILE_SHOW_SIZE, i*TILE_SHOW_SIZE);
        painter.drawLine(line);
    }

    for (int i=0; i<=cols; i++) {
        line = QLineF(i*TILE_SHOW_SIZE, 0, i*TILE_SHOW_SIZE, rows*TILE_SHOW_SIZE);
        painter.drawLine(line);
    }
}

void worldMapEditorArea::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/TILE_SHOW_SIZE;
    editor_selectedTileY = pnt.y()/TILE_SHOW_SIZE;
    //std::cout << "SET MAP AT [" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;

    // can't change area-zero minimal parts //
    if (currentArea == 0 && editor_selectedTileY == 0 && editor_selectedTileX >= 10 && editor_selectedTileX < 20) {
        return;
    }

    // search area-list to check if point is already used
    int point_already_used = -1;
    int current_area_rooms_count = 0;
    bool is_adjascent_point_to_same_area = false;
    bool is_adjascent_point = false;
    for (unsigned int area_n=0; area_n<SharedData::get_instance()->v6_area_list.size(); area_n++) {
        for (unsigned int i=0; i<SharedData::get_instance()->v6_level_map.at(area_n).size(); i++) {
            std::cout << "CLICK - area_n[" << area_n << "], i[" << i << "]" << std::endl;
            std::cout << "CLICK - area.size[" << SharedData::get_instance()->v6_level_map.at(area_n).size() << "]" << std::endl;
            if (SharedData::get_instance()->v6_level_map.at(area_n).at(i).x == editor_selectedTileX && SharedData::get_instance()->v6_level_map.at(area_n).at(i).y == editor_selectedTileY) {
                point_already_used = i;
            }
            bool is_next_to_room = false;
            // up
            if (editor_selectedTileX == SharedData::get_instance()->v6_level_map.at(area_n).at(i).x && editor_selectedTileY == SharedData::get_instance()->v6_level_map.at(area_n).at(i).y-1) {
                is_next_to_room = true;
            }
            // down
            if (editor_selectedTileX == SharedData::get_instance()->v6_level_map.at(area_n).at(i).x && editor_selectedTileY == SharedData::get_instance()->v6_level_map.at(area_n).at(i).y+1) {
                is_next_to_room = true;
            }
            // left
            if (editor_selectedTileX == SharedData::get_instance()->v6_level_map.at(area_n).at(i).x-1 && editor_selectedTileY == SharedData::get_instance()->v6_level_map.at(area_n).at(i).y) {
                is_next_to_room = true;
            }
            // right
            if (editor_selectedTileX == SharedData::get_instance()->v6_level_map.at(area_n).at(i).x+1 && editor_selectedTileY == SharedData::get_instance()->v6_level_map.at(area_n).at(i).y) {
                is_next_to_room = true;
            }
            if (SharedData::get_instance()->v6_level_map.at(area_n).at(i).area_number == currentArea) {
                current_area_rooms_count++;
                if (is_next_to_room == true) {
                    is_adjascent_point_to_same_area = true;
                }
            } else {
                if (is_next_to_room == true) {
                    is_adjascent_point = true;
                }
            }
        }
    }

    if (current_area_rooms_count >= AREA_ROOM_NUMBER) {
        QMessageBox msgBox;
        msgBox.setText("You reached the maximum number of rooms for this area");
        msgBox.exec();
        return;
    }

    //std::cout << "point_already_used[" << point_already_used << "] at [" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;
    if (point_already_used == -1) {
        // only accept adjascent points
        if (current_area_rooms_count > 0 && is_adjascent_point_to_same_area == false) {
            QMessageBox msgBox;
            msgBox.setText("You can only add points next to existing ones from same area");
            msgBox.exec();
            return;
        } else if (currentArea != 0 && current_area_rooms_count == 0 && is_adjascent_point == false) {
            QMessageBox msgBox;
            msgBox.setText("The first point of an area needs to be next to one from another area");
            msgBox.exec();
            return;
        }

        file_v6_level_point point;
        point.x = editor_selectedTileX;
        point.y = editor_selectedTileY;
        point.area_number = currentArea;
        SharedData::get_instance()->v6_level_map.at(currentArea).push_back(point);
        SharedData::get_instance()->add_missing_area_rooms(currentArea);
        std::cout << "point added area[" << currentArea << "], list_size[" << SharedData::get_instance()->v6_level_map.size() << "]" << std::endl;
    } else {
        std::cout << "ERROR: point already taken" << std::endl;
        /// @TODO: remove element from list, remove room, etc
        //SharedData::get_instance()->v6_level_list.at(point_already_used).x = -1;
    }

    repaint();


/*
    if (edit_mode == AREA_EDIT_MODE_NORMAL) {
        if (SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY].area_n == -1) {
            // add only if first piece or have another piece in the neighboor horizontal or vertical axis
            if (is_first_area_pieces() || have_adjacent_same_area_piece()) {
                SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY].area_n = currentMap;
            }
        } else if (SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY].area_n == currentMap) {
            SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY].area_n = -1;
        }
        repaint();
    } else if (edit_mode == AREA_EDIT_MODE_VLINK || edit_mode == AREA_EDIT_MODE_HLINK) {
        // TODO: check if point has a link, so delete it
        if (SharedData::get_instance()->file_v5_area_link_map.find(currentArea) != SharedData::get_instance()->file_v5_area_link_map.end()) {
            for (int i=0; i<SharedData::get_instance()->file_v5_area_link_map.at(currentArea).size(); i++) {
                struct_file_v5_area_link &link_data = SharedData::get_instance()->file_v5_area_link_map.at(currentArea).at(i);
                if ((link_data.p1.x == editor_selectedTileX && link_data.p1.y == editor_selectedTileY) || (link_data.p2.x == editor_selectedTileX && link_data.p2.y == editor_selectedTileY)) {
                    std::cout << "#### REMOVED LINK ####" << std::endl;
                    SharedData::get_instance()->file_v5_area_link_map.at(currentArea).erase(SharedData::get_instance()->file_v5_area_link_map.at(currentArea).begin()+i);
                    edit_mode == AREA_EDIT_MODE_NORMAL;
                    QApplication::setOverrideCursor(Qt::ArrowCursor);
                    link_p1.x = -1;
                    link_p1.y = -1;
                    repaint();
                    return;
                }
            }
        }

        // first point, just set
        if (link_p1.x == -1 && link_p1.y == -1) {
            std::cout << "#### ADD LINK - PART 1 ####" << std::endl;
            link_p1.x = editor_selectedTileX;
            link_p1.y = editor_selectedTileY;
            repaint();
        // second link, assure the point is on correct position
        } else {
            bool added_link = false;
            if (edit_mode == AREA_EDIT_MODE_VLINK && editor_selectedTileX == link_p1.x && (editor_selectedTileY == link_p1.y-1 || editor_selectedTileY == link_p1.y+1)) {
                added_link = true;
            } else if (edit_mode == AREA_EDIT_MODE_HLINK && editor_selectedTileY == link_p1.y && (editor_selectedTileX == link_p1.x-1 || editor_selectedTileX == link_p1.x+1)) {
                added_link = true;
            }
            std::cout << "#### ADD LINK - PART 2 - added_link[" << added_link << "] ####" << std::endl;
            if (added_link == true) {
                if (SharedData::get_instance()->file_v5_area_link_map.find(currentArea) == SharedData::get_instance()->file_v5_area_link_map.end()) {
                    SharedData::get_instance()->file_v5_area_link_map.insert(std::pair<unsigned int, std::vector<struct_file_v5_area_link>>(currentArea, std::vector<struct_file_v5_area_link>()));
                }
                SharedData::get_instance()->file_v5_area_link_map.at(currentArea).push_back(struct_file_v5_area_link(edit_mode, link_p1, st_position(editor_selectedTileX, editor_selectedTileY)));
                link_p1 = st_position(-1, -1);
                QApplication::setOverrideCursor(Qt::ArrowCursor);
                repaint();
            }
        }
    }
*/
}

bool worldMapEditorArea::is_first_area_pieces()
{
    /*
    for (int x=0; x<FILE_AREA_W; x++) {
        for (int y=0; y<FILE_AREA_H; y++) {
            if (SharedData::get_instance()->v6_level_list.at(currentArea).rooms[x][y].area_n == currentMap) {
                return false;
            }
        }
    }
    */
    return true;
}

bool worldMapEditorArea::have_adjacent_same_area_piece()
{
    /*
    if (editor_selectedTileX > 0 && SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX-1][editor_selectedTileY].area_n == currentMap) {
        return true;
    }
    if (editor_selectedTileX < FILE_AREA_W-2 && SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX+1][editor_selectedTileY].area_n == currentMap) {
        return true;
    }

    if (editor_selectedTileY > 0 && SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY-1].area_n == currentMap) {
        return true;
    }
    if (editor_selectedTileY < FILE_AREA_H-2 && SharedData::get_instance()->v6_level_list.at(currentArea).rooms[editor_selectedTileX][editor_selectedTileY+1].area_n == currentMap) {
        return true;
    }
    */

    return false;
}
