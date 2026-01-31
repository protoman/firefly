#include "areaeditpreviewwidget.h"
#include "mediator.h"
#include "aux_tools/stringutils.h"


#include <QPainter>
#include <QBitmap>

areaEditPreviewWidget::areaEditPreviewWidget(QWidget *parent) : QWidget(parent)
{

}

void areaEditPreviewWidget::update_files()
{
    unsigned int style_n = GameData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage)->second.at(SharedData::get_instance()->v6_selected_area).style;
    if (style_n >= GameData::get_instance()->v6_style_list.size()) {
        style_n = 0;
    }

    std::cout << "### areaEditPreviewWidget - style_n[" << style_n << "]" << std::endl;

    file_v6_style style = GameData::get_instance()->v6_style_list.at(style_n);
    std::string filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/tilesets/") + style.tileset_filename;
    if (filename_str.length() == 0) {
        tileset_image = QPixmap();
    } else {
        tileset_image = QPixmap(filename_str.c_str());
    }

    for (int i=0; i<LAYERS_COUNT; i++) {
        std::string layer_filename = std::string(style.layers[i].filename);
        if (layer_filename.length() == 0) {
            layer_pixmap_list[i] = QPixmap();
        } else {
            std::string filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/map_backgrounds/") + style.layers[i].filename;
            filename_str = StringUtils::clean_filename(filename_str);
            layer_pixmap_list[i] = QPixmap(filename_str.c_str());
        }
    }
}

void areaEditPreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int style_n = GameData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage)->second.at(SharedData::get_instance()->v6_selected_area).style;
    file_v6_style style = GameData::get_instance()->v6_style_list.at(style_n);


    // DRAW BORDER //
    painter.setPen(Qt::black);
    painter.drawRect(QRect(0, 0, RES_W*Mediator::get_instance()->zoom, RES_H*Mediator::get_instance()->zoom));

    painter.setPen(QColor(0, 0, 200));

    if (Mediator::get_instance()->show_background_color == true) {
        st_color bg_color = style.background_color;
        QColor qbg_color = QColor(bg_color.r, bg_color.g, bg_color.b, 255);
        painter.fillRect(QRectF(0.0, 0.0, RES_W*Mediator::get_instance()->zoom, AREA_H*Mediator::get_instance()->zoom), qbg_color);
    }
    // draw backgrounds //
    if (Mediator::get_instance()->show_bg1 == true) {
        for (int i=0; i<LAYERS_COUNT; i++) {
            if (!layer_pixmap_list[i].isNull()) {
                //std::cout << ">>>>>>>>>> paintEvent.show_layer[" << i << "]" << std::endl;
                int bg_pos_y = style.layers[i].adjust_y;
                int max_repeat = 2;
                for (int k=0; k<max_repeat; k++) {
                    QRectF pos_source(QPoint(0, 0), QSize(layer_pixmap_list[i].width(), layer_pixmap_list[i].height()));
                    QRectF pos_dest(QPoint(k*layer_pixmap_list[i].width()*Mediator::get_instance()->zoom, bg_pos_y*Mediator::get_instance()->zoom), QSize(layer_pixmap_list[i].width()*Mediator::get_instance()->zoom, layer_pixmap_list[i].height()*Mediator::get_instance()->zoom));
                    painter.drawPixmap(pos_dest, layer_pixmap_list[i], pos_source);
                }
            }
        }
    }
    // draw borders
    if (!layer_pixmap_list[current_layer].isNull()) {
        //std::cout << ">>>>>>>>>> paintEvent.show_layer[" << i << "]" << std::endl;
        int bg_pos_y = style.layers[current_layer].adjust_y;
        int max_repeat = 2;
        for (int k=0; k<max_repeat; k++) {
            QRectF pos_source(QPoint(0, 0), QSize(layer_pixmap_list[current_layer].width(), layer_pixmap_list[current_layer].height()));
            QRectF pos_dest(QPoint(k*layer_pixmap_list[current_layer].width()*Mediator::get_instance()->zoom, bg_pos_y*Mediator::get_instance()->zoom), QSize(layer_pixmap_list[current_layer].width()*Mediator::get_instance()->zoom, layer_pixmap_list[current_layer].height()*Mediator::get_instance()->zoom));
            painter.drawRect(QRectF(
                                 QPoint(k*layer_pixmap_list[current_layer].width()*Mediator::get_instance()->zoom+1, bg_pos_y*Mediator::get_instance()->zoom+1),
                                 QSize(layer_pixmap_list[current_layer].width()*Mediator::get_instance()->zoom-4, layer_pixmap_list[current_layer].height()*Mediator::get_instance()->zoom-2))
                             );
        }
    }
}

