#include "areaedittab.h"
#include "ui_areaedittab.h"

#include "mediator.h"
#include "common.h"


#include <QColorDialog>

AreaEditTab::AreaEditTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditTab)
{
    ui->setupUi(this);
    fill_data();
    set_data();
}

AreaEditTab::~AreaEditTab()
{
    delete ui;
}

void AreaEditTab::on_currentStageComboBox_currentIndexChanged(int index)
{
    if (index == -1) {
        return;
    }
    SharedData::get_instance()->v6_selected_stage = index;
    SharedData::get_instance()->v6_selected_area = 0;
    Mediator::get_instance()->load_area_rooms(SharedData::get_instance()->v6_selected_stage);
    set_data();
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();

    ui->area_rooms->setCurrentStage(index);
    ui->area_rooms->setCurrentArea(SharedData::get_instance()->v6_selected_area);
    fill_area_combo();
    ui->areaNameLineEdit->setText(GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).at(SharedData::get_instance()->v6_selected_area).area_name);
    ui->area_rooms->repaint();

}

void AreaEditTab::reload()
{
    fill_data();
}

void AreaEditTab::fill_data()
{
    is_loading = true;

    // MAP-FIELDS //
    ui->currentStageComboBox->clear();
    for (unsigned int i=0; i<GameData::get_instance()->v6_stage_list.size(); i++) {
        ui->currentStageComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(GameData::get_instance()->v6_stage_list.at(i).stage_name));
    }
    fill_area_combo();
    common::fill_files_combo("/music", ui->musicFilenameComboBox);
    common::fill_room_styles_combo(ui->styleComboBox);

    set_data();
    is_loading = false;
}

void AreaEditTab::set_data()
{
    // MAP-DATA //
    unsigned int currentMap = ui->currentStageComboBox->currentIndex();
    if (currentMap >= GameData::get_instance()->v6_stage_list.size()) {
        return;
    }

    bool must_reset_is_loading = false;
    if (is_loading == false) {
        is_loading = true;
        must_reset_is_loading = true;
    }

    ui->stageNameLineEdit->setText(GameData::get_instance()->v6_stage_list.at(currentMap).stage_name);
    ui->styleComboBox->setCurrentIndex(get_area_style_number());


    // LAYERS-DATA //
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();

    ui->musicFilenameComboBox->setCurrentIndex(ui->musicFilenameComboBox->findText(GameData::get_instance()->v6_stage_list.at(currentMap).music_filename));

    if (must_reset_is_loading) {
        is_loading = false;
    }

}

void AreaEditTab::fill_area_combo()
{
    if (GameData::get_instance()->v6_area_map.size() == 0) {
        return;
    }
    is_loading = true;
    SharedData::get_instance()->v6_selected_area = 0;
    ui->areaSelectionComboBox->clear();

    auto it = GameData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage);
    if (it != GameData::get_instance()->v6_area_map.end()) {
        for (unsigned int i=0; i<it->second.size(); i++) {
            ui->areaSelectionComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(it->second.at(i).area_name));
        }
    }

    ui->areaEditPreviewWidgetObj->repaint();
    is_loading = false;
}

int AreaEditTab::get_area_style_number()
{
    std::map<int, std::vector<file_v6_area>>::iterator it = GameData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage);
    if (it == GameData::get_instance()->v6_area_map.end()) {
        std::cout << "ERROR - Could not find area-list for stage[" << SharedData::get_instance()->v6_selected_stage << "]" << std::endl;
        exit(-1);
    }
    if (SharedData::get_instance()->v6_selected_area >= it->second.size()) {
        std::cout << "ERROR - Could not find area[" << SharedData::get_instance()->v6_selected_area << "]" << std::endl;
        exit(-1);
    }
    if (it->second.at(SharedData::get_instance()->v6_selected_area).style >= GameData::get_instance()->v6_style_list.size()) {
        std::cout << "ERROR - Could not find style[" << it->second.at(SharedData::get_instance()->v6_selected_area).style << "]" << std::endl;
        exit(-1);
    }
    return it->second.at(SharedData::get_instance()->v6_selected_area).style;
}


bool AreaEditTab::checkLoadingAndValid()
{
    if (is_loading) { return true; }
    unsigned int currentMap = ui->currentStageComboBox->currentIndex();
    if (currentMap >= GameData::get_instance()->v6_stage_list.size()) {
        return true;
    }
    return false;
}


void AreaEditTab::on_stageNameLineEdit_textEdited(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(GameData::get_instance()->v6_stage_list.at(ui->currentStageComboBox->currentIndex()).stage_name, "%s", arg1.toStdString().c_str());
    //fill_data();
    //ui->currentStageComboBox->setCurrentIndex(ui->currentStageComboBox->currentIndex());
}

void AreaEditTab::on_addAreaPushButton_clicked()
{
    GameData::get_instance()->v6_stage_list.push_back(file_v6_stage());
    int list_size = GameData::get_instance()->v6_stage_list.size();
    sprintf(GameData::get_instance()->v6_stage_list.at(list_size-1).stage_name, "STAGE [%d]", list_size);
    ui->currentStageComboBox->addItem(QString("[") + QString::number(list_size) + "] - " + QString(GameData::get_instance()->v6_stage_list.at(list_size-1).stage_name));
    Mediator::get_instance()->add_missing_areas();
}


void AreaEditTab::on_musicFilenameComboBox_currentIndexChanged(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(GameData::get_instance()->v6_stage_list.at(ui->currentStageComboBox->currentIndex()).music_filename, "%s", arg1.toStdString().c_str());
}

void AreaEditTab::on_styleComboBox_currentIndexChanged(int index)
{
    if (checkLoadingAndValid()) { return; }
    if (index == -1) {
        return;
    }
    std::map<int, std::vector<file_v6_area>>::iterator it = GameData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage);
    if (it == GameData::get_instance()->v6_area_map.end()) {
        std::cout << "ERROR - Could not find area-list for stage[" << SharedData::get_instance()->v6_selected_stage << "]" << std::endl;
        exit(-1);
    }
    if (SharedData::get_instance()->v6_selected_area >= it->second.size()) {
        std::cout << "ERROR - Could not find area[" << SharedData::get_instance()->v6_selected_area << "]" << std::endl;
        exit(-1);
    }
    if (index >= GameData::get_instance()->v6_style_list.size()) {
        std::cout << "ERROR - Could not find style[" << index << "]" << std::endl;
        exit(-1);
    }
    it->second.at(SharedData::get_instance()->v6_selected_area).style = index;
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}


void AreaEditTab::on_areaSelectionComboBox_currentIndexChanged(int index)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_selected_area = index;
    ui->areaNameLineEdit->setText(GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).at(SharedData::get_instance()->v6_selected_area).area_name);
    ui->area_rooms->setCurrentArea(SharedData::get_instance()->v6_selected_area);
    ui->styleComboBox->setCurrentIndex(get_area_style_number());
    ui->area_rooms->repaint();
}


void AreaEditTab::on_addAreaPushButton_2_clicked()
{
    Mediator::get_instance()->add_missing_areas();
    GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).push_back(file_v6_area());
    int list_size = GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).size();
    sprintf(GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).at(list_size-1).area_name, "AREA [%d]", list_size);
    ui->areaSelectionComboBox->addItem(QString("[") + QString::number(list_size) + "] - " + QString(GameData::get_instance()->v6_area_map.at(SharedData::get_instance()->v6_selected_stage).at(list_size-1).area_name));
}

