#include "areaseditorarea.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QResource>


#include "defines.h"
#include "data/shareddata.h"

#define TILE_SHOW_SIZE 16

areasEditorArea::areasEditorArea(QWidget *parent) : QWidget(parent)
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

void areasEditorArea::setCurrentArea(int area_n)
{
    currentArea = area_n;
}

void areasEditorArea::setCurrentMap(int map_n)
{
    currentMap = map_n;
}

void areasEditorArea::set_edit_mode(e_AREA_EDIT_MODE mode)
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

void areasEditorArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen_red(QColor(180, 50, 50), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QLineF line;

    // draw map points
    for (int i=0; i<GAME_AREA_SIZE; i++) {
        for (int j=0; j<GAME_AREA_SIZE; j++) {
            // TODO - make current make different color
            if (SharedData::get_instance()->area_list.size() > currentArea && SharedData::get_instance()->area_list.at(currentArea).point[i][j] != -1) {
                //std::cout << "x[" << i << "], y[" << j << "], map[" << SharedData::get_instance()->area_list.at(currentArea).point[i][j] << "]" << std::endl;
                if (SharedData::get_instance()->area_list.at(currentArea).point[i][j] == currentMap) {
                    painter.setBrush(QColor(0, 0, 255, 180));
                    painter.setPen(QColor(0, 0, 180, 255));
                } else {
                    painter.setBrush(QColor(99, 173, 230, 180));
                    painter.setPen(QColor(99, 117, 230, 255));
                }
                //std::cout << "found map[" << SharedData::get_instance()->area_list.at(currentArea).point[i][j] << "], at[" << i << "][" << j << "]" << std::endl;
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


    painter.setPen(pen);
    for (int i=0; i<=GAME_AREA_SIZE; i++) {
        line = QLineF(0, i*TILE_SHOW_SIZE, GAME_AREA_SIZE*TILE_SHOW_SIZE, i*TILE_SHOW_SIZE);
        painter.drawLine(line);
    }

    for (int i=0; i<=GAME_AREA_SIZE; i++) {
        line = QLineF(i*TILE_SHOW_SIZE, 0, i*TILE_SHOW_SIZE, GAME_AREA_SIZE*TILE_SHOW_SIZE);
        painter.drawLine(line);
    }

}

void areasEditorArea::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    editor_selectedTileX = pnt.x()/TILE_SHOW_SIZE;
    editor_selectedTileY = pnt.y()/TILE_SHOW_SIZE;
    std::cout << "SET MAP AT [" << editor_selectedTileX << "][" << editor_selectedTileY << "]" << std::endl;

    if (edit_mode == AREA_EDIT_MODE_NORMAL) {
        if (SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] == -1) {
            SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] = currentMap;
        } else if (SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] == currentMap) {
            SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] = -1;
        }
        if (SharedData::get_instance()->area_list.at(currentArea).map[currentMap] == -1) {
            SharedData::get_instance()->area_list.at(currentArea).map[currentMap] = currentMap;
        }
        repaint();
    } else if (edit_mode == AREA_EDIT_MODE_VLINK || edit_mode == AREA_EDIT_MODE_HLINK) {
        // TODO: check if point has a link, so delete it
        if (SharedData::get_instance()->file_v5_area_link_map.find(currentArea) != SharedData::get_instance()->file_v5_area_link_map.end()) {
            for (int i=0; i<SharedData::get_instance()->file_v5_area_link_map.at(currentArea).size(); i++) {
                struct_file_v5_area_link &link_data = SharedData::get_instance()->file_v5_area_link_map.at(currentArea).at(i);
                if ((link_data.p1.x == editor_selectedTileX && link_data.p1.y == editor_selectedTileY) || (link_data.p2.x == editor_selectedTileX && link_data.p2.y == editor_selectedTileY)) {
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
}
