#ifndef AREAEDITPREVIEWWIDGET_H
#define AREAEDITPREVIEWWIDGET_H

#include <QObject>
#include <QWidget>

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

signals:


public slots:



};

#endif // AREAEDITPREVIEWWIDGET_H
