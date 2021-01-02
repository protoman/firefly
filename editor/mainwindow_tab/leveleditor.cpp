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
    for (int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        ui->areaComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(SharedData::get_instance()->v6_area_list.at(i).map_name));
    }
    ui->areaPreviewWidget->repaint();
    data_loading = false;
}



void LevelEditor::on_areaComboBox_currentIndexChanged(int index)
{
    data_loading = true;
    unsigned int currentArea = ui->areaComboBox->currentIndex();
    if (SharedData::get_instance()->v6_level_map.size() <= currentArea) {
        return;
    }
    ui->areaPreviewWidget->setCurrentArea(index);
    ui->areaPreviewWidget->repaint();
    data_loading = false;
}

