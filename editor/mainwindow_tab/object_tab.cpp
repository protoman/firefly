#include "object_tab.h"
#include "ui_object_tab.h"
#include "common.h"

object_tab::object_tab(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::object_tab),
	_current_object(0),
	_data_loaded(false)
{
    ui->setupUi(this);

}

object_tab::~object_tab()
{
    delete ui;
}

void object_tab::reload()
{
    _current_object = 0;
    fill_data();
}

void object_tab::fill_data()
{
    _data_loaded = false;
    common::fill_files_combo("/images/sprites/objects", ui->graphicfile_combo);
    if (SharedData::get_instance()->v6_object_list.size() > 0) {
        ui->object_preview_area->set_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/objects/")+std::string(SharedData::get_instance()->v6_object_list.at(0).graphic_filename));
        ui->object_preview_area->repaint();
    }

    common::fill_object_combo(ui->objectlist_combo);
    fill_ability_data();

    _data_loaded = true;
    on_objectlist_combo_currentIndexChanged(0);
}

void object_tab::fill_ability_data()
{
    bool original_data_loaded = _data_loaded;
    _data_loaded = false;
    int obj_type =SharedData::get_instance()->v6_object_list.at(ui->objectlist_combo->currentIndex()).type;
    if (SharedData::get_instance()->v6_object_list.size() > 0 && (obj_type == OBJ_ABILITY_ITEM || obj_type == OBJ_TREASURE_CHEST)) {
        ui->givenAbility_label->setText("Given Item");
        common::fill_object_combo(ui->givenAbilityComboBox);
        ui->givenAbilityComboBox->setEnabled(true);
        ui->key_comboBox->setEnabled(true);
        ui->givenAbilityComboBox->setCurrentIndex(SharedData::get_instance()->v6_object_list.at(ui->objectlist_combo->currentIndex()).given_ability);
        ui->key_comboBox->setCurrentIndex(SharedData::get_instance()->v6_object_list.at(ui->objectlist_combo->currentIndex()).key_id);
    } else {
        ui->givenAbility_label->setText("Given Ability");
        common::fill_abilities_combo(ui->givenAbilityComboBox);
        SharedData::get_instance()->v6_object_list.at(ui->objectlist_combo->currentIndex()).given_ability = -1;
        ui->givenAbilityComboBox->setEnabled(false);
        ui->key_comboBox->setEnabled(false);
    }

    _data_loaded = original_data_loaded;
}

void object_tab::on_graphicfile_combo_currentIndexChanged(const QString &arg1)
{
	if (_data_loaded == false) {
		return;
	}
    sprintf(SharedData::get_instance()->v6_object_list.at(_current_object).graphic_filename, "%s", arg1.toStdString().c_str());
    ui->object_preview_area->set_graphicfile(SharedData::get_instance()->FILEPATH+std::string("/images/sprites/objects/")+arg1.toStdString());
    ui->object_preview_area->repaint();
}

void object_tab::on_objectlist_combo_currentIndexChanged(int index)
{
	if (_data_loaded == false) {
		return;
	}

    fill_ability_data();

	_current_object = index;
    std::cout << ">>>>>>>>>>>>>>>> _current_object[" << _current_object << "], list.size[" << SharedData::get_instance()->v6_object_list.size() << "]" << std::endl;
    if (_current_object >= SharedData::get_instance()->v6_object_list.size()) {
        std::cout << "INVALID OBJ-N, LEAVE" << std::endl;
        return;
    }
    ui->name->setText(SharedData::get_instance()->v6_object_list.at(_current_object).name);
    ui->graphicfile_combo->setCurrentIndex(ui->graphicfile_combo->findText(QString(SharedData::get_instance()->v6_object_list.at(index).graphic_filename)));
    ui->graphic_w->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).size.width);
    ui->graphic_h->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).size.height);
    ui->type_combo->setCurrentIndex(SharedData::get_instance()->v6_object_list.at(_current_object).type);
    ui->timer->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).timer);
    ui->speed->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).speed);
    ui->limit->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).limit);
    fill_ability_data();
    ui->animate_on_active_checkbox->setChecked(!SharedData::get_instance()->v6_object_list.at(_current_object).animation_auto_start);
    ui->animation_loop_checkbox->setChecked(SharedData::get_instance()->v6_object_list.at(_current_object).animation_loop);
    ui->frame_duration_spinbox->setValue(SharedData::get_instance()->v6_object_list.at(_current_object).frame_duration);
    ui->animation_reverse_checkbox->setChecked(SharedData::get_instance()->v6_object_list.at(_current_object).animation_reverse);

}

void object_tab::on_name_textChanged(const QString &arg1)
{
    if (_data_loaded == false) { return; }
    sprintf(SharedData::get_instance()->v6_object_list.at(_current_object).name, "%s", arg1.toStdString().c_str());
    ui->objectlist_combo->setItemText(_current_object, QString(SharedData::get_instance()->v6_object_list.at(_current_object).name));
}

void object_tab::on_type_combo_currentIndexChanged(int index)
{
	if (_data_loaded == false) {
		return;
	}
    fill_ability_data();
    if (index == OBJ_DOOR_LOCKED || index == OBJ_BOSS_DOOR || index == OBJ_DOOR_KEY) {
        ui->key_comboBox->setEnabled(true);
    } else {
        ui->key_comboBox->setEnabled(false);
    }
    if (SharedData::get_instance()->v6_object_list.size() > 0) {
        SharedData::get_instance()->v6_object_list.at(_current_object).type = index;
    }

}

void object_tab::on_limit_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).limit = arg1;
}

void object_tab::on_graphic_w_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
	ui->object_preview_area->set_grid_w(arg1);
    SharedData::get_instance()->v6_object_list.at(_current_object).size.width = arg1;
}

void object_tab::on_graphic_h_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
	ui->object_preview_area->set_grid_h(arg1);
    SharedData::get_instance()->v6_object_list.at(_current_object).size.height = arg1;
}

void object_tab::on_speed_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).speed = arg1;
}

void object_tab::on_animate_on_active_checkbox_toggled(bool checked)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).animation_auto_start = !checked;
}

void object_tab::on_animation_loop_checkbox_toggled(bool checked)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).animation_loop = checked;
}

void object_tab::on_frame_duration_spinbox_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).frame_duration = arg1;
}

void object_tab::on_animation_reverse_checkbox_toggled(bool checked)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).animation_reverse = checked;
}

void object_tab::on_timer_valueChanged(int arg1)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).timer = arg1;
}

void object_tab::on_pushButton_clicked()
{
    SharedData::get_instance()->v6_object_list.push_back(v6_file_object());
    ui->objectlist_combo->addItem(QString("[") + QString::number(SharedData::get_instance()->v6_object_list.size()-1) + QString("] Object"));
    ui->objectlist_combo->setCurrentIndex(SharedData::get_instance()->v6_object_list.size()-1);

}

void object_tab::on_givenAbilityComboBox_currentIndexChanged(int index)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).given_ability = index;
}

void object_tab::on_key_comboBox_currentIndexChanged(int index)
{
    if (_data_loaded == false) { return; }
    SharedData::get_instance()->v6_object_list.at(_current_object).key_id = index;
}
