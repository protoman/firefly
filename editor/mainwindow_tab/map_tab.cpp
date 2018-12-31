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
    ui->animTilePaletteWidget->reload();
    ui->editArea->update_files();
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

void map_tab::on_color_selected1(QColor color)
{
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.r = color.red();
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.g = color.green();
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.b = color.blue();
    fill_background_list();
    update_edit_area();
}


void map_tab::fill_data()
{
    if (FILEPATH.length() == 0) {
        return;
    }
    _data_loading = true;
    fill_anim_tiles_data();
    common::fill_stages_combo(ui->stageListCombo);
    common::fill_map_list_combo(ui->mapListCombo);
    common::fill_npc_listwidget(ui->npc_listWidget);
    common::fill_object_listWidget(ui->objectListWidget);
    fill_background_list();
    ui->npc_direction_combo->setCurrentIndex(Mediator::get_instance()->npc_direction);
    ui->object_direction_combo->setCurrentIndex(Mediator::get_instance()->object_direction);

    fill_map_selector();
    fill_map_v5_data();
    common::fill_files_combo("images/tilesets", ui->v5_tileset_comboBox);
    if (Mediator::get_instance()->file_v5_map_header_list.size() > 0) {
        QString tilesetFilename(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tileset_filename);
        std::cout << "################### tilesetFilename[" << tilesetFilename.toStdString() << "]" << std::endl;
        if (tilesetFilename.length() > 0) {
            ui->v5_tileset_comboBox->setCurrentIndex(ui->v5_tileset_comboBox->findText(tilesetFilename));
            Mediator::get_instance()->setPallete(tilesetFilename.toStdString());
            ui->editArea->update_files();
            ui->editArea->repaint();
        } else {
            ui->v5_tileset_comboBox->setCurrentIndex(-1);
            Mediator::get_instance()->setPallete("default.png");
            ui->editArea->update_files();
            ui->editArea->repaint();
        }
    }
    for (int i=0; i<BACKGROUND_LAYERS_MAX; i++) {
        QString itemName = QString("[") + QString::number(i) + QString("] - ");
        if (i<=9) {
            itemName += QString("BACKGROUND #") + QString::number(i+1);
        } else {
            itemName += QString("FOREGROUND #") + QString::number(i-9);
        }
        ui->layerSelector_comboBox->addItem(itemName);
    }


    _data_loading = false;
}

void map_tab::fill_background_list()
{

    // BACKGROUND //
    QString bg1_filename(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].filename);
    common::fill_files_combo("images/map_backgrounds", ui->bg1_filename);

    set_layer_data();

    std::stringstream ss;
    ss.str(std::string());
    ss << "background-color: rgb(" << Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.r << ", " << Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.g << ", " << Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].background_color.b << ")";
    ui->bg_color_pick->setStyleSheet(ss.str().c_str());

    ui->mapGFX_comboBox->setCurrentIndex(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].gfx);


    // FOREGROUND //
    QString fg_layer_filename(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].filename);

    ui->mapGFXMode_comboBox->setCurrentIndex(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].auto_scroll);


    CURRENT_FILE_FORMAT::file_stage temp_stage = Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage];
    bool autoscroll_checked = (bool)temp_stage.autoscroll[Mediator::get_instance()->currentMap];
    if (autoscroll_checked != true) {
        autoscroll_checked = false;
    }
    ui->mapAutoScroll_checkBox->setChecked(autoscroll_checked);



}

void map_tab::fill_anim_tiles_data()
{
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

void map_tab::on_link_orientation_combobox_currentIndexChanged(int index)
{
    Mediator::get_instance()->link_type = index;
}

void map_tab::on_link_bidi_checkbox_toggled(bool checked)
{
    Mediator::get_instance()->link_bidi = checked;
}

void map_tab::on_link_isdoor_checkbox_toggled(bool checked)
{
    Mediator::get_instance()->link_is_door = checked;
}

void map_tab::on_objectListWidget_currentRowChanged(int currentRow)
{
    Mediator::get_instance()->selectedNPC = currentRow;
}

void map_tab::on_bg_color_pick_clicked()
{
    QColorDialog *colorDialog = new QColorDialog(this);
    QObject::connect(colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(on_color_selected1(QColor)));
    colorDialog->show();
}

void map_tab::on_bg1_filename_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) { return; }
    if (arg1.toStdString() == std::string("None")) {
        Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].filename[0] = '\0';
    } else {
        sprintf(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].filename, "%s", arg1.toStdString().c_str());
    }
    update_edit_area();
}

void map_tab::on_bg1_speed_valueChanged(double arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].speed = arg1*10;
    std::cout << "#3 *** on_bg1_speed_valueChanged - setvalue: " << arg1 << ", bg1.speed: " << (int)Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].speed << std::endl;
    update_edit_area();
}

void map_tab::on_bg1_y_pos_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].adjust_y = arg1;
    update_edit_area();
}


void map_tab::on_checkBox_toggled(bool checked)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->show_background_color = checked;
    update_edit_area();
}

void map_tab::on_checkBox_2_toggled(bool checked)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->show_bg1 = checked;
    update_edit_area();
}

void map_tab::on_object_direction_combo_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->object_direction = index;
}

void map_tab::on_editTile_button_clicked()
{
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);

    //ui->editModeNormal_button->setEnabled(true);
    ui->editModeLock_button->setEnabled(true);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(1);
    Mediator::get_instance()->editMode = EDITMODE_NORMAL;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_editObject_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editObject_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(5);
    Mediator::get_instance()->editMode = EDITMODE_OBJECT;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
    update_edit_area();
}

void map_tab::on_editLink_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(4);
    Mediator::get_instance()->editMode = EDITMODE_LINK;
    Mediator::get_instance()->editTool = EDITMODE_LINK;
    update_edit_area();
}

void map_tab::on_editNpc_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editNpc_button->setChecked(true);
    ui->addAnimTile_toolButton->setChecked(false);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(3);
    Mediator::get_instance()->editMode = EDITMODE_NPC;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_editSetSubBoss_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(3);
    Mediator::get_instance()->editMode = EDITMODE_SET_SUBBOSS;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_editSetBoss_button_clicked()
{
    ui->editTile_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editNpc_button->setChecked(false);
    ui->editSetBoss_button->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);

    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(3);
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
        set_current_box(1);
    } else if (ui->editNpc_button->isChecked()) {
        set_current_box(3);
    } else if (ui->editObject_button->isChecked()) {
        set_current_box(5);
    } else if (ui->editLink_button->isChecked()) {
        set_current_box(4);
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

    set_current_box(2);
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



void map_tab::on_mapGFX_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].gfx = index;
}

void map_tab::on_autoScrollBG1_mode_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[0].auto_scroll = index;
}


void map_tab::on_addAnimTile_toolButton_clicked()
{
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(true);
    ui->editModeNormal_button->setEnabled(true);

    //ui->editModeNormal_button->setEnabled(true);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(true);

    set_current_box(6);
    Mediator::get_instance()->editMode = EDITMODE_ANIM_TILE;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;

}

void map_tab::on_toolButton_clicked()
{
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);

    ui->editModeNormal_button->setEnabled(false);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(1);
    Mediator::get_instance()->editMode = EDITMODE_SELECT;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_paste_toolButton_clicked()
{
    ui->editNpc_button->setChecked(false);
    ui->editSetSubBoss_button->setChecked(false);
    ui->editSetBoss_button->setChecked(false);
    ui->editObject_button->setChecked(false);
    ui->editLink_button->setChecked(false);
    ui->editTile_button->setChecked(false);
    ui->addAnimTile_toolButton->setChecked(false);

    ui->editModeNormal_button->setEnabled(false);
    ui->editModeLock_button->setEnabled(false);
    ui->editModeErase_button->setEnabled(false);

    set_current_box(1);
    Mediator::get_instance()->editMode = EDITMODE_PASTE;
    Mediator::get_instance()->editTool = EDITMODE_NORMAL;
}

void map_tab::on_mapAutoScroll_checkBox_clicked(bool checked)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->stage_data.stages[Mediator::get_instance()->currentStage].autoscroll[Mediator::get_instance()->currentMap] = checked;
}


void map_tab::on_fb_image_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) { return; }
    if (arg1.toStdString() == std::string("None")) {
        Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].filename[0] = '\0';
    } else {
        sprintf(Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].filename, "%s", arg1.toStdString().c_str());
    }
    update_edit_area();
}

void map_tab::on_fg_speed_doubleSpinBox_valueChanged(double arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].speed = arg1*10;
    std::cout << ">> on_FG_speed_valueChanged - setvalue: " << arg1 << ", FG.speed: " << (int)Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].speed << std::endl;
    update_edit_area();
}

void map_tab::on_fg_position_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].adjust_y = arg1;
    update_edit_area();
}

void map_tab::on_fg_show_checkBox_toggled(bool checked)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->show_fg_layer = checked;
    update_edit_area();
}

void map_tab::on_fg_opacity_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].gfx = arg1;
    update_edit_area();
}

void map_tab::on_mapGFXMode_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->maps_data_v2[Mediator::get_instance()->currentStage][Mediator::get_instance()->currentMap].backgrounds[1].auto_scroll = index;
}

void map_tab::on_difficulty_comboBox_currentIndexChanged(int index)
{
    Mediator::get_instance()->currentDifficulty = index;
    update_edit_area();
}


void map_tab::on_difficultyMode_pushButton_clicked()
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->currentDifficultyMode++;
    if (Mediator::get_instance()->currentDifficultyMode >= DIFFICULTY_MODE_COUNT) {
        Mediator::get_instance()->currentDifficultyMode = DIFFICULTY_MODE_GREATER;
    }
    if (Mediator::get_instance()->currentDifficultyMode == DIFFICULTY_MODE_GREATER) {
        ui->difficultyMode_pushButton->setText(">=");
    } else if (Mediator::get_instance()->currentDifficultyMode == DIFFICULTY_MODE_EQUAL) {
        ui->difficultyMode_pushButton->setText("=");
//    } else if (Mediator::get_instance()->currentDifficultyMode == DIFFICULTY_MODE_LESS) {
//        ui->difficultyMode_pushButton->setText("<=");
    }
}

void map_tab::on_addMap_pushButton_clicked()
{
    Mediator::get_instance()->file_v5_map_header_list.push_back(file_v5_map_header());
    _data_loading = true;
    fill_map_selector();
    fill_data();
    _data_loading = false;
}

void map_tab::fill_map_selector()
{
    ui->mapSelector_comboBox->clear();
    for (unsigned int i=0; i<Mediator::get_instance()->file_v5_map_header_list.size(); i++) {
        QString itemName = QString("[") + QString::number(i) + QString("] - ") + QString(Mediator::get_instance()->file_v5_map_header_list.at(i).map_name);
        ui->mapSelector_comboBox->addItem(itemName);
    }
    ui->mapSelector_comboBox->setCurrentIndex(Mediator::get_instance()->file_v5_selected_map);
}

void map_tab::fill_map_v5_data()
{
    if (Mediator::get_instance()->file_v5_map_header_list.size() == 0) {
        return;
    }
    ui->v5_mapName_lineEdit->setText(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).map_name);
    ui->mapSizeW_spinBox->setValue(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w);
    ui->mapSizeH_spinBox->setValue(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h);
}

void map_tab::on_mapSelector_comboBox_currentIndexChanged(int index)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->file_v5_selected_map = index;
    _data_loading = true;
    fill_map_v5_data();
    _data_loading = false;
}


void map_tab::on_v5_mapName_lineEdit_textChanged(const QString &arg1)
{
    if (_data_loading == true) { return; }
    sprintf(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).map_name, "%s", arg1.toStdString().c_str());
    _data_loading = true;
    fill_map_selector();
    _data_loading = false;
}

void map_tab::on_mapSizeW_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w = arg1;
}

void map_tab::on_mapSizeH_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h = arg1;

}

void map_tab::on_generateMapTiles_pushButton_clicked()
{
    if (ui->mapSizeW_spinBox->value() == 0 || ui->mapSizeH_spinBox->value() == 0) {
        QMessageBox::warning(this, "Error", "Invalid map size (zero).");
        return;
    }
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Rockbot Editor :: Map Editor", tr("Generating tiles will erase any existing map data. Continue?\n"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn == QMessageBox::No) {
        return;
    } else {
        generateMapTiles();
    }
}

void map_tab::generateMapTiles()
{
    if (Mediator::get_instance()->file_v5_map_tile_map.find(Mediator::get_instance()->file_v5_selected_map) == Mediator::get_instance()->file_v5_map_tile_map.end()) {
        std::cout << "map_tab::generateMapTiles - entry not found, adding it..." << std::endl;
        Mediator::get_instance()->file_v5_map_tile_map.insert(std::pair<int, std::vector<file_v5_map_tile>>(Mediator::get_instance()->file_v5_selected_map, std::vector<file_v5_map_tile>()));
    }
    std::cout << "map_tab::generateMapTiles -generating [" << Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w << "][" << Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h << "] tiles in key [" << Mediator::get_instance()->file_v5_selected_map << "]" << std::endl;
    Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).clear();
    for (int i=0; i< Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_w; i++) {
        for (int j=0; j< Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tiles_h; j++) {
            Mediator::get_instance()->file_v5_map_tile_map.at(Mediator::get_instance()->file_v5_selected_map).push_back(file_v5_map_tile());
        }
    }
    ui->editArea->repaint();
}

void map_tab::on_v5_tileset_comboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading == true) { return; }
    if (arg1.length() == 0) { // reset to default
        Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tileset_filename[0] = '\0';
        Mediator::get_instance()->setPallete(std::string("default.png"));
    } else {
        sprintf(Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map).tileset_filename, "%s", arg1.toStdString().c_str());
        Mediator::get_instance()->setPallete(arg1.toStdString());
    }
    ui->pallete->repaint();
    update_edit_area();
}

void map_tab::on_layerSelector_comboBox_currentIndexChanged(int index)
{
    Mediator::get_instance()->file_v5_selected_layer = index;
    _data_loading = true;
    set_layer_data();
    _data_loading = false;
}

void map_tab::set_layer_data()
{
    if (Mediator::get_instance()->file_v5_map_header_list.size() == 0) {
        return;
    }
    if (Mediator::get_instance()->file_v5_map_header_list.size() < Mediator::get_instance()->file_v5_selected_layer) {
        return;
    }
    file_v5_map_header map_header = Mediator::get_instance()->file_v5_map_header_list.at(Mediator::get_instance()->file_v5_selected_map);
    ui->bg1_filename->setCurrentIndex(ui->bg1_filename->findText(map_header.backgrounds[Mediator::get_instance()->file_v5_selected_layer].filename));
    ui->bg1_y_pos->setValue(map_header.backgrounds[Mediator::get_instance()->file_v5_selected_layer].adjust_y);
    float bg1_speed = (float)map_header.backgrounds[Mediator::get_instance()->file_v5_selected_layer].speed/10;
    ui->bg1_speed->setValue(bg1_speed);

    ui->autoScrollBG1_mode->setCurrentIndex(map_header.backgrounds[Mediator::get_instance()->file_v5_selected_layer].auto_scroll);

}
