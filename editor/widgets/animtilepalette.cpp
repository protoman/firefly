#include "animtilepalette.h"
#include "mediator.h"

#include <QPainter>
#include <QMouseEvent>


animTilePalette::animTilePalette(QWidget *parent) : QWidget(parent)
{
    myParent = parent;
    selectedTileX = 0;
    selectedTileY = 0;
}

QString animTilePalette::getPallete()
{

}

void animTilePalette::reload()
{
    image_list.clear();
    int max = Mediator::get_instance()->anim_block_list.size();
    //std::cout << "ANIMPALETTE::reload::max: " << max << std::endl;
    for (int i=0; i<max; i++) {
        std::string filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/anim/" + std::string(Mediator::get_instance()->anim_block_list.at(i).filename);
        if (filename.find(".png") == std::string::npos) {
            continue;
        }
        QPixmap image(QString(filename.c_str()));
        image = image.scaled(image.width()*2, image.height()*2);
        image_list.push_back(image);
    }
    this->resize(QSize(max*SHOW_TILESIZE, SHOW_TILESIZE));
    myParent->adjustSize();
}

void animTilePalette::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int row = 0;
    int col = 0;
    for (int i=0; i<image_list.size(); i++) {
        if (image_list.at(i).isNull() == false) {
            QRectF target(QPoint(col*SHOW_TILESIZE, row*SHOW_TILESIZE), QSize(SHOW_TILESIZE, SHOW_TILESIZE));
            QRectF source(QPoint(0, 0), QSize(SHOW_TILESIZE, SHOW_TILESIZE));
            painter.drawPixmap(target, image_list.at(i), source);
            col++;
            if (col > EDITOR_ANIM_PALETE_MAX_COL) {
                row++;
                col = 0;
            }
        }
    }
    this->resize(this->width(), (row+1)*SHOW_TILESIZE);
    // draw the selection marker
    painter.setPen(QColor(255, 0, 0));
    QRectF select(QPoint((selectedTileX*SHOW_TILESIZE), (selectedTileY*SHOW_TILESIZE)), QSize(SHOW_TILESIZE, SHOW_TILESIZE-1));
    painter.drawRect(select);
}

void animTilePalette::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    selectedTileX = pnt.x()/(SHOW_TILESIZE);
    selectedTileY = pnt.y()/(SHOW_TILESIZE);
    Mediator::get_instance()->setPalleteX(selectedTileX);
    Mediator::get_instance()->setPalleteY(selectedTileY);

    std::cout << ">>>>>>>>>>>>> animTilePalette::mousePressEvent - x: " << selectedTileX << ", y: " << selectedTileY << std::endl;

    Mediator::get_instance()->selectedAnimTileset = selectedTileX + (selectedTileY * EDITOR_ANIM_PALETE_MAX_COL) + selectedTileY;

    repaint();
}

void animTilePalette::changeTileSet(const QString &tileset)
{

}

