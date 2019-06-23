#include "editortilepallete.h"
#include <stdio.h>


EditorTilePallete::EditorTilePallete(QWidget *parent) : QWidget(parent) {
   printf("DEBUG.EditorTilePallete - constructor\n");
   selectedTileX=0;
   selectedTileY=0;
   myParent = parent;
   parent->resize(QSize(250, 200));
   ADJUSTED_SIZE = (double)SHOW_TILESIZE/TILESIZE;
   std::cout << ">>>>>>>>>>>>>>>>>>>>>>>> ADJUSTED_SIZE[" << ADJUSTED_SIZE << "]" << std::endl;
   this->show();
}

void EditorTilePallete::paintEvent(QPaintEvent *) {
    // @TODO - load images only when needed //
   QPixmap *image;

   QString filename;
   if (Mediator::get_instance()->getPallete().length() < 1) {
        //std::cout << "EditorTilePallete::paintEvent - USING DEFAULT TILESET" << std::endl;
        filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/") + QString("default.png");
   } else {
       //std::cout << "EditorTilePallete::paintEvent - USING SET TILESET[" << std::endl;
        filename = QString(SharedData::get_instance()->FILEPATH.c_str()) + QString("/images/tilesets/") + QString(Mediator::get_instance()->getPallete().c_str());
   }

   QPainter painter(this);
   image = new QPixmap(filename);
   if (image->isNull()) {
      std::cout << "EditorTilePallete::paintEvent - DEBUG.Tile - Could not load image file[" << qPrintable(filename) << "]" << std::endl;;
   }
   QRectF target(QPoint(0, 0), QSize(image->size().width()*ADJUSTED_SIZE, image->size().height()*ADJUSTED_SIZE));
   QRectF source(QPoint(0, 0), image->size());
   painter.drawPixmap(target, *image, source);
   this->resize(image->size());
   myParent->adjustSize();
   // draw the selection marker
   painter.setPen(QColor(255, 0, 0));
   QRectF select(QPoint((selectedTileX*SHOW_TILESIZE), (selectedTileY*SHOW_TILESIZE)), QSize(SHOW_TILESIZE, SHOW_TILESIZE));
   painter.drawRect(select);
}

void EditorTilePallete::changeTileSet(const QString &tileset) {
   printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% mudando paleta para %s\n", qPrintable(tileset));
   signalPalleteChanged();
   repaint();
}

void EditorTilePallete::mousePressEvent(QMouseEvent *event) {
   QPoint pnt = event->pos();
   selectedTileX = (int)(pnt.x()/SHOW_TILESIZE);
   selectedTileY = (int)(pnt.y()/SHOW_TILESIZE);
   Mediator::get_instance()->setPalleteX(selectedTileX);
   Mediator::get_instance()->setPalleteY(selectedTileY);
   printf("DEBUG.EditorTilePallete::mousePressEvent - PalleteX: %d, palleteY: %d\n", selectedTileX, selectedTileY);
   repaint();
}




QString EditorTilePallete::getPallete() {
    QString res(Mediator::get_instance()->getPallete().c_str());
    std::cout << "EditorTilePallete::getPallete - res: " << res.toStdString() << std::endl;
    return res;
}

//void EditorTilePallete::signalPalleteChanged() {
//   return;
//}


