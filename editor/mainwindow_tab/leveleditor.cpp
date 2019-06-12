#include "leveleditor.h"
#include "ui_leveleditor.h"

#include "mediator.h"
#include "common.h"

LevelEditor::LevelEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditor)
{
    ui->setupUi(this);
}

LevelEditor::~LevelEditor()
{
    delete ui;
}

void LevelEditor::reload()
{
    data_loading = true;
    ui->areaComboBox->clear();
    for (int i=0; i<SharedData::get_instance()->v6_level_list.size(); i++) {
        ui->areaComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(SharedData::get_instance()->v6_level_list.at(i).level_name));
    }
    ui->mapComboBox->clear();
    for (int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        ui->mapComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(SharedData::get_instance()->v6_area_list.at(i).map_name));
    }
    data_loading = false;
}



void LevelEditor::on_addAreaPushButton_clicked()
{
    if (data_loading) { return; }
    SharedData::get_instance()->v6_level_list.push_back(file_v6_level());
    unsigned int new_n = SharedData::get_instance()->v6_level_list.size();
    sprintf(SharedData::get_instance()->v6_level_list.at(SharedData::get_instance()->v6_level_list.size()-1).level_name, "LEVEL %d", new_n);
    data_loading = true;
    reload();
    data_loading = false;
}

void LevelEditor::on_areaNameLineEdit_textChanged(const QString &arg1)
{
    if (data_loading) { return; }
    // record current combo selection to restore later //
    unsigned int currentLevel = ui->areaComboBox->currentIndex();
    unsigned int currentMap = ui->mapComboBox->currentIndex();
    sprintf(SharedData::get_instance()->v6_level_list.at(currentLevel).level_name, "%s", arg1.toStdString().c_str());
    data_loading = true;
    reload();
    // restore combo selection //
    ui->areaComboBox->setCurrentIndex(currentLevel);
    ui->mapComboBox->setCurrentIndex(currentMap);
    data_loading = false;
}

void LevelEditor::on_areaComboBox_currentIndexChanged(int index)
{
    data_loading = true;
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->v6_level_list.size() <= currentArea) {
        return;
    }
    ui->areaNameLineEdit->setText(SharedData::get_instance()->v6_level_list.at(currentArea).level_name);
    data_loading = false;
}




void LevelEditor::on_mapComboBox_currentIndexChanged(int index)
{
    if (data_loading) { return; }
    ui->areaPreviewWidget->setCurrentMap(index);
    ui->areaPreviewWidget->repaint();
}

void LevelEditor::on_addVerticalLinkPushButton_clicked()
{
    ui->areaPreviewWidget->set_edit_mode(AREA_EDIT_MODE_VLINK);
}

void LevelEditor::on_addHorizontalLinkPushButton_clicked()
{
    ui->areaPreviewWidget->set_edit_mode(AREA_EDIT_MODE_HLINK);
}
