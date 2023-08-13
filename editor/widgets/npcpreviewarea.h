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

signals:

};

#endif // NPCPREVIEWAREA_H
