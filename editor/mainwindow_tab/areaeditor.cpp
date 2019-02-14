#include "areaeditor.h"
#include "ui_areaeditor.h"

#include "mediator.h"
#include "common.h"

AreaEditor::AreaEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditor)
{
    ui->setupUi(this);
}

AreaEditor::~AreaEditor()
{
    delete ui;
}

void AreaEditor::reload()
{
    data_loading = true;
    //common::fill_files_combo("/music/", ui->musicComboBox);
    data_loading = false;
    if (SharedData::get_instance()->area_list.size() == 0) {
        ui->addMapPushButton->setEnabled(false);
        ui->mapComboBox->setEnabled(false);
        ui->areaNameLineEdit->setEnabled(false);
    } else {
        ui->addMapPushButton->setEnabled(true);
        ui->mapComboBox->setEnabled(true);
        ui->areaNameLineEdit->setEnabled(true);
    }
    ui->areaComboBox->clear();
    for (int i=0; i<SharedData::get_instance()->area_list.size(); i++) {
        ui->areaComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(SharedData::get_instance()->area_list.at(i).name));
    }
    int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->area_list.size() > currentArea) {
        //ui->graphic_combo->setCurrentIndex(ui->graphic_combo->findText(QString(Mediator::get_instance()->anim_block_list.at(index).filename)));
        unsigned int new_index = ui->musicComboBox->findText(QString(SharedData::get_instance()->area_list.at(currentArea).music_filename));
        std::cout << ">>>>>>>>>>>> LOAD music_filename, new_index[" << new_index << ", [" << SharedData::get_instance()->area_list.at(currentArea).music_filename << "]" << std::endl;
        ui->musicComboBox->setCurrentIndex(new_index);
    }
    reload_map_combo();
}

void AreaEditor::reload_map_combo()
{
    std::cout << "reload_map_combo" << std::endl;
    ui->mapComboBox->clear();
    int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->area_list.size() <= currentArea) {
        std::cout << "reload_map_combo - no areas" << std::endl;
        return;
    }
    for (int i=0; i<GAME_AREA_SIZE; i++) {
        if (SharedData::get_instance()->area_list.at(currentArea).map[i] != -1) {
            std::cout << "found map at[" << i << "]" << std::endl;
            ui->mapComboBox->addItem(QString("[") + QString::number(i) + "]");
        }
    }
}



void AreaEditor::on_addAreaPushButton_clicked()
{
    if (data_loading) { return; }
    SharedData::get_instance()->area_list.push_back(struct_file_v5_area());
    sprintf(SharedData::get_instance()->area_list.at(SharedData::get_instance()->area_list.size()-1).name, "%s", "NEW-AREA");
    data_loading = true;
    reload();
    data_loading = false;
}

void AreaEditor::on_addMapPushButton_clicked()
{
    std::cout << "on_addMapPushButton_clicked" << std::endl;
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    for (int i=0; i<GAME_AREA_SIZE; i++) {
        if (SharedData::get_instance()->area_list.at(currentArea).map[i] == -1) {
            std::cout << "ADDED MAP AT[" << i << "]" << std::endl;
            SharedData::get_instance()->area_list.at(currentArea).map[i] = i;
            break;
        }
    }
    data_loading = true;
    reload_map_combo();
    data_loading = false;
}

void AreaEditor::on_areaNameLineEdit_textChanged(const QString &arg1)
{
    if (data_loading) { return; }
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    unsigned int currentMap = ui->mapComboBox->currentIndex();
    sprintf(SharedData::get_instance()->area_list.at(SharedData::get_instance()->area_list.size()-1).name, "%s", arg1.toStdString().c_str());
    data_loading = true;
    reload();
    ui->areaComboBox->setCurrentIndex(currentArea);
    ui->mapComboBox->setCurrentIndex(currentMap);
    data_loading = false;
}

void AreaEditor::on_areaComboBox_currentIndexChanged(int index)
{
    data_loading = true;
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->area_list.size() <= currentArea) {
        return;
    }
    ui->areaNameLineEdit->setText(SharedData::get_instance()->area_list.at(currentArea).name);
    reload_map_combo();
    data_loading = false;
}


void AreaEditor::on_musicComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) { return; }
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->area_list.size() <= currentArea) {
        return;
    }
    sprintf(SharedData::get_instance()->area_list.at(currentArea).music_filename, "%s", arg1.toStdString().c_str());
    std::cout << ">>>>>>>>>>>> SET music_filename[" << SharedData::get_instance()->area_list.at(currentArea).music_filename << "]" << std::endl;
}


void AreaEditor::on_generateTilesetPushButton_clicked()
{
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    unsigned int currentMap = ui->mapComboBox->currentIndex() + (currentArea*GAME_AREA_SIZE);
    if (SharedData::get_instance()->area_list.size() <= currentArea) {
        return;
    }

    std::cout << "AreaEditor::on_generateTilesetPushButton_clicke - currentArea[" << currentArea << "], currentMap[" << currentMap << "]" << std::endl;


    st_position top_leftmost_point = st_position(GAME_AREA_SIZE, GAME_AREA_SIZE);
    st_position bottom_rightmost_point = st_position(0, 0);
    // find left top-leftmost and bottom rightmost points
    for (int i=0; i<GAME_AREA_SIZE; i++) {
        for (int j=0; j<GAME_AREA_SIZE; j++) {
            if (SharedData::get_instance()->area_list.at(currentArea).point[i][j] == currentMap) {
                if (top_leftmost_point.x >= i && top_leftmost_point.y >= j) {
                    top_leftmost_point.x = i;
                    top_leftmost_point.y = j;
                }
                if (bottom_rightmost_point.x <= i && bottom_rightmost_point.y <= j) {
                    bottom_rightmost_point.x = i;
                    bottom_rightmost_point.y = j;
                }
            }
        }
    }
    std::cout << "top_leftmost_point.x[" << top_leftmost_point.x << "], y[" << top_leftmost_point.y << "]" << std::endl;
    std::cout << "bottom_rightmost_point.x[" << bottom_rightmost_point.x << "], y[" << bottom_rightmost_point.y << "]" << std::endl;

    // +1 because count starts in zero
    unsigned int size_w = bottom_rightmost_point.x - top_leftmost_point.x + 1;
    unsigned int size_h = bottom_rightmost_point.y - top_leftmost_point.y + 1;

    int total_size_w = size_w*GAME_AREA_W;
    int total_size_h = size_h*GAME_AREA_H;
    std::cout << "size_w[" << size_w << "], size_h[" << size_h << "]" << std::endl;
    std::cout << "total_size_w[" << total_size_w << "], total_size_h[" << total_size_h << "]" << std::endl;

    // already have header and tileset
    if (SharedData::get_instance()->file_v5_map_header_list.size() > currentMap && SharedData::get_instance()->file_v5_map_tile_map.find(currentMap) != SharedData::get_instance()->file_v5_map_tile_map.end()) {
        // TODO: if size was not changed, must only set the enabled/disabled tiles
        SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_w = total_size_w;
        SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_h = total_size_h;
        SharedData::get_instance()->file_v5_map_tile_map.at(currentMap).clear();


        std::cout << ">>>>>>>>>>>> currentMap[" << currentMap << "]" << std::endl;
        // TODO: set empty areas (also, add support in editor for those)


        int map_w = SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_w;
        int map_h = SharedData::get_instance()->file_v5_map_header_list.at(currentMap).tiles_h;

        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                SharedData::get_instance()->file_v5_map_tile_map.at(currentMap).push_back(file_v5_map_tile());
            }
        }
        std::cout << "MAP[" << currentMap << "], tiles[" << SharedData::get_instance()->file_v5_map_tile_map.at(currentMap).size() << "]" << std::endl;

        for (int i=0; i<map_w; i++) {
            for (int j=0; j<map_h; j++) {
                int n = j*map_w + i;
                if (SharedData::get_instance()->area_list.at(currentArea).point[i/GAME_AREA_W][j/GAME_AREA_H] == -1) {

                    file_v5_map_tile* tileItem = &SharedData::get_instance()->file_v5_map_tile_map.at(currentMap).at(n);

                    //std::cout << "SET-POIINT-EMPTY x[" << i << "], y[" << j << "], n[" << n << "]" << std::endl;

                    tileItem->locked = -2;
                    tileItem->tile_underlay.x = -2;
                    tileItem->tile_underlay.y = -2;
                    tileItem->tile_underlay.type = TILE_TYPE_UNUSED;
                }
            }
        }

    }

    //GAME_AREA_W

}

void AreaEditor::on_mapComboBox_currentIndexChanged(int index)
{
    if (data_loading) { return; }
    ui->areaPreviewWidget->setCurrentMap(index);
    ui->areaPreviewWidget->repaint();
}

void AreaEditor::on_addVerticalLinkPushButton_clicked()
{
    ui->areaPreviewWidget->set_edit_mode(AREA_EDIT_MODE_VLINK);
}

void AreaEditor::on_addHorizontalLinkPushButton_clicked()
{
    ui->areaPreviewWidget->set_edit_mode(AREA_EDIT_MODE_HLINK);
}
