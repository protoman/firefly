#include "worldmapeditorArea.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QResource>


#include "defines.h"
#include "data/shareddata.h"
#include "game_data.h"
#include "mediator.h"

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

void worldMapEditorArea::setCurrentStage(int area_n)
{
    currentStage = area_n;
}

void worldMapEditorArea::setCurrentArea(int map_n)
{
    currentArea = map_n;
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
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QLineF line;


    // draw areas-points
    for (unsigned int area_n=0; area_n<GameData::get_instance()->v6_stage_list.size(); area_n++) {
        if (Mediator::get_instance()->v6_level_map.find(area_n) != Mediator::get_instance()->v6_level_map.end()) {
            for (unsigned int i=0; i<Mediator::get_instance()->v6_level_map.at(area_n).size(); i++) {
                if (Mediator::get_instance()->v6_level_map.at(area_n).at(i).stage_number == currentStage) {
                    painter.setBrush(QColor(0, 0, 255, 180));
                    if (Mediator::get_instance()->v6_level_map.at(area_n).at(i).area_number == currentArea) {
                        painter.setBrush(QColor(0, 255, 0, 180));
                    }
                    painter.setPen(QColor(0, 0, 180, 255));
                    painter.drawRect(Mediator::get_instance()->v6_level_map.at(area_n).at(i).x*TILE_SHOW_SIZE, Mediator::get_instance()->v6_level_map.at(area_n).at(i).y*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);
                } else {
                    std::cout << ">>>>>>>>>> invalid area number[" << Mediator::get_instance()->v6_level_map.at(area_n).at(i).stage_number << "] in area[" << currentStage << "] data file" << std::endl;
                }
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

    // can't change area-zero minimal parts //
    if (currentStage != 0 && editor_selectedTileY == 0 && editor_selectedTileX < 10) {
        QMessageBox msgBox;
        msgBox.setText("This room is reserved for initial area.");
        msgBox.exec();
        return;
    }

    // search area-list to check if point is already used
    int current_area_rooms_count = 0;
    bool is_adjascent_point_to_same_area = false;
    bool is_adjascent_point = false;
    for (unsigned int area_n=0; area_n<GameData::get_instance()->v6_stage_list.size(); area_n++) {

        if (Mediator::get_instance()->v6_level_map.find(area_n) == Mediator::get_instance()->v6_level_map.end()) {
            std::vector<file_v6_level_point> point_list;
            Mediator::get_instance()->v6_level_map.insert(std::pair<int, std::vector<file_v6_level_point>>(area_n, point_list));
        }

        for (unsigned int i=0; i<Mediator::get_instance()->v6_level_map.at(area_n).size(); i++) {
            std::cout << "CLICK - area_n[" << area_n << "], i[" << i << "]" << std::endl;
            std::cout << "CLICK - area.size[" << Mediator::get_instance()->v6_level_map.at(area_n).size() << "]" << std::endl;
            bool is_next_to_room = false;

            if (editor_selectedTileX == Mediator::get_instance()->v6_level_map.at(area_n).at(i).x && editor_selectedTileY == Mediator::get_instance()->v6_level_map.at(area_n).at(i).y) {
                QMessageBox msgBox;
                msgBox.setText("Point already set with area[" + QString::number(Mediator::get_instance()->v6_level_map.at(area_n).at(i).area_number) + "]");
                msgBox.exec();
                return;
            }

            // up
            if (editor_selectedTileX == Mediator::get_instance()->v6_level_map.at(area_n).at(i).x && editor_selectedTileY == Mediator::get_instance()->v6_level_map.at(area_n).at(i).y-1) {
                is_next_to_room = true;
            }
            // down
            if (editor_selectedTileX == Mediator::get_instance()->v6_level_map.at(area_n).at(i).x && editor_selectedTileY == Mediator::get_instance()->v6_level_map.at(area_n).at(i).y+1) {
                is_next_to_room = true;
            }
            // left
            if (editor_selectedTileX == Mediator::get_instance()->v6_level_map.at(area_n).at(i).x-1 && editor_selectedTileY == Mediator::get_instance()->v6_level_map.at(area_n).at(i).y) {
                is_next_to_room = true;
            }
            // right
            if (editor_selectedTileX == Mediator::get_instance()->v6_level_map.at(area_n).at(i).x+1 && editor_selectedTileY == Mediator::get_instance()->v6_level_map.at(area_n).at(i).y) {
                is_next_to_room = true;
            }
            if (Mediator::get_instance()->v6_level_map.at(area_n).at(i).stage_number == currentStage) {
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

    // TODO: do not add same point again

    if (current_area_rooms_count >= AREA_ROOM_MAX_NUMBER) {
        QMessageBox msgBox;
        msgBox.setText("You reached the maximum number of rooms for this area");
        msgBox.exec();
        return;
    }

    //std::cout << "point_already_used[" << point_already_used << "] at [" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;
    // only accept adjascent points
    if (current_area_rooms_count > 0 && is_adjascent_point_to_same_area == false) {
        QMessageBox msgBox;
        msgBox.setText("You can only add points next to existing ones from same area");
        msgBox.exec();
        return;
    } else if (currentStage != 0 && current_area_rooms_count == 0 && is_adjascent_point == false) {
        QMessageBox msgBox;
        msgBox.setText("The first point of an area needs to be next to one from another area");
        msgBox.exec();
        return;
    }

    file_v6_level_point point;
    point.x = editor_selectedTileX;
    point.y = editor_selectedTileY;
    point.stage_number = currentStage;
    point.area_number = currentArea;
    Mediator::get_instance()->v6_level_map.at(currentStage).push_back(point);
    Mediator::get_instance()->add_missing_area_rooms(currentStage, currentArea);
    std::cout << "point added area[" << currentStage << "], list_size[" << Mediator::get_instance()->v6_level_map.size() << "]" << std::endl;

    repaint();

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
