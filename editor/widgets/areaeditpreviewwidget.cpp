#include "areaeditpreviewwidget.h"
#include "mediator.h"
#include "aux_tools/stringutils.h"


#include <QPainter>

areaEditPreviewWidget::areaEditPreviewWidget(QWidget *parent) : QWidget(parent)
{

}

void areaEditPreviewWidget::update_files()
{
    std::string filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/tilesets/") + SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).tileset_filename;
    if (filename_str.length() == 0) {
        tileset_image = QPixmap();
    } else {
        tileset_image = QPixmap(filename_str.c_str());
    }

    for (int i=0; i<LAYERS_COUNT; i++) {
        std::string layer_filename = std::string(SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].filename);
        if (layer_filename.length() == 0) {
            layer_pixmap_list[i] = QPixmap();
        } else {
            std::string filename_str = SharedData::get_instance()->FILEPATH + std::string("/images/map_backgrounds/") + SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].filename;
            filename_str = StringUtils::clean_filename(filename_str);
            //std::cout << ">>>>>>>> update_files.filename-bg[" << i << "]: [" << filename_str << "]" << std::endl;
            layer_pixmap_list[i] = QPixmap(filename_str.c_str());
        }
    }
}

void areaEditPreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (Mediator::get_instance()->show_background_color == true) {
        st_color bg_color = SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).background_color;
        QColor qbg_color = QColor(bg_color.r, bg_color.g, bg_color.b, 255);
        painter.fillRect(QRectF(0.0, 0.0, RES_W*Mediator::get_instance()->zoom, AREA_H*Mediator::get_instance()->zoom), qbg_color);
    }
    // draw backgrounds //
    if (Mediator::get_instance()->show_bg1 == true) {
        for (int i=0; i<LAYERS_COUNT; i++) {
            if (!layer_pixmap_list[i].isNull()) {
                //std::cout << ">>>>>>>>>> paintEvent.show_layer[" << i << "]" << std::endl;
                int bg_pos_y =SharedData::get_instance()->v6_area_list.at(SharedData::get_instance()->v6_selected_area).layers[i].adjust_y;
                int max_repeat = 2;
                for (int k=0; k<max_repeat; k++) {
                    QRectF pos_source(QPoint(0, 0), QSize(layer_pixmap_list[i].width(), layer_pixmap_list[i].height()));
                    QRectF pos_dest(QPoint(k*layer_pixmap_list[i].width()*Mediator::get_instance()->zoom, bg_pos_y*Mediator::get_instance()->zoom), QSize(layer_pixmap_list[i].width()*Mediator::get_instance()->zoom, layer_pixmap_list[i].height()*Mediator::get_instance()->zoom));
                    painter.drawPixmap(pos_dest, layer_pixmap_list[i], pos_source);
                }
            }
        }
    }
    // DRAW BORDER //
    painter.setPen(Qt::black);
    QRect rect = QRect(0, 0, RES_W*Mediator::get_instance()->zoom, AREA_H*Mediator::get_instance()->zoom);
    painter.drawRect(rect);
}

