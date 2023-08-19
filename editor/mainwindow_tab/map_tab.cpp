#include "map_tab.h"
#include "ui_map_tab.h"
#include "common.h"

#include <QColorDialog>

map_tab::map_tab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::map_tab),
    _data_loading(true)
{
    ui->setupUi(this);

    fill_data();

    ui->editTile_button->setChecked(true);
    ui->editModeNormal_button->setChecked(true);


    ui->editArea->repaint();

    Mediator::get_instance()->currentDifficulty = DIFFICULTY_EASY;
    Mediator::get_instance()->currentDifficultyMode = DIFFICULTY_MODE_GREATER;
}

map_tab::~map_tab()
{
    delete ui;
}

void map_tab::reload()
{
    SharedData::get_instance()->v6_selected_area = 0;
    ui->animTilePaletteWidget->reload();
    ui->editArea->update_files();

    properties_hidden = true;
    ui->editArea->update_map_data();
    file_v6_style style = SharedData::get_instance()->v6_style_list.at(SharedData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).style);
    std::string pallete_filename = style.tileset_filename;
    if (pallete_filename.length() > 0) {
        Mediator::get_instance()->setPallete(pallete_filename);
    }

    fill_data();
}

void map_tab::set_current_box(short n)
{
    ui->toolBox->setCurrentIndex(n);
    update_edit_area();
}

void map_tab::update_edit_area()
{
    ui->editArea->update_files();
    ui->editArea->repaint();
}

void map_tab::fill_data()
{
    if (SharedData::get_instance()->FILEPATH.length() == 0) {
        return;
    }
    _data_loading = true;
    common::fill_npc_listwidget(ui->npc_listWidget);
    common::fill_object_listWidget(ui->objectListWidget);
    ui->npc_direction_combo->setCurrentIndex(Mediator::get_instance()->npc_direction);
    ui->object_direction_combo->setCurrentIndex(Mediator::get_instance()->object_direction);

    fill_map_selector();
    fill_area_combo();

    _data_loading = false;
}

void map_tab::fill_area_combo()
{
    if (SharedData::get_instance()->v6_area_map.size() == 0) {
        return;
    }
    _data_loading = true;
    SharedData::get_instance()->v6_selected_area = 0;
    ui->areaSelector_comboBox->clear();

    auto it = SharedData::get_instance()->v6_area_map.find(SharedData::get_instance()->v6_selected_stage);
    if (it != SharedData::get_instance()->v6_area_map.end()) {
        for (unsigned int i=0; i<it->second.size(); i++) {
            ui->areaSelector_comboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(it->second.at(i).area_name));
        }
    }

    ui->editArea->repaint();
    _data_loading = false;
}

void map_tab::on_comboBox_currentIndexChanged(int index)
{
    int value = 1;
    if (index == 1) {
        value = 3;
    }
    Mediator::get_instance()->layerLevel = value;
    update_edit_area();

}

void map_tab::on_listWidget_2_currentRowChanged(int currentRow)
{
    Mediator::get_instance()->terrainType = currentRow+1;
}

void map_tab::on_npc_listWidget_currentRowChanged(int currentRow)
{
    Mediator::get_instance()->selectedNPC = currentRow;
}

void map_tab::on_npc_direction_combo_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->npc_direction = index;
}


void map_tab::on_objectListWidget_currentRowChanged(int currentRow)
{
    Mediator::get_instance()->selectedNPC = currentRow;
}



void map_tab::on_object_direction_combo_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->object_direction = index;
}

void map_tab::on_editTile_button_clicked()
{
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);
    ui->editModeSlope_Button->setChecked(false);

    //ui->editModeNormal_button->setEnabled(true);
    ui->editModeLock_button->setEnabled(true);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(0);
    Mediator::get_instance()->editMode = EDITMODE_NORMAL;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_editObject_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editObject_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);
    ui->editModeSlope_Button->setChecked(false);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(4);
    Mediator::get_instance()->editMode = EDITMODE_OBJECT;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
    update_edit_area();
}

void map_tab::on_editLink_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeSlope_Button->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(3);
    Mediator::get_instance()->editMode = EDITMODE_LINK;
    Mediator::get_instance()->editTool = EDITMODE_LINK;
    update_edit_area();
}

void map_tab::on_editEnemy_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editEnemy_button->setChecked(true);
    ui->editNPC_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);
    ui->editModeSlope_Button->setChecked(false);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(2);
    Mediator::get_instance()->editMode = EDITMODE_ENEMY;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_editSetSubBoss_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(2);
    Mediator::get_instance()->editMode = EDITMODE_SET_SUBBOSS;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_editSetBoss_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editEnemy_button->setChecked(false);
    ui->editSetBoss_button->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(2);
    Mediator::get_instance()->editMode = EDITMODE_SET_BOSS;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}


void map_tab::on_editModeNormal_button_clicked()
{
    ui->editModeNormal_button->setChecked(true);
    ui->editModeLock_button->setChecked(false);
    ui->editModeErase_button->setChecked(false);

    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);

    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
    if (ui->editTile_button->isChecked()) {
        set_current_box(0);
    } else if (ui->editEnemy_button->isChecked()) {
        set_current_box(2);
    } else if (ui->editObject_button->isChecked()) {
        set_current_box(4);
    } else if (ui->editLink_button->isChecked()) {
        set_current_box(3);
    }
    update_edit_area();

}

void map_tab::on_editModeLock_button_clicked()
{
    ui->editModeNormal_button->setChecked(false);
    ui->editModeLock_button->setChecked(true);
    ui->editModeErase_button->setChecked(false);

    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);

    set_current_box(1);
    Mediator::get_instance()->editTool = EDITMODE_LOCK;
    update_edit_area();
}

void map_tab::on_editModeErase_button_clicked()
{
    ui->editModeNormal_button->setChecked(false);
    ui->editModeLock_button->setChecked(false);
    ui->editModeErase_button->setChecked(true);

    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);

    Mediator::get_instance()->editTool = EDITMODE_ERASER;
    update_edit_area();

}




void map_tab::on_addAnimTile_toolButton_clicked()
{
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);
    ui->editModeSlope_Button->setChecked(false);

    //ui->editModeNormal_button->setEnabled(true);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(5);
    Mediator::get_instance()->editMode = EDITMODE_ANIM_TILE;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_toolButton_clicked()
{
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);

    ui->editModeNormal_button->setEnabled(false);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(0);
    Mediator::get_instance()->editMode = EDITMODE_SELECT;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_paste_toolButton_clicked()
{
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);

    ui->editModeNormal_button->setEnabled(false);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(0);
    Mediator::get_instance()->editMode = EDITMODE_PASTE;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_editModeSlope_Button_clicked()
{
    ui->editEnemy_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeSlope_Button->setChecked(true);

    ui->editModeNormal_button->setEnabled(false);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(6);
    Mediator::get_instance()->editMode = EDITMODE_SLOPE;
    std::cout << "SET-SLOPE-MODE [" << Mediator::get_instance()->editMode << "]" << std::endl;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}


void map_tab::fill_map_selector()
{
    ui->mapSelector_comboBox->clear();
    for (unsigned int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        QString itemName = QString("[") + QString::number(i) + QString("] - ") + QString(SharedData::get_instance()->v6_stage_list.at(i).stage_name);
        ui->mapSelector_comboBox->addItem(itemName);
    }
    ui->mapSelector_comboBox->setCurrentIndex(SharedData::get_instance()->v6_selected_stage);
}


void map_tab::on_mapSelector_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true || index == -1) { return; }
    file_v6_style style = SharedData::get_instance()->v6_style_list.at(SharedData::get_instance()->v6_stage_list.at(SharedData::get_instance()->v6_selected_stage).style);
    SharedData::get_instance()->v6_selected_stage = index;
    _data_loading = true;
    Mediator::get_instance()->load_area_rooms(SharedData::get_instance()->v6_selected_stage);
    fill_data();
    ui->editArea->update_map_data();
    Mediator::get_instance()->setPallete(style.tileset_filename);
    ui->editArea->repaint();
    _data_loading = false;
}







void map_tab::on_areaSelector_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true || index == -1) { return; }
    SharedData::get_instance()->v6_selected_area = index;
    ui->editArea->repaint();
}


void map_tab::on_editNPC_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editEnemy_button->setChecked(false);
    ui->editNPC_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);
    ui->editModeSlope_Button->setChecked(false);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(7);
    Mediator::get_instance()->editMode = EDITMODE_NPC;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

