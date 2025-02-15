#include "npcpreviewarea.h"

#include <QPainter>
#include "game_data.h"

NpcPreviewArea::NpcPreviewArea(QWidget *parent) : QWidget(parent)
{
    my_parent = parent;
}

void NpcPreviewArea::set_anim_type(int type)
{
    animation_type = type;
}

void NpcPreviewArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLineF line;
    QRectF target, source;
    int i;

    if (GameData::get_instance()->npc_list.size() <= SharedData::get_instance()->selected_npc) {
        std::cout << "NpcPreviewArea::paintEvent::LEAVE#1" << std::endl;
        return;
    }

    std::string graphic_filename = SharedData::get_instance()->FILEPATH + std::string("/images/sprites/npcs/") + std::string(GameData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).graphic_filename);
    if (graphic_filename.find(".png") == std::string::npos) {
        std::cout << "NpcPreviewArea::paintEvent::LEAVE#2" << std::endl;
        return;
    }

    QPixmap image(graphic_filename.c_str());
    if (image.isNull() == true || image.width() <= 0) {
        std::cout << "NpcPreviewArea::paintEvent::LEAVE#3" << std::endl;
        return;
    }

    file_npc_v3_1_2 npc_data = GameData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc);
    int img_grid_w = npc_data.frame_width;
    int img_grid_h = image.height();

    image = image.scaled(image.width()*2, image.height()*2);
    this->resize(QSize(image.width()+1, image.height()+1));

    my_parent->adjustSize();

    target = QRectF(QPoint(0, 0), QSize(image.width(), image.height()));
    source = QRectF(QPoint(0, 0), QSize(image.width(), image.height()));
    painter.drawPixmap(target, image, source);

    painter.setPen(QColor(0, 120, 0));
    for (i=0; i<=this->width(); i=i+img_grid_w*2) {
        // linhas verticais
        line = QLineF(i, 0, i, this->height());
        painter.drawLine(line);
    }
    for (i=0; i<this->height(); i=i+img_grid_h*2) {
        // linhas horizontais
        line = QLineF(0, i, this->width(), i);
        painter.drawLine(line);
    }

    // draw hitbox

    // draw hit-area
    painter.setBrush(QColor(255, 0, 0, 50));
    if (npc_data.hit_area.x != 0 || npc_data.hit_area.y != 0 || npc_data.hit_area.w != 0 || npc_data.hit_area.h != 0) {
        int hitx = npc_data.hit_area.x*2;
        int hity = npc_data.hit_area.y*2;
        int hitw = npc_data.hit_area.w*2;
        int hith = npc_data.hit_area.h*2;
        painter.drawRect(hitx, hity, hitw, hith);
    }

}
