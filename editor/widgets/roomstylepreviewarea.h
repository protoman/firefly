#ifndef ROOMSTYLEPREVIEWAREA_H
#define ROOMSTYLEPREVIEWAREA_H

#include <QObject>
#include <QWidget>
#include <QImage>

#include "file/v6/file_style_v6.h"
#include "data/shareddata.h"

class RoomStylePreviewArea : public QWidget
{
    Q_OBJECT
public:
    explicit RoomStylePreviewArea(QWidget *parent = nullptr);
    void update_files();

protected:
    void paintEvent(QPaintEvent *event);


private:
    QPixmap tileset_image;
    QPixmap layer_pixmap_list[LAYERS_COUNT];


signals:


public slots:



};

#endif // ROOMSTYLEPREVIEWAREA_H
