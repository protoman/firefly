#include "enemygraphicpreviewareawidget.h"

#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <iostream>

EnemyGraphicPreviewAreaWidget::EnemyGraphicPreviewAreaWidget(QWidget *parent)
    : QWidget{parent} {
    myParent = parent;
}

void EnemyGraphicPreviewAreaWidget::updateGraphic(int w, int h, std::string filename) {
    graphic_filename = game_data_directory + filename;
    if (!filename.empty()) {
        graphic_image = QPixmap(graphic_filename.c_str());
    }
    size_w = w;
    size_h = h;
}

void EnemyGraphicPreviewAreaWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QLineF line;

    if (graphic_filename.empty()) {
        return;
    }
    if (graphic_image.isNull()) {
        std::cout << "Could not open image data for file [" << graphic_filename << "]" << std::endl;
        return;
    }

    // DRAW IMAGE //
    QRectF target(QPoint(0, 0), QSize(graphic_image.size().width() * zoom, graphic_image.size().height() * zoom));
    QRectF source(QPoint(0, 0), QSize(graphic_image.size().width(), graphic_image.size().height()));
    painter.drawPixmap(target, graphic_image, source);

    // DRAW GRID //
    if (size_w != 0 && size_h != 0) {
        int max_w = (graphic_image.size().width() / size_w) + 1;
        int max_h = (graphic_image.size().height() / size_h) + 1;
        QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        // linhas verticais
        for (unsigned int i=0; i<max_w; i++) {
            int pos = i * size_w * zoom;
            line = QLineF(pos, 0, pos, (graphic_image.size().height() * zoom));
            painter.drawLine(line);
        }
        // linhas horizontais
        for (unsigned int i=0; i<max_h; i++) {
            int pos = i * size_h * zoom;
            line = QLineF(0, pos, (graphic_image.size().width() * zoom), pos);
            painter.drawLine(line);
        }
    }

    // DRAW SELECTED FRAME //
    QPen pen(QColor(255, 60, 60), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    // linhas verticais
    int pos_x1 = selected_point.x * size_w * zoom;
    line = QLineF(pos_x1, 0, pos_x1, (size_h * zoom));
    painter.drawLine(line);
    int pos_x2 = (selected_point.x +1) * size_w * zoom;
    line = QLineF(pos_x2, 0, pos_x2, (size_h * zoom));
    painter.drawLine(line);
    // linhas horizontais
    int pos_y1 = selected_point.y * size_h * zoom;
    line = QLineF(pos_x1, pos_y1, pos_x2, pos_y1);
    painter.drawLine(line);
    int pos_y2 = (selected_point.y + 1) * size_h * zoom;
    line = QLineF(pos_x1, pos_y2, pos_x2, pos_y2);
    painter.drawLine(line);
}

void EnemyGraphicPreviewAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    int point_x = pnt.x() / (size_w * zoom);
    int point_y = pnt.y() / (size_h * zoom);
    if (!(point_x < 0 || (point_x + 1) > (graphic_image.size().width() / size_w) || point_y < 0 || (point_y + 1) > (graphic_image.size().height() / size_h))) {
        selected_point.x = point_x;
        selected_point.y = point_y;
    }
    repaint();
}

void EnemyGraphicPreviewAreaWidget::setDataDirectory(std::string directory) {
    game_data_directory = directory + "/images/sprites/enemies/";
}
