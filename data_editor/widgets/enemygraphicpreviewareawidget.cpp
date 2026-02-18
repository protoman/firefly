#include "enemygraphicpreviewareawidget.h"

#include <QPainter>
#include <QColor>
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
    if (size_w != 0 && size_w != 0) {
        int max_w = graphic_image.size().width() / size_w;
        int max_h = graphic_image.size().height() / size_h;
        QPen pen(QColor(160, 160, 160), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        // linhas verticais
        for (unsigned int i=0; i<max_h; i++) {
            int pos = i*size_w*zoom;
            line = QLineF(pos, 0, pos,200 * size_w * zoom);
            painter.drawLine(line);
        }
        // linhas horizontais
        for (unsigned int i=0; i<max_w; i++) {
            int pos = i * size_h*zoom;
            //QLineF line(0, 800, 16, 800);
            line = QLineF(0, pos, 200*size_h*zoom, pos);
            painter.drawLine(line);
        }
    }

}

void EnemyGraphicPreviewAreaWidget::setDataDirectory(std::string directory) {
    game_data_directory = directory + "/images/sprites/enemies/";
}
