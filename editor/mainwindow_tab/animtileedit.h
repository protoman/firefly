#ifndef ANIMTILEEDIT_H
#define ANIMTILEEDIT_H

#include <QWidget>
#include <QPainter>
#include <stdio.h>
#include <QMouseEvent>
#include "mediator.h"

class animTileEdit : public QWidget
{
    Q_OBJECT
public:
    explicit animTileEdit(QWidget *parent = nullptr);
    void set_graphicfile(std::string filename);
    std::string get_graphicfile();
    void set_bg_graphicfile(std::string filename);
    void set_grid_w(int new_w);
    void set_grid_h(int new_h);
    void set_sprite_pos(st_position s_pos);
    int get_cursor_pos();

private:
  int editor_selectedTileX;
  int editor_selectedTileY;
  std::string graphic_filename;
  int img_grid_w;
  int img_grid_h;
  int image_w;
  int image_h;
  int bg_image_w;
  int bg_image_h;
  st_position _sprites_pos;
  std::string _bg_graphic_filename;
  double ADJUSTED_SIZE;

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent * event);

  QWidget *myParent;

signals:
     void clickedIn();

public slots:
};

#endif // ANIMTILEEDIT_H
