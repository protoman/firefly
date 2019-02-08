#include "areaseditorarea.h"

#include <QPainter>
#include <QMouseEvent>


#include "defines.h"
#include "data/shareddata.h"

#define TILE_SHOW_SIZE 16

areasEditorArea::areasEditorArea(QWidget *parent) : QWidget(parent)
{

}

void areasEditorArea::setCurrentArea(int area_n)
{
    currentArea = area_n;
}

void areasEditorArea::setCurrentMap(int map_n)
{
    currentMap = map_n;
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
            painter.setBrush(QColor(0, 0, 255, 180));
            painter.setPen(QColor(0, 0, 255, 255));
            if (SharedData::get_instance()->area_list.at(currentArea).point[i][j] != -1) {
                //std::cout << "found map[" << SharedData::get_instance()->area_list.at(currentArea).point[i][j] << "], at[" << i << "][" << j << "]" << std::endl;
                painter.drawRect(i*TILE_SHOW_SIZE, j*TILE_SHOW_SIZE, TILE_SHOW_SIZE, TILE_SHOW_SIZE);
            }
        }
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

    if (SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] == -1) {
        SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] = currentMap;
    } else if (SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] == currentMap) {
        SharedData::get_instance()->area_list.at(currentArea).point[editor_selectedTileX][editor_selectedTileY] = -1;
    }


    if (SharedData::get_instance()->area_list.at(currentArea).map[currentMap] == -1) {
        SharedData::get_instance()->area_list.at(currentArea).map[currentMap] = currentMap;
    }
    repaint();
}
