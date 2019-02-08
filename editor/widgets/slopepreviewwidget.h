#ifndef SLOPEPREVIEWWIDGET_H
#define SLOPEPREVIEWWIDGET_H

#include <iostream>
#include <QWidgetData>
#include <QPainter>
#include <QMouseEvent>
#include "data/shareddata.h"

class SlopePreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SlopePreviewWidget(QWidget *parent = nullptr);
    void setImage(std::string filename);
    int getCurrentPos();
    void setLeft(int left);
    void setRight(int right);


protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent * event);


private:
    std::string filename = "";
    int selectedTileX=0;
    int selectedTileY=0;
    int left = 0;
    int right = 0;

signals:
    int currentPosChanged(int pos);

public slots:

};


#endif // SLOPEPREVIEWWIDGET_H
