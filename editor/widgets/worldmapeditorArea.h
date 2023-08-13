#ifndef AREASEDITORAREA_H
#define AREASEDITORAREA_H

#include <QObject>
#include <QWidget>
#include <QMessageBox>
#include "defines.h"
#include "data/st_common.h"

class worldMapEditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit worldMapEditorArea(QWidget *parent = nullptr);
    void setCurrentStage(int area_n);
    void setCurrentArea(int map_n);
    void set_edit_mode(e_AREA_EDIT_MODE mode);



protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * event);
    //void mouseReleaseEvent(QMouseEvent * event);
    //void mouseMoveEvent(QMouseEvent *event);

private:
    bool is_first_area_pieces();
    bool have_adjacent_same_area_piece();

signals:

private:
    unsigned int currentStage = 0;
    unsigned int currentArea = 0;
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
