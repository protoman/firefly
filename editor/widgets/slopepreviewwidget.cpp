#include "slopepreviewwidget.h"

#define SHOW_TILESIZE 64

SlopePreviewWidget::SlopePreviewWidget(QWidget *parent) : QWidget(parent)
{

}

void SlopePreviewWidget::setImage(std::string filename)
{
    this->filename = filename;
    //std::cout << "### SlopePreviewWidget::setImage[" << filename << "]" << std::endl;
}

int SlopePreviewWidget::getCurrentPos()
{
    return selectedTileX;
}

void SlopePreviewWidget::setLeft(int left)
{
    this->left = left;
}

void SlopePreviewWidget::setRight(int right)
{
    this->right = right;
}

void SlopePreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (filename.length() == 0) {
        std::cout << "### SlopePreviewWidget::NO_IMAGE" << std::endl;
        return;
    }
    std::cout << "### SlopePreviewWidget::paintEvent filename[" << filename << "]" << std::endl;

    std::string full_filename = SharedData::get_instance()->FILEPATH + "/images/tilesets/slope/" + filename;
    QPixmap image(QString(full_filename.c_str()));
    if (image.isNull()) {
        std::cout << "### SlopePreviewWidget::NULL_IMAGE[" << full_filename << "]" << std::endl;
        return;
    }
    image = image.scaled(image.width()*2, image.height()*2);
    QRectF target(QPoint(0, 0), QSize(image.width(), image.height()));
    QRectF source(QPoint(0, 0), QSize(image.width(), image.height()));
    painter.drawPixmap(target, image, source);

    // TODO: draw grid //


    // draw pos selection
    painter.setPen(QColor(255, 0, 0));
    QRectF select(QPoint((selectedTileX*SHOW_TILESIZE), (selectedTileY*SHOW_TILESIZE)), QSize(SHOW_TILESIZE, SHOW_TILESIZE-1));
    painter.drawRect(select);


    // draw left-point //
    painter.setPen(QColor(0, 200, 0));
    painter.setBrush(QColor(0, 200, 0));
    QRectF left_point(QPoint(1+selectedTileX*SHOW_TILESIZE, left*2), QSize(2, 2));
    painter.drawRect(left_point);


    // draw right-point //
    painter.setPen(QColor(224, 207, 0));
    painter.setBrush(QColor(224, 207, 0));
    QRectF right_point(QPoint((selectedTileX+1)*SHOW_TILESIZE-3, right*2), QSize(2, 2));
    painter.drawRect(right_point);

}

void SlopePreviewWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pnt = event->pos();
    selectedTileX = pnt.x()/(SHOW_TILESIZE);
    selectedTileY = pnt.y()/(SHOW_TILESIZE);
    emit currentPosChanged(selectedTileX);
    repaint();
}
