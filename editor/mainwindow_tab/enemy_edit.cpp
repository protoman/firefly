#include "enemy_edit.h"
#include "ui_npc_edit.h"

#include "common.h"

#include <QMessageBox>

enemy_edit::enemy_edit(QWidget *parent) : _data_loading(false), QWidget(parent), _npcedit_tab_selectednpc(0), ui(new Ui::npc_edit)
{
	_data_loading = true;
    _ignore_spritelist_row_changed = false;
	ui->setupUi(this);
	QObject::connect(ui->npc_edit_tab_previewarea, SIGNAL(clickedIn()), this, SLOT(set_npc_frame()));
	_npcedit_tab_selected_weakness_weapon = 0;

    fill_data();
	on_npc_edit_tab_selectnpccombo_currentIndexChanged(0);
	_data_loading = false;
	reload_frame_list(ui->frame_list_selector->currentIndex());
}

enemy_edit::~enemy_edit()
{
    delete ui;
}

void enemy_edit::reload()
{
    _data_loading = true;
    fill_data();
    on_npc_edit_tab_selectnpccombo_currentIndexChanged(0);
    _data_loading = false;
    reload_frame_list(ui->frame_list_selector->currentIndex());
}



void enemy_edit::fill_data()
{
    if (SharedData::get_instance()->FILEPATH.length() == 0) {
        return;
    }
	_data_loading = true;
    common::fill_files_combo("images/sprites/enemies", ui->npc_edit_tab_graphiccombo);
    common::fill_files_combo("images/sprites/enemies/backgrounds", ui->backgroundFileComboBox);
    common::fill_object_combo_with_none(ui->npc_requestedItem_comboBox);
    common::fill_object_combo_with_none(ui->npc_givenItem_comboBox);

    common::fill_enemies_combo(ui->npc_edit_tab_selectnpccombo);

    if (GameData::get_instance()->enemy_list.size() > 0) {
        // GRAPHIC FILE //
        ui->npc_edit_tab_graphiccombo->setCurrentIndex(ui->npc_edit_tab_graphiccombo->findText(QString(GameData::get_instance()->enemy_list.at(0).graphic_filename)));
        ui->npc_edit_tab_previewarea->set_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/")+std::string(GameData::get_instance()->enemy_list.at(0).graphic_filename));
        // BACKGROUND //
        //std::cout << ">>>>> BACKGROUND[" << GameData::get_instance()->enemy_list.at(0).bg_graphic_filename << "]" << std::endl;
        ui->backgroundFileComboBox->setCurrentIndex(ui->backgroundFileComboBox->findText(QString(GameData::get_instance()->enemy_list.at(0).bg_graphic_filename)));
        ui->npc_edit_tab_previewarea->set_bg_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/backgrounds/")+std::string(GameData::get_instance()->enemy_list.at(0).bg_graphic_filename));
        ui->sprite_pos_x->setValue(GameData::get_instance()->enemy_list.at(0).sprites_pos_bg.x);
        ui->sprite_pos_y->setValue(GameData::get_instance()->enemy_list.at(0).sprites_pos_bg.y);

        ui->npc_edit_tab_previewarea->set_sprite_pos(GameData::get_instance()->enemy_list.at(0).sprites_pos_bg);
        ui->npc_edit_tab_previewarea->repaint();

        _data_loading = true;

        if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).fly_flag == 0) {
            ui->checkBox->setChecked(false);
        } else {
            ui->checkBox->setChecked(true);
        }

        ui->hitarea_x_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.x);
        ui->hitarea_y_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.y);

        if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w != 0) {
            ui->hitarea_w_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w);
        } else {
            ui->hitarea_w_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.width);
        }
        if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h != 0) {
            ui->hitarea_h_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h);
        } else {
            ui->hitarea_h_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.height);
        }

        ui->respawn_time_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).respawn_delay);

        ui->npc_edit_tab_graphicheight->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.height);
        ui->npc_edit_tab_graphicwidth->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.width);
        Mediator::get_instance()->npcGraphicSize_h = GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.height;
        Mediator::get_instance()->npcGraphicSize_w = GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.width;

        ui->projectileOriginX_spinBox->setValue(GameData::get_instance()->enemy_list.at(0).attack_arm_pos.x);
        ui->projectileOriginY_spinBox->setValue(GameData::get_instance()->enemy_list.at(0).attack_arm_pos.y);

        ui->isNPC_checkBox->setChecked(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).is_npc);
        ui->npcDialogId_comboBox->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_dialog_id);

        ui->npc_requestedItem_comboBox->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_requested_item_id+1);
        ui->npc_givenItem_comboBox->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_given_item_id+1);
    }

    ui->npcDialogId_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());
    ui->npc_requestedItem_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());
    ui->npc_givenItem_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());


}

void enemy_edit::add_frame_one()
{
    file_enemy_v3_1_2* new_npc_ref = &GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n);
    if (new_npc_ref->sprites[ANIM_TYPE_STAND][0].used == false) {
        new_npc_ref->sprites[ANIM_TYPE_STAND][0].used = true;
        new_npc_ref->sprites[ANIM_TYPE_STAND][0].duration = 100;
        new_npc_ref->sprites[ANIM_TYPE_STAND][0].sprite_graphic_pos_x = 0;
        ui->frame_list_selector->setCurrentIndex(0);
        reload_frame_list(ANIM_TYPE_STAND);
        ui->frameList_listWidget->setCurrentRow(0);
    }
}


void enemy_edit::on_npc_edit_tab_selectnpccombo_currentIndexChanged(int index)
{
    if (index == -1) {
        return;
    }
	_data_loading = true;
    Mediator::get_instance()->current_npc_n = index;

	_npcedit_tab_selectednpc = index;

    if (GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }

    ui->npc_edit_tab_canshoot->setChecked(GameData::get_instance()->enemy_list.at(index).is_ghost);
    // GRAPHIC FILE //
    std::string image_filename(GameData::get_instance()->enemy_list.at(index).graphic_filename);
    if (image_filename.length() > 0) {
        ui->npc_edit_tab_graphiccombo->setCurrentIndex(ui->npc_edit_tab_graphiccombo->findText(QString(image_filename.c_str())));
    }

    // BACKGROUND FILE //
    std::string bg_graphic_filename(GameData::get_instance()->enemy_list.at(index).bg_graphic_filename);
    if (bg_graphic_filename.length() > 0) {
        ui->backgroundFileComboBox->setCurrentIndex(ui->backgroundFileComboBox->findText(QString(bg_graphic_filename.c_str())));
    }
    ui->sprite_pos_x->setValue(GameData::get_instance()->enemy_list.at(index).sprites_pos_bg.x);
    ui->sprite_pos_y->setValue(GameData::get_instance()->enemy_list.at(index).sprites_pos_bg.y);


    ui->npc_edit_tab_graphicheight->setValue(GameData::get_instance()->enemy_list.at(index).frame_size.height);
    ui->npc_edit_tab_graphicwidth->setValue(GameData::get_instance()->enemy_list.at(index).frame_size.width);

    ui->npc_edit_tab_previewarea->set_grid_w(GameData::get_instance()->enemy_list.at(index).frame_size.width);
    ui->npc_edit_tab_previewarea->set_grid_h(GameData::get_instance()->enemy_list.at(index).frame_size.height);

    Mediator::get_instance()->npcGraphicSize_h = GameData::get_instance()->enemy_list.at(index).frame_size.height;
    Mediator::get_instance()->npcGraphicSize_w = GameData::get_instance()->enemy_list.at(index).frame_size.width;
    ui->npc_edit_tab_movespeed->setValue(GameData::get_instance()->enemy_list.at(index).speed);
    ui->npc_edit_tab_NpcHP->setValue(GameData::get_instance()->enemy_list.at(index).hp);
    ui->npc_edit_tab_NpcName->setText(GameData::get_instance()->enemy_list.at(index).name);
    ui->npc_edit_tab_range->setValue(GameData::get_instance()->enemy_list.at(index).walk_range);
    ui->npc_edit_tab_shieldtype->setCurrentIndex(GameData::get_instance()->enemy_list.at(index).shield_type);
	ui->npc_edit_tab_weakness_list->setCurrentIndex(0);

    ui->npc_edit_tab_previewarea->set_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/")+std::string(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).graphic_filename));

    ui->npc_edit_tab_previewarea->set_bg_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/backgrounds/")+std::string(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).bg_graphic_filename));
    ui->npc_edit_tab_previewarea->set_sprite_pos(GameData::get_instance()->enemy_list.at(index).sprites_pos_bg);
    ui->npc_edit_tab_previewarea->repaint();
	reload_frame_list(ui->frame_list_selector->currentIndex());
	ui->sprites_preview_widget->repaint();

    ui->isBoss_checkBox->setChecked(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_boss);
    ui->isSubBoss_checkbox->setChecked(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_sub_boss);

    if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).fly_flag == 0) {
        ui->checkBox->setChecked(false);
    } else {
        ui->checkBox->setChecked(true);
    }

    ui->hitarea_x_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.x);
    ui->hitarea_y_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.y);

    if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w != 0) {
        ui->hitarea_w_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w);
    } else {
        ui->hitarea_w_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.width);
    }
    if (GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h != 0) {
        ui->hitarea_h_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h);
    } else {
        ui->hitarea_h_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).frame_size.height);
    }

    ui->respawn_time_spinBox->setValue(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).respawn_delay);

    ui->projectileOriginX_spinBox->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_arm_pos.x);
    ui->projectileOriginY_spinBox->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_arm_pos.y);

    ui->isNPC_checkBox->setChecked(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).is_npc);

    ui->npc_requestedItem_comboBox->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_requested_item_id+1);
    ui->npc_givenItem_comboBox->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_given_item_id+1);

    ui->npcDialogId_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());
    ui->npc_requestedItem_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());
    ui->npc_givenItem_comboBox->setEnabled(ui->isNPC_checkBox->isChecked());

	_data_loading = false;
}

void enemy_edit::on_npc_edit_tab_graphiccombo_currentIndexChanged(const QString &arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    sprintf(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).graphic_filename, "%s", arg1.toStdString().c_str());
    ui->npc_edit_tab_previewarea->set_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/")+arg1.toStdString());
    add_frame_one();
    ui->npc_edit_tab_previewarea->repaint();
}

void enemy_edit::on_npc_edit_tab_graphicwidth_valueChanged(int arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    ui->npc_edit_tab_previewarea->set_grid_w(arg1);
    // if hitarea is zero or old value, update to new one
    _data_loading = true;
    if (ui->hitarea_w_spinBox->value() == 0 || ui->hitarea_w_spinBox->value() == GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width) {
        ui->hitarea_w_spinBox->setValue(arg1);
        if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width == GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w) {
            GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w = arg1;
        }
        ui->npc_edit_tab_previewarea->update();
        reload_frame_list(ui->frame_list_selector->currentIndex());
    }
    _data_loading = false;
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width = arg1;
}

void enemy_edit::on_npc_edit_tab_graphicheight_valueChanged(int arg1)
{
    if (_data_loading) {
        return;
    }
    if (GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    ui->npc_edit_tab_previewarea->set_grid_h(arg1);
    // if hitarea is zero or old value, update to new one
    _data_loading = true;
    if (ui->hitarea_h_spinBox->value() == 0 || ui->hitarea_h_spinBox->value() == GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height) {
        ui->hitarea_h_spinBox->setValue(arg1);

        if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height == GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h) {
            std::cout << "### SET collistion H" << std::endl;
            GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h = arg1;
        }

        ui->npc_edit_tab_previewarea->update();
        reload_frame_list(ui->frame_list_selector->currentIndex());
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height = arg1;
    _data_loading = false;
}

void enemy_edit::on_npc_edit_tab_NpcName_textChanged(const QString &arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }

    // check that it is not the name of a player
    if (arg1.startsWith("PLAYER_")) {
        QMessageBox msgBox;
        msgBox.setText("ERROR");
        msgBox.setInformativeText("Invalid name conflicts with player names.");
        int ret = msgBox.exec();
        char reset_name[50];
        sprintf(reset_name, "NPC[%d]", _npcedit_tab_selectednpc);
        ui->npc_edit_tab_NpcName->setText(reset_name);
        return;
    }
    // check that it is not the name of another enemy
    bool name_error = false;
    for (int i=0; i<GameData::get_instance()->enemy_list.size(); i++) {
        if (arg1.toStdString() == GameData::get_instance()->enemy_list.at(i).name) {
            name_error = true;
            break;
        }
    }
    if (name_error == true) {
        ui->npc_edit_tab_NpcName->setStyleSheet("color: #FF0000");
    } else {
        ui->npc_edit_tab_NpcName->setStyleSheet("color: #000000");
    }


    sprintf(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).name, "%s", arg1.toStdString().c_str());
    QString temp_str = QString("[");
    if (_npcedit_tab_selectednpc < 10) {
        temp_str += QString("0");
    }
    temp_str += QString::number(_npcedit_tab_selectednpc) + QString("] - ") + QString(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).name);
    ui->npc_edit_tab_selectnpccombo->setItemText(_npcedit_tab_selectednpc, temp_str);
}

void enemy_edit::on_npc_edit_tab_NpcHP_valueChanged(int arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).hp = arg1;
}

void enemy_edit::on_npc_edit_tab_canshoot_toggled(bool checked)
{
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).is_ghost = checked;
}

void enemy_edit::on_npc_edit_tab_shieldtype_currentIndexChanged(int index)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).shield_type = index;
}

void enemy_edit::on_npc_edit_tab_movespeed_valueChanged(int arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).speed = arg1;
}

void enemy_edit::on_npc_edit_tab_range_valueChanged(int arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).walk_range = arg1;
}

void enemy_edit::on_npc_edit_tab_frametype_currentIndexChanged(int index)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    // @204
    //GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frames[Mediator::get_instance()->getPalleteX()].state = index-1;
}


void enemy_edit::set_npc_frame() {
	printf(">> DialogNPCEdit::setNPCFrame - CALLED\n");
    // @204
    //ui->npc_edit_tab_frameduration->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frames[Mediator::get_instance()->getPalleteX()].duration);
    //ui->npc_edit_tab_frametype->setCurrentIndex(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frames[Mediator::get_instance()->getPalleteX()].state+1);
}


void enemy_edit::on_bg_graphic_combo_currentIndexChanged(const QString &arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    if (arg1.length() == 0) {
        //std::cout << "*************** on_bg_graphic_combo_currentIndexChanged - ZERO" << std::endl;
        sprintf(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).bg_graphic_filename, "%s", "");
        ui->npc_edit_tab_previewarea->set_bg_graphicfile("");
    } else {
        //std::cout << "*************** on_bg_graphic_combo_currentIndexChanged - SET to '" << arg1.toStdString() << "'" << std::endl;
        sprintf(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).bg_graphic_filename, "%s", arg1.toStdString().c_str());
        ui->npc_edit_tab_previewarea->set_bg_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/backgrounds/")+std::string(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).bg_graphic_filename));
    }
}


void enemy_edit::on_frame_list_selector_currentIndexChanged(int index)
{
	if (_data_loading == true) {
		return;
	}
    Mediator::get_instance()->current_sprite_type = index;


    _data_loading = true;
    if (ui->frame_list_selector->currentText() == "ATTACK") {
        ui->isAttackFrame_checkBox->setEnabled(true);
    } else {
        ui->isAttackFrame_checkBox->setEnabled(false);
    }
    _data_loading = false;
    reload_frame_list(index);
}

void enemy_edit::reload_frame_list(int index)
{
    int current_row = -1;
    if (ui->frameList_listWidget->selectedItems().size() > 0) {
        current_row = ui->frameList_listWidget->currentRow();
    }
	// clean all items from combo
	ui->frameList_listWidget->clear();


	// insert all sprites for the given type into the combo
    //std::cout << ">> Adding frames for NPC[" << GameData::get_instance()->game_data.game_npcs[i].name << "] - type: " << index << std::endl;
    std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/" + GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).graphic_filename;
    // @204
    for (int j=0; j<ANIM_FRAMES_COUNT; j++) {
        if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[index][j].used == true) {
            int calc_pos_x = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[index][j].sprite_graphic_pos_x * GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width;
            //std::cout << "NPC[" << GameData::get_instance()->game_data.game_npcs[i].name << "] - frame.pos_x: " << GameData::get_instance()->game_data.game_npcs[i].sprites[index][j].sprite_graphic_pos_x << ", cal_pos_x: " << calc_pos_x << std::endl;
            QListWidgetItem* item = new QListWidgetItem;
            QString temp_str(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).name + QString(" (") + QString::number(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[index][j].duration) + QString(")"));
            item->setText(temp_str);
            //GameData::get_instance()->game_data.game_npcs[i].name + " (" + QString::number(GameData::get_instance()->game_data.game_npcs[i].sprites[index][j].duration).c_ + ")");
            QPixmap image(filename.c_str());
            if (image.isNull() == false && image.width() > 0) {
                image = image.copy(calc_pos_x, 0, GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width, GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height);
                image = image.scaled(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width*2, GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height*2);
                item->setIcon(image);
            }
            int h = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height*2+4;
            if (h < 24) {
                h = 24;
            }
            item->setSizeHint(QSize(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width*2, h));
            ui->frameList_listWidget->addItem(item);
        }
    }


    if (current_row != -1) {
        _ignore_spritelist_row_changed = true;
        if (ui->frameList_listWidget->count() >= current_row) {
            ui->frameList_listWidget->setCurrentRow(current_row);
        } else if (ui->frameList_listWidget->count() > 0) {
            ui->frameList_listWidget->setCurrentRow(0);
        }
    } else {
        ui->frameList_listWidget->setCurrentRow(0);
    }
    ui->frameList_listWidget->update();
}

void enemy_edit::on_pushButton_clicked()
{
    if (ui->frameList_listWidget->selectedItems().size() == 0) {
        return;
    }
    //copy next sprite to current position
    for (int j=ui->frameList_listWidget->currentRow(); j<ANIM_FRAMES_COUNT-1; j++) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][j] = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][j+1];
    }
    // clear last sprite
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ANIM_FRAMES_COUNT-1].used = false;
    reload_frame_list(ui->frame_list_selector->currentIndex());
}

void enemy_edit::on_frameUp_clicked()
{
    if (ui->frameList_listWidget->currentRow() == 0) {
        return;
    }
    if (ui->frameList_listWidget->selectedItems().size() == 0) {
        return;
    }
    st_sprite_data temp = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()-1];
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()-1] = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()];
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()] = temp;
    reload_frame_list(ui->frame_list_selector->currentIndex());
}

void enemy_edit::on_frameDown_clicked()
{
    if (ui->frameList_listWidget->selectedItems().size() == 0) {
        return;
    }
    if (ui->frameList_listWidget->currentRow() >= ANIM_FRAMES_COUNT-1) {
        return;
    }
    if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()+1].used == false) {
        return;
    }
    st_sprite_data temp = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()+1];
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()+1] = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()];
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][ui->frameList_listWidget->currentRow()] = temp;
    reload_frame_list(ui->frame_list_selector->currentIndex());
}

void enemy_edit::on_frameList_listWidget_currentRowChanged(int currentRow)
{
    ui->sprite_duration_spinBox->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][currentRow].duration);
    ui->sprite_collision_x->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][currentRow].collision_rect.x);
    ui->sprite_collision_y->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][currentRow].collision_rect.y);
    ui->sprite_collision_w->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][currentRow].collision_rect.w);
    ui->sprite_collision_h->setValue(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[ui->frame_list_selector->currentIndex()][currentRow].collision_rect.h);

    if (ui->frame_list_selector->currentText() == "ATTACK") {
        std::cout << "attack_frame: " << (int)GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_frame << ", currentRow: " << currentRow << std::endl;
        _data_loading = true;
        if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_frame == currentRow) {
            ui->isAttackFrame_checkBox->setChecked(true);
        } else {
            ui->isAttackFrame_checkBox->setChecked(false);
        }
        _data_loading = false;
    }
    Mediator::get_instance()->current_sprite_selection = currentRow;
}

void enemy_edit::on_sprite_duration_spinBox_valueChanged(int arg1)
{
    if (ui->frameList_listWidget->selectedItems().size() > 0) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].duration = arg1;
        if (_ignore_spritelist_row_changed == false) {
            reload_frame_list(ui->frame_list_selector->currentIndex());
        } else {
            _ignore_spritelist_row_changed = false;
        }
    }
}

void enemy_edit::on_sprite_collision_x_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
	if (ui->frameList_listWidget->selectedItems().size() > 0) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].collision_rect.x = arg1;
	}
	ui->sprites_preview_widget->repaint();
}

void enemy_edit::on_sprite_collision_y_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
	if (ui->frameList_listWidget->selectedItems().size() > 0) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].collision_rect.y = arg1;
	}
	ui->sprites_preview_widget->repaint();
}

void enemy_edit::on_sprite_collision_w_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
	if (ui->frameList_listWidget->selectedItems().size() > 0) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].collision_rect.w = arg1;
	}
	ui->sprites_preview_widget->repaint();

}

void enemy_edit::on_sprite_collision_h_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
	if (ui->frameList_listWidget->selectedItems().size() > 0) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].collision_rect.h = arg1;
	}
	ui->sprites_preview_widget->repaint();

}


void enemy_edit::on_isBoss_checkBox_toggled(bool checked)
{
    if (_data_loading == true) {
        return;
    }
    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_boss = checked;
    // a sub-boss can't be a boss and vice-versa
    if (checked == true && GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_sub_boss == true) {
        GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_sub_boss = false;
        _data_loading = true;
        ui->isSubBoss_checkbox->setChecked(false);
        _data_loading = false;
    }
}

void enemy_edit::on_isSubBoss_checkbox_toggled(bool checked)
{
    if (_data_loading == true) { return; }
    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_sub_boss = checked;
    // a sub-boss can't be a boss and vice-versa
    if (checked == true && GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_boss == true) {
        GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).is_boss = false;
        _data_loading = true;
        ui->isBoss_checkBox->setChecked(false);
        _data_loading = false;
    }
}

void enemy_edit::on_sprite_pos_x_valueChanged(int arg1)
{
    if (_data_loading == true) {
        return;
    }
    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg.x = arg1;
    ui->npc_edit_tab_previewarea->set_sprite_pos(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg);
    ui->npc_edit_tab_previewarea->update();
    reload_frame_list(ui->frame_list_selector->currentIndex());
}

void enemy_edit::on_sprite_pos_y_valueChanged(int arg1)
{
    if (_data_loading == true) {
        return;
    }
    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg.y = arg1;
    ui->npc_edit_tab_previewarea->set_sprite_pos(GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg);
    ui->npc_edit_tab_previewarea->update();
    reload_frame_list(ui->frame_list_selector->currentIndex());
}

void enemy_edit::on_checkBox_toggled(bool checked)
{
    if (_data_loading == true) {
        return;
    }
    if (checked == true) {
        GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).fly_flag = 1;
    } else {
        GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).fly_flag = 0;
    }
}

void enemy_edit::on_hitarea_x_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }

    std::cout << "hi_area::change(x): " << arg1 << std::endl;

    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.x = arg1;
    ui->npc_edit_tab_previewarea->update();
}

void enemy_edit::on_hitarea_y_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }

    std::cout << "hi_area::change(y): " << arg1 << std::endl;

    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.y = arg1;
    ui->npc_edit_tab_previewarea->update();
}

void enemy_edit::on_hitarea_w_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }

    std::cout << "hi_area::change(w): " << arg1 << std::endl;

    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.w = arg1;
    ui->npc_edit_tab_previewarea->update();
}

void enemy_edit::on_hitarea_h_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }

    std::cout << "hi_area::change(h): " << arg1 << std::endl;

    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).vulnerable_area.h = arg1;
    ui->npc_edit_tab_previewarea->update();
}



void enemy_edit::on_respawn_time_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).respawn_delay = arg1;
}

void enemy_edit::on_AddFrame_Button_clicked()
{
    //frame_list_selector
    // go to the first free slot
    // @204
    std::cout << "## FOUND NPC!!!!" << std::endl;
    for (int j=0; j<ANIM_FRAMES_COUNT; j++) {
        int frame_type = ui->frame_list_selector->currentIndex();
        if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].used == false) {
            std::cout << "ADD-SPRITE-FRAME: FOUND EMPTY SLOT AT [" << j << "]" << std::endl;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].used = true;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].duration = 100;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].sprite_graphic_pos_x = Mediator::get_instance()->getPalleteX();
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].collision_rect.w = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.width;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].collision_rect.h = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).frame_size.height;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].collision_rect.x = GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg.x;
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[frame_type][j].collision_rect.y = GameData::get_instance()->enemy_list.at(Mediator::get_instance()->current_npc_n).sprites_pos_bg.y;
            reload_frame_list(ui->frame_list_selector->currentIndex());
            return;
        }
    }
}

void enemy_edit::on_addEnemy_pushButton_clicked()
{
    _data_loading = true;
    GameData::get_instance()->enemy_list.push_back(file_enemy_v3_1_2());
    // add equivalent AI for enemy
    GameData::get_instance()->ai_list.push_back(file_artificial_inteligence());
    ui->npc_edit_tab_selectnpccombo->addItem(QString("[") + QString::number(GameData::get_instance()->enemy_list.size()-1) + QString("] Enemy Name"));
    ui->npc_edit_tab_graphiccombo->setCurrentIndex(-1);
    _data_loading = false;
    ui->npc_edit_tab_selectnpccombo->setCurrentIndex(GameData::get_instance()->enemy_list.size()-1);
}

void enemy_edit::on_isAttackFrame_checkBox_toggled(bool checked)
{
    if (_data_loading == true) { return; }
    if (ui->frame_list_selector->currentText() == "ATTACK") {
        if (checked == true) {
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_frame = ui->frameList_listWidget->currentRow();
        } else {
            GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_frame = 0;
        }
    }
}

void enemy_edit::on_projectileOriginX_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_arm_pos.x = arg1;
}

void enemy_edit::on_projectileOriginY_spinBox_valueChanged(int arg1)
{
    if (_data_loading == true) { return; }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).attack_arm_pos.y = arg1;
}

void enemy_edit::on_pushButton_2_clicked()
{
    if (_data_loading == true) { return; }
    if (ui->frameList_listWidget->selectedItems().size() > 0) {
        for (int i=0; i<ANIM_FRAMES_COUNT; i++) {
            if (i != ui->frameList_listWidget->currentRow()) {
                GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][i].collision_rect = GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).sprites[Mediator::get_instance()->current_sprite_type][ui->frameList_listWidget->currentRow()].collision_rect;
            }
        }
        ui->sprites_preview_widget->repaint();
    }
}

void enemy_edit::on_backgroundFileComboBox_currentIndexChanged(const QString &arg1)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    sprintf(GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).bg_graphic_filename, "%s", arg1.toStdString().c_str());
    ui->npc_edit_tab_previewarea->set_bg_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/enemies/backgrounds/")+arg1.toStdString());
    add_frame_one();
    ui->npc_edit_tab_previewarea->repaint();
}

void enemy_edit::on_isNPC_checkBox_toggled(bool checked)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    if (GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).is_npc == true && checked == false) {
        GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_dialog_id = -1;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).is_npc = checked;
    ui->npcDialogId_comboBox->setEnabled(checked);
    ui->npc_requestedItem_comboBox->setEnabled(checked);
    ui->npc_givenItem_comboBox->setEnabled(checked);
    // disable NPC fields when checkbox is set to disabled
}

void enemy_edit::on_npc_requestedItem_comboBox_currentIndexChanged(int index)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_requested_item_id = index-1;
}

void enemy_edit::on_npc_givenItem_comboBox_currentIndexChanged(int index)
{
    if (_data_loading || GameData::get_instance()->enemy_list.size() == 0) {
        return;
    }
    GameData::get_instance()->enemy_list.at(_npcedit_tab_selectednpc).npc_given_item_id = index-1;
}
