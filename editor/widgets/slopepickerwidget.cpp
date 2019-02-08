#include "slopepickerwidget.h"

#include "mediator.h"

SlopePickerWidget::SlopePickerWidget(QWidget *parent) : QWidget(parent)
{

}

void SlopePickerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    slope_id_list.clear();
    int n = 0;
    for (int i=0; i<SharedData::get_instance()->slope_list.size(); i++) {
        file_v5_slope_tile* slope_data = &SharedData::get_instance()->slope_list.at(i);
        std::string full_filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/slope/" + slope_data->filename;

        QPixmap image(QString(full_filename.c_str()));
        if (image.isNull()) {
            continue;
        }
        image = image.scaled(image.width()/2, image.height()/2);

        for (int j=0; j<SLOPE_MAX_TILES; j++) {
            if (slope_data->slope[j].left == 0 && slope_data->slope[j].right == 0) {
                continue;
            }
            QRectF target(QPoint(TILESIZE/2*n, 0), QSize(TILESIZE/2, TILESIZE/2));
            QRectF source(QPoint(j*TILESIZE/2, 0), QSize(TILESIZE/2, TILESIZE/2));

            painter.drawPixmap(target, image, source);

            slope_id_list.push_back(st_position(i, j));
            n++;
        }
        QRectF selection(QPoint(TILESIZE/2*selectedTileX, 0), QSize(TILESIZE/2, TILESIZE/2));
        painter.setPen(QColor(255, 0, 0));
        painter.drawRect(selection);
    }
}

void SlopePickerWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    selectedTileX = pnt.x()/(SHOW_TILESIZE);
    Mediator::get_instance()->setPalleteX(slope_id_list.at(selectedTileX).x);
    Mediator::get_instance()->setPalleteY(slope_id_list.at(selectedTileX).y);
    repaint();
}
