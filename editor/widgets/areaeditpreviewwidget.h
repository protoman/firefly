#ifndef AREAEDITPREVIEWWIDGET_H
#define AREAEDITPREVIEWWIDGET_H

#include <QObject>
#include <QWidget>
#include <QImage>

#include "file/v6/file_area_v6.h"
#include "data/shareddata.h"

class areaEditPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit areaEditPreviewWidget(QWidget *parent = nullptr);
    void update_files();

protected:
    void paintEvent(QPaintEvent *event);


private:
    QPixmap tileset_image;
    QPixmap layer_pixmap_list[LAYERS_COUNT];


public:
    short current_layer = 0;

signals:


public slots:



};

#endif // AREAEDITPREVIEWWIDGET_H
