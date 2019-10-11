#include <QWidget>
#include <QShortcut>
#include <QPainter>
#include <QTimer>
#include <QScrollArea>
#include <QMessageBox>
#include <QColor>
#include <QMouseEvent>
#include <QBitmap>
#include <QInputEvent>

#include "aux_tools/stringutils.h"

#include "mediator.h"

#ifndef EDITORTILEPALLETE
    #include "editortilepallete.h"
#endif

#ifndef FILE_GAME
	#include "../file/format.h"
#endif



extern int palleteX;
extern int palleteY;

struct pointElement{
   int palleteX, palleteY, type;
};

// used in copy/paste
struct st_tile_point {
    st_position_int8 tile1;
    st_position_int8 tile3;
    Sint8 locked;

    st_tile_point(st_position_int8 t1, st_position_int8 t3, Sint8 lk) {
        tile1 = t1;
        tile3 = t3;
        locked = lk;
    }
};

class EditorArea : public QWidget
{
  Q_OBJECT

public:
  // methods
  EditorArea(QWidget *parent = 0);

  // variables
  QWidget *myParent;

  void update_files();                          // rebuild image files when needed
  void update_map_data();                       // rebuild map data/tiles when needed
  void update_editarea_size();

private:
  void preload_slope_images();
  void draw_slope_tile(int x, int y, int dest_x, int dest_y, QPainter *painter);

private:

  // variables
  int link_pos_x;
  int link_pos_y;
  int link_map_origin;
  int link_size;
  e_LINK_DIRECTION link_direction;
  int total_editarea_w = 0;
  int total_editarea_h = 0;


protected:
    // methods
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    int find_npc_in_position(int x, int y);

    void drawTileset(QPainter *painter);
    void drawLockTileset(QPainter *painter);
    void drawMapEnemies(QPainter *painter);
    void drawMapObjects(QPainter *painter);

    // variables
    int temp;
    int editor_selectedTileX, editor_selectedTileY;
    int tempX, tempY;
    int editor_selected_object_pos;
    int editor_selected_object_pos_map;

    int map_backup_n;
    bool mouse_released;
    bool selection_started;

    int selection_start_x;
    int selection_start_y;
    std::vector<std::vector<st_tile_point> > selection_matrix;
    int selection_current_x;
    int selection_current_y;

    QPixmap hard_mode_tile;
    QPixmap easy_mode_tile;
    QPixmap tileset_image;
    QBitmap tileset_bitmap;
    QPixmap bg1_image;
    QPixmap fg_layer__image;
    float fg_opacity;

    std::map<int, QPixmap> slope_image_list;

    QPixmap layer_pixmap_list[LAYERS_COUNT];
    int leftmost_room = FILE_AREA_W;
    int rightmost_room = 0;
    int topmost_room = FILE_AREA_H;
    int bottommost_room = 0;

    v6_map_object* obj_ref = nullptr;


//signals:
//     void save();

public slots:
   void changeTile();
   void update_area_data();

};
