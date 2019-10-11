#include "game_properties_tab.h"
#include "ui_game_properties_tab.h"
#include "common.h"

#include "mediator.h"

game_properties_tab::game_properties_tab(QWidget *parent) : QWidget(parent), ui(new Ui::game_properties_tab), _data_loading(false), _current_stage(0)
{
    ui->setupUi(this);
    fill_data();
}

game_properties_tab::~game_properties_tab()
{
    delete ui;
}

void game_properties_tab::reload()
{
    fill_data();
}

void game_properties_tab::fill_data()
{
    if (SharedData::get_instance()->FILEPATH.length() == 0) {
        return;
    }
    _data_loading = true;
    _current_stage = 0;
    common::fill_projectiles_combo(ui->semicharged_shot_combo);
    ui->stagefaces_stage_combo->setCurrentIndex(_current_stage);
    common::fill_graphicfiles_combobox(std::string("images/faces"), ui->stagefaces_face_combo);
    common::fill_object_combo(ui->special_item1_combo);
    common::fill_object_combo(ui->special_item2_combo);
    common::fill_npc_combo(ui->finalBoss_comboBox);

    // set values/positions
    ui->semicharged_shot_combo->setCurrentIndex(SharedData::get_instance()->game_data.semi_charged_projectile_id);
    ui->finalBoss_comboBox->setCurrentIndex(SharedData::get_instance()->game_data.final_boss_id);


    ui->lineEdit->setText(SharedData::get_instance()->game_data.name);

    // file-format 4.0 fields
    common::fill_files_combo(std::string("/music"), ui->bossMusic_comboBox);
    common::fill_files_combo(std::string("/music"), ui->finalBossMusic_comboBox);
    common::fill_files_combo(std::string("/music"), ui->gotWeaponMusic_comboBox);
    common::fill_files_combo(std::string("/music"), ui->gameOverMusic_comboBox);
    common::fill_files_combo(std::string("/music"), ui->stageSelectMusic_comboBox);
    common::fill_files_combo(std::string("/music"), ui->gameStartMusic_comboBox);


    ui->gameStyle_comboBox->setCurrentIndex(SharedData::get_instance()->game_data.game_style);


    if (SharedData::get_instance()->game_data.use_second_castle == true) {
        ui->stageNumber_comboBox->setCurrentIndex(1);
    } else {
        ui->stageNumber_comboBox->setCurrentIndex(0);
    }

    _data_loading = false;
}


void game_properties_tab::on_semicharged_shot_combo_currentIndexChanged(int index)
{
    if (_data_loading == true) return;
    SharedData::get_instance()->game_data.semi_charged_projectile_id = index;
}



void game_properties_tab::on_lineEdit_textChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.name, "%s", arg1.toStdString().c_str());

}

void game_properties_tab::on_gameStyle_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) return;
    SharedData::get_instance()->game_data.game_style = index;
}

void game_properties_tab::on_bossMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.boss_music_filename, "%s", arg1.toStdString().c_str());
}

void game_properties_tab::on_finalBossMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.final_boss_music_filename, "%s", arg1.toStdString().c_str());
}


void game_properties_tab::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.game_start_screen_music_filename, "%s", arg1.toStdString().c_str());
}

void game_properties_tab::on_gotWeaponMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.got_weapon_music_filename, "%s", arg1.toStdString().c_str());

}

void game_properties_tab::on_gameOverMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.game_over_music_filename, "%s", arg1.toStdString().c_str());
}

void game_properties_tab::on_stageSelectMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.stage_select_music_filename, "%s", arg1.toStdString().c_str());
}

void game_properties_tab::on_stageNumber_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) return;
    if (index == 0) {
        SharedData::get_instance()->game_data.use_second_castle = false;
    } else {
        SharedData::get_instance()->game_data.use_second_castle = true;
    }
}


void game_properties_tab::on_gameStartMusic_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) return;
    sprintf(SharedData::get_instance()->game_data.game_start_screen_music_filename, "%s", arg1.toStdString().c_str());
}


void game_properties_tab::on_finalBoss_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) return;
    SharedData::get_instance()->game_data.final_boss_id = index;
}


