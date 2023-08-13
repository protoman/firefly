#include "npcedittab.h"
#include "ui_npcedittab.h"
#include "data/shareddata.h"
#include "common.h"

NPCEditTab::NPCEditTab(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NPCEditTab)
{
    ui->setupUi(this);
    fill_data();
}

NPCEditTab::~NPCEditTab()
{
    delete ui;
}

void NPCEditTab::reload()
{
    fill_data();
}

void NPCEditTab::fill_data()
{
    data_loading = true;
    common::fill_enemies_combo(ui->npcSelectorComboBox);
    common::fill_files_combo("images/sprites/npcs", ui->graphicsComboBox);
    set_data();
    data_loading = false;
}

void NPCEditTab::set_data()
{
    bool old_data_loading = data_loading; // prevents setting data_loading to false before needed
    data_loading = true;

    if (SharedData::get_instance()->npc_list.size() <= SharedData::get_instance()->selected_npc) {
        return;
    }

    ui->nameLineEdit->setText(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).name);
    ui->graphicsComboBox->setCurrentIndex(ui->graphicsComboBox->findText((SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).graphic_filename)));
    ui->moveBehaviorComboBox->setCurrentIndex(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_move_behavior);
    ui->moveSpeedSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).speed);
    ui->moveRangeSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).walk_range);
    ui->dialogSelectorComboBox->setCurrentIndex(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_dialog_id);
    ui->questSelectorComboBox->setCurrentIndex(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_quest_id);
    ui->frameWidthSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).frame_width);
    ui->animationFrameDurationSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).frame_duration);
    ui->hitboxXSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.x);
    ui->hitboxYSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.y);
    ui->hitboxWSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.w);
    ui->hitboxHSpinBox->setValue(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.h);

    data_loading = old_data_loading;
}

void NPCEditTab::on_pushButton_clicked()
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.push_back(file_npc_v3_1_2());
    fill_data();
}


void NPCEditTab::on_npcSelectorComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->selected_npc = index;
    set_data();
}


void NPCEditTab::on_nameLineEdit_textChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).name, "%s", arg1.toStdString().c_str());
}


void NPCEditTab::on_graphicsComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).graphic_filename, "%s", arg1.toStdString().c_str());
}


void NPCEditTab::on_moveSpeedSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).speed = arg1;
}


void NPCEditTab::on_moveRangeSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).walk_range = arg1;
}


void NPCEditTab::on_animationFrameDurationSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).frame_duration = arg1;
}


void NPCEditTab::on_frameWidthSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).frame_width = arg1;
}


void NPCEditTab::on_hitboxXSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.x = arg1;
}


void NPCEditTab::on_hitboxYSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.y = arg1;
}


void NPCEditTab::on_hitboxWSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.w = arg1;
}


void NPCEditTab::on_hitboxHSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).hit_area.h = arg1;
}


void NPCEditTab::on_dialogSelectorComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_dialog_id = index;
}


void NPCEditTab::on_questSelectorComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_quest_id = index;
}


void NPCEditTab::on_moveBehaviorComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->npc_list.at(SharedData::get_instance()->selected_npc).npc_move_behavior = index;
}

