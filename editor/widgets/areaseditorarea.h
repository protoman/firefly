#ifndef AREASEDITORAREA_H
#define AREASEDITORAREA_H

#include <QObject>
#include <QWidget>
#include "defines.h"
#include "data/st_common.h"

class areasEditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit areasEditorArea(QWidget *parent = nullptr);
    void setCurrentArea(int area_n);
    void setCurrentMap(int map_n);
    void set_edit_mode(e_AREA_EDIT_MODE mode);



protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * event);
    //void mouseReleaseEvent(QMouseEvent * event);
    //void mouseMoveEvent(QMouseEvent *event);

signals:

private:
    unsigned int currentArea = 0;
    unsigned int currentMap = 0;
    unsigned int editor_selectedTileX = 0;
    unsigned int editor_selectedTileY = 0;
    e_AREA_EDIT_MODE edit_mode = AREA_EDIT_MODE_NORMAL;
    st_position link_p1 = st_position(-1, -1);
    QPixmap arrow_left;
    QPixmap arrow_right;
    QPixmap arrow_up;
    QPixmap arrow_down;

public slots:
};

#endif // AREASEDITORAREA_H
