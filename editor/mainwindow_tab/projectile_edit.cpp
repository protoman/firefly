#include "projectile_edit.h"
#include "ui_projectile_edit.h"

#include "common.h"

projectile_edit::projectile_edit(QWidget *parent) : QWidget(parent), ui(new Ui::projectile_edit)
{
    ui->setupUi(this);
    fill_data();
}

projectile_edit::~projectile_edit()
{
    delete ui;
}

void projectile_edit::reload()
{
    fill_data();
}

void projectile_edit::fill_data()
{
    if (SharedData::get_instance()->FILEPATH.length() == 0) {
        return;
    }

    data_loading = true;
    common::fill_projectiles_combo(ui->projectileList_combo);
    common::fill_files_combo(std::string("/images/projectiles"), ui->graphic_filename);
    common::fill_files_combo(std::string("/sfx"), ui->sfxFilename_comboBox);
	common::fill_trajectories_combo(ui->trajectory);
    Mediator::get_instance()->current_projectile = 0;
    set_edit_data(0);
    data_loading = false;
}

void projectile_edit::set_edit_data(int index)
{
    if (index < 0 || index >= GameData::get_instance()->projectile_list_v0.size()) {
        return;
    }
    data_loading = true;
	Mediator::get_instance()->current_projectile = index;
    ui->name->setText(QString(GameData::get_instance()->projectile_list_v0.at(index).name.c_str()));
    ui->graphic_filename->setCurrentIndex(ui->graphic_filename->findText(QString(GameData::get_instance()->projectile_list_v0.at(index).graphic_filename.c_str())));
    ui->sfxFilename_comboBox->setCurrentIndex(ui->sfxFilename_comboBox->findText(QString(GameData::get_instance()->projectile_list_v0.at(index).sfx_filename.c_str())));
    ui->trajectory->setCurrentIndex(GameData::get_instance()->projectile_list_v0.at(index).trajectory);
    ui->img_w->setValue(GameData::get_instance()->projectile_list_v0.at(index).size.width);
    ui->img_h->setValue(GameData::get_instance()->projectile_list_v0.at(index).size.height);
    if (GameData::get_instance()->projectile_list_v0.at(index).is_destructible == true) {
		ui->projectileDestructibleCheckBox->setChecked(true);
	} else {
		ui->projectileDestructibleCheckBox->setChecked(false);
	}
    ui->projectileHitPointsSpinBox->setValue(GameData::get_instance()->projectile_list_v0.at(index).hp);
    ui->max_shots->setValue(GameData::get_instance()->projectile_list_v0.at(index).max_shots);
    ui->speed->setValue(GameData::get_instance()->projectile_list_v0.at(index).speed);
    ui->damage->setValue(GameData::get_instance()->projectile_list_v0.at(index).damage);

    if (GameData::get_instance()->projectile_list_v0.size() == 0) {
        GameData::get_instance()->projectile_list_v0.push_back(file_projectile_v0());
    }

    ui->explosive_checkBox->setChecked(GameData::get_instance()->projectile_list_v0.at(index).is_explosive);
    ui->vanishOnHit_checkBox->setChecked(GameData::get_instance()->projectile_list_v0.at(index).vanishes_on_hit);

	ui->projectilePreviewAreaWidget->repaint();
    data_loading = false;
}

void projectile_edit::on_projectileList_combo_currentIndexChanged(int index)
{
    if (data_loading) { return; }
    Mediator::get_instance()->current_projectile = index;
    set_edit_data(index);
}

void projectile_edit::on_name_textChanged(const QString &arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).name = arg1.toStdString();
    ui->projectileList_combo->setItemText(Mediator::get_instance()->current_projectile, arg1);
}

void projectile_edit::on_graphic_filename_currentIndexChanged(const QString &arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).graphic_filename = arg1.toStdString();
	ui->projectilePreviewAreaWidget->repaint();
}


void projectile_edit::on_sfxFilename_comboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).sfx_filename = arg1.toStdString();
}

void projectile_edit::on_trajectory_currentIndexChanged(int index)
{
    if (data_loading) { return; }
	PROJECTILE_TRAJECTORIES temp = (PROJECTILE_TRAJECTORIES)index;
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).trajectory = temp;
}

void projectile_edit::on_img_w_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).size.width = arg1;
	ui->projectilePreviewAreaWidget->repaint();
}

void projectile_edit::on_img_h_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).size.height = arg1;
	ui->projectilePreviewAreaWidget->repaint();
}

void projectile_edit::on_projectileDestructibleCheckBox_toggled(bool checked)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).is_destructible = checked;
}

void projectile_edit::on_projectileHitPointsSpinBox_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).hp = arg1;
}

void projectile_edit::on_max_shots_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).max_shots = arg1;
}

void projectile_edit::on_speed_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).speed = arg1;
}

void projectile_edit::on_damage_valueChanged(int arg1)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).damage = arg1;
}

void projectile_edit::on_pushButton_clicked()
{
    GameData::get_instance()->projectile_list_v0.push_back(file_projectile_v0());
    ui->projectileList_combo->addItem(QString("[") + QString::number(GameData::get_instance()->projectile_list_v0.size()-1) + QString("] Projectile Name"));
    ui->projectileList_combo->setCurrentIndex(GameData::get_instance()->projectile_list_v0.size()-1);
}


void projectile_edit::on_explosive_checkBox_toggled(bool checked)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).is_explosive = checked;
}

void projectile_edit::on_vanishOnHit_checkBox_toggled(bool checked)
{
    if (data_loading) { return; }
    GameData::get_instance()->projectile_list_v0.at(Mediator::get_instance()->current_projectile).vanishes_on_hit = checked;
}
