#ifndef SLOPEPICKERWIDGET_H
#define SLOPEPICKERWIDGET_H

#include <QObject>
#include <QWidget>

#include <iostream>
#include <QWidgetData>
#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include "data/shareddata.h"


class SlopePickerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SlopePickerWidget(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent * event);


private:
    std::vector<st_position> slope_id_list;
    unsigned int selectedTileX = 0;
    unsigned int selectedTileY = 0;

signals:

public slots:
};



#endif // SLOPEPICKERWIDGET_H
