#ifndef AREASEDITORAREA_H
#define AREASEDITORAREA_H

#include <QObject>
#include <QWidget>

class areasEditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit areasEditorArea(QWidget *parent = nullptr);
    void setCurrentArea(int area_n);
    void setCurrentMap(int map_n);


protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * event);
    //void mouseReleaseEvent(QMouseEvent * event);
    //void mouseMoveEvent(QMouseEvent *event);

signals:

private:
    int currentArea = 0;
    int currentMap = 0;
    int editor_selectedTileX = 0;
    int editor_selectedTileY = 0;

public slots:
};

#endif // AREASEDITORAREA_H
