#include "slopepickerwidget.h"

#include "mediator.h"

SlopePickerWidget::SlopePickerWidget(QWidget *parent) : QWidget(parent)
{

}

void SlopePickerWidget::paintEvent(QPaintEvent *event)
{
    int n = 0;

    slope_id_list.clear();
    QPainter painter(this);
    int column = 0;
    int row = 0;

    for (int i=0; i<GameData::get_instance()->slope_list.size(); i++) {
        file_v5_slope_tile* slope_data = &GameData::get_instance()->slope_list.at(i);
        std::string full_filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/slope/" + slope_data->filename;

        QPixmap image(QString(full_filename.c_str()));
        if (image.isNull()) {
            continue;
        }
        //image = image.scaled(image.width()*2, image.height()*2);

        for (int j=0; j<SLOPE_MAX_TILES; j++) {
            if (slope_data->slope[j].left == 0 && slope_data->slope[j].right == 0) {
                continue;
            }
            QRectF target(QPoint(TILESIZE*column, TILESIZE*row), QSize(TILESIZE, TILESIZE));
            QRectF source(QPoint(j*TILESIZE, 0), QSize(TILESIZE, TILESIZE));
            column++;
            if (column >= 8) {
                column = 0;
                row++;
            }

            painter.drawPixmap(target, image, source);

            slope_id_list.push_back(st_position(i, j));
            n++;
        }
        QRectF selection(QPoint(TILESIZE*selectedTileX, TILESIZE*selectedTileY), QSize(TILESIZE, TILESIZE));
        painter.setPen(QColor(255, 0, 0));
        painter.drawRect(selection);
    }
}

void SlopePickerWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    selectedTileX = pnt.x()/(SHOW_TILESIZE);
    selectedTileY = pnt.y()/(SHOW_TILESIZE);
    int n = selectedTileX + selectedTileY*8;
    if (n >= slope_id_list.size()) {
        std::cout << "Invalid selection n[" << n << "], x[" << selectedTileX << "], y[" << selectedTileY << "]" << std::endl;
        selectedTileX = 0;
        selectedTileY = 0;
        Mediator::get_instance()->setPalleteX(slope_id_list.at(0).x);
        Mediator::get_instance()->setPalleteY(slope_id_list.at(0).y);
        repaint();
        return;
    }
    Mediator::get_instance()->setPalleteX(slope_id_list.at(n).x);
    Mediator::get_instance()->setPalleteY(slope_id_list.at(n).y);
    repaint();
}
