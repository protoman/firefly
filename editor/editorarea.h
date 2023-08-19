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
#include <iostream>
#include <map>

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

protected:
    // methods
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
  void preload_slope_images();
  void draw_slope_tile(int x, int y, int dest_x, int dest_y, QPainter *painter);
  int check_area_links(int room_x, int room_y, int tile_x, int tile_y);
  void drawRoomLayers(QPainter *painter, std::map<st_position, file_v6_room>::iterator it, int map_pos_x, int map_pos_y);
  void drawTileset(QPainter *painter);
  void drawLockTileset(QPainter *painter);
  void drawMapEnemies(QPainter *painter);
  void drawMapNpcs(QPainter *painter);
  void drawStageObjects(QPainter *painter);
  void rebuild_stage_npcs_map();
  void drawDoorObject(QPainter *painter, v6_stage_object &object,QPixmap &image);
  void drawDoorPlacingTiles(QPainter *painter);

private:
    st_position door_start_point;
    st_position door_current_point;
    bool door_placing = false;
    bool door_placing_incorrect_point = false;
    // variables
    int link_pos_x;
    int link_pos_y;
    int link_map_origin;
    int link_size;
    e_LINK_DIRECTION link_direction;
    int total_editarea_w = 0;
    int total_editarea_h = 0;

    int topmost_point = 99999;
    int bottommost_point = -1;
    int leftmost_point = 99999;
    int rightmost_point = -1;

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
    QPixmap fg_layer_image;
    float fg_opacity;

    std::map<int, QPixmap> slope_image_list;

    QPixmap layer_pixmap_list[LAYERS_COUNT];

    v6_stage_object* obj_ref = nullptr;

    // ============================ NEW STYLE-MAP SYSTEM ============================= //
    std::map<int, QPixmap> style_map_tileset;
    std::map<int, std::vector<QPixmap>> style_map_layers;

//signals:
//     void save();

public slots:
   void changeTile();
   void update_area_data();

};
