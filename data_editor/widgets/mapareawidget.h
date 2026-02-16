#ifndef MAPAREAWIDGET_H
#define MAPAREAWIDGET_H

#include <list>
#include <QWidget>
#include "../../tiled/TiledWorldLoader.hpp"

class MapAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapAreaWidget(QWidget *parent = nullptr);
    void loadMapData();

protected:
    // methods
    void paintEvent(QPaintEvent *event);

private:
    void loadMapImages();
    int findTilesetByTileNumber(int tileNumber) const;

private:
    QWidget *myParent;
    int zoom = 1;
    int TILESIZE = 32;
    int MAP_W = 320;
    int MAP_H = 200;
    TiledWorldLoader worldLoader;
    std::map<int, e_tiled_world_layer_type> layers;
    std::map<int, tiled_world_image_layer_data> imageLayers;
    std::map<int, std::vector<tiled_world_tileset_origin_data>> tilesetLayers;
    std::vector<tiled_world_tileset_data> tilesetsInfo;
    std::vector<QPixmap> tilesetImages;
    std::map<int, QPixmap> imageLayerImagesMap;
    tiled_world_map_properties map_properties;


signals:
};

#endif // MAPAREAWIDGET_H
