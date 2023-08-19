#ifndef NPCPREVIEWAREA_H
#define NPCPREVIEWAREA_H

#include <QObject>
#include <QWidget>
#include <QImage>

#include "file/v6/file_area_v6.h"
#include "data/shareddata.h"


class NpcPreviewArea : public QWidget
{
    Q_OBJECT
public:
    explicit NpcPreviewArea(QWidget *parent = nullptr);
    void set_anim_type(int type);

protected:
  void paintEvent(QPaintEvent *event);

private:
  QWidget *my_parent;
  int animation_type = 0;

signals:

};

#endif // NPCPREVIEWAREA_H
