#include "slopeedittab.h"
#include "ui_slopeedittab.h"
#include "game_data.h"


SlopeEditTab::SlopeEditTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlopeEditTab)
{
    ui->setupUi(this);
    QObject::connect(ui->slopePreviewWidget, SIGNAL(currentPosChanged(int)), this, SLOT(currentPosChanged(int)));
    fill_data();
}

SlopeEditTab::~SlopeEditTab()
{
    delete ui;
}

void SlopeEditTab::reload()
{
    fill_data();
}

void SlopeEditTab::fill_data()
{
    data_loading = true;
    ui->currentSlopeComboBox->clear();
    common::fill_files_combo("/images/tilesets/slope", ui->imageComboBox);

    if (GameData::get_instance()->slope_list.size() == 0) {
        data_loading = false;
        return;
    }
    for (int i=0; i<GameData::get_instance()->slope_list.size(); i++) {
        QString text = QString("[") + QString::number(i+1) + QString ("]");
        ui->currentSlopeComboBox->addItem(text);
    }
    ui->left_spinBox->setValue(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].left);
    ui->right_spinBox->setValue(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].right);
    ui->left_spinBox->setValue(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].left);
    ui->right_spinBox->setValue(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].right);

    ui->slopePreviewWidget->setLeft(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].left);
    ui->slopePreviewWidget->setRight(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).slope[0].right);

    data_loading = false;
    ui->imageComboBox->setCurrentIndex(ui->imageComboBox->findText(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).filename));
}

void SlopeEditTab::on_addSlopePushButton_clicked()
{
    GameData::get_instance()->slope_list.push_back(file_v5_slope_tile());
    fill_data();
}

void SlopeEditTab::on_currentSlopeComboBox_currentIndexChanged(int index)
{
    if (data_loading) { return; }
    currentSlopePos = index;
    ui->slopePreviewWidget->setRight(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].right);
    ui->slopePreviewWidget->setLeft(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].left);
    data_loading = true;
    ui->left_spinBox->setValue(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].left);
    ui->right_spinBox->setValue(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].right);
    data_loading = false;
    ui->slopePreviewWidget->repaint();

    ui->imageComboBox->setCurrentIndex(ui->imageComboBox->findText(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).filename));
}

void SlopeEditTab::on_imageComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) { return; }

    sprintf(GameData::get_instance()->slope_list.at(ui->currentSlopeComboBox->currentIndex()).filename, "%s", arg1.toStdString().c_str());
    ui->slopePreviewWidget->setImage(arg1.toStdString());
    ui->slopePreviewWidget->repaint();
    if (arg1.length() > 0) {
        ui->left_spinBox->setEnabled(true);
        ui->right_spinBox->setEnabled(true);
    } else {
        ui->left_spinBox->setEnabled(false);
        ui->right_spinBox->setEnabled(false);
    }
}

void SlopeEditTab::on_left_spinBox_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].left = arg1;

    ui->slopePreviewWidget->setLeft(arg1);
    ui->slopePreviewWidget->repaint();
}

void SlopeEditTab::on_right_spinBox_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].right = arg1;

    ui->slopePreviewWidget->setRight(arg1);
    ui->slopePreviewWidget->repaint();
}

void SlopeEditTab::currentPosChanged(int pos)
{
    if (data_loading) { return; }
    selectedSlopePos = pos;
    ui->left_spinBox->setValue(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].left);
    ui->right_spinBox->setValue(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].right);


    ui->slopePreviewWidget->setLeft(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].left);
    ui->slopePreviewWidget->setRight(GameData::get_instance()->slope_list.at(currentSlopePos).slope[selectedSlopePos].right);
    ui->slopePreviewWidget->repaint();
}
