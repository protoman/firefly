#include "enemyedit.h"
#include "ui_enemyedit.h"
#include "file/data/map_elements.hpp"
#include "util/CommonUtils.hpp"

#include <QComboBox>

EnemyEdit::EnemyEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EnemyEdit)
{
    ui->setupUi(this);
    this->setWindowTitle("Enemies Editor");
    selectorCombobox = new QComboBox();
    ui->toolBar->addWidget(selectorCombobox); // Add the combobox to the mainToolBar
    connect(selectorCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectorIndexChanged(int))); // Connect signals and slots as needed
}

EnemyEdit::~EnemyEdit()
{
    delete ui;
}

void EnemyEdit::start(std::string data_directory)
{
    game_data_directory = data_directory;
    this->show();

    ui->centralwidget->blockSignals(true);
    ui->frameSelectWidget->setDataDirectory(game_data_directory);

    CommonUtils::fill_files_combo(game_data_directory + "/images/sprites/enemies", ui->GraphicFilenameComboBox, true);
    loadData();
    fillSelectorCombobox();
    fillFormWithData(0);

    ui->centralwidget->blockSignals(false);
}

void EnemyEdit::loadData()
{
    enemies = data::loadEnemies(game_data_directory);
    // need at least one npc
    if (enemies.enemy_list.size() == 0) {
        data::file_enemy enemy = data::file_enemy();
        enemy.id = 0;
        enemy.hp = 1;
        enemy.name = "NPC #0";
        enemies.enemy_list.emplace_back(enemy);
    }
}

void EnemyEdit::handleSelectorIndexChanged(int index) {
    fillFormWithData(index);
}

void EnemyEdit::on_actionAdd_triggered()
{
    int new_number = enemies.enemy_list.size();
    data::file_enemy enemy = data::file_enemy();
    enemy.id = 0;
    enemy.hp = 1;
    enemy.name = "NPC #" + std::to_string(new_number);
    enemies.enemy_list.emplace_back(enemy);
    fillSelectorCombobox();
    selectorCombobox->setCurrentIndex(new_number);
}


void EnemyEdit::on_actionSave_triggered()
{
    data::saveEnemies(game_data_directory, enemies);
}

void EnemyEdit::fillSelectorCombobox() {
    selectorCombobox->blockSignals(true);
    selectorCombobox->clear();
    for (data::file_enemy enemy : enemies.enemy_list) {
        selectorCombobox->addItem(enemy.name.c_str());
    }
    selectorCombobox->blockSignals(false);
}

void EnemyEdit::fillFormWithData(int selected_enemy) {
    ui->nameLineEdit->setText(enemies.enemy_list.at(selected_enemy).name.c_str());
    ui->HPSpinBox->setValue(enemies.enemy_list.at(selected_enemy).hp);
    ui->shieldTypeComboBox->setCurrentIndex(enemies.enemy_list.at(selected_enemy).shield_mode);
    ui->moveSpeedHorizontalDoubleSpinBox->setValue(enemies.enemy_list.at(selected_enemy).speed);
    ui->jumpSpeedDoubleSpinBox->setValue(enemies.enemy_list.at(selected_enemy).jump_speed);
    ui->rangeSpinBox->setValue(enemies.enemy_list.at(selected_enemy).range);
    ui->GraphicFilenameComboBox->setCurrentIndex(ui->GraphicFilenameComboBox->findText(QString::fromStdString(enemies.enemy_list.at(selected_enemy).graphic_filename)));
    ui->spriteSizeWidthSpinBox->setValue(enemies.enemy_list.at(selected_enemy).sprite_size.w);
    ui->spriteSizeHeightSpinBox->setValue(enemies.enemy_list.at(selected_enemy).sprite_size.h);
    ui->projectileOriginXSpinBox->setValue(enemies.enemy_list.at(selected_enemy).projectile_origin_point.x);
    ui->projectileOriginYSpinBox->setValue(enemies.enemy_list.at(selected_enemy).projectile_origin_point.y);

    ui->frameSelectWidget->updateGraphic(enemies.enemy_list.at(selected_enemy).sprite_size.w, enemies.enemy_list.at(selected_enemy).sprite_size.h, enemies.enemy_list.at(selected_enemy).graphic_filename);
}



void EnemyEdit::on_GraphicFilenameComboBox_currentTextChanged(const QString &arg1)
{
    if (selectorCombobox->currentIndex() > enemies.enemy_list.size()) {
        return;
    }
    enemies.enemy_list.at(selectorCombobox->currentIndex()).graphic_filename = arg1.toStdString();
    ui->frameSelectWidget->updateGraphic(ui->spriteSizeWidthSpinBox->value(), ui->spriteSizeHeightSpinBox->value(), arg1.toStdString());
}


void EnemyEdit::on_nameLineEdit_textChanged(const QString &arg1)
{
    if (selectorCombobox->currentIndex() > enemies.enemy_list.size()) {
        return;
    }
    enemies.enemy_list.at(selectorCombobox->currentIndex()).name = arg1.toStdString();
}


void EnemyEdit::on_spriteSizeWidthSpinBox_valueChanged(int arg1)
{
    enemies.enemy_list.at(selectorCombobox->currentIndex()).sprite_size.w = arg1;
    ui->frameSelectWidget->updateGraphic(arg1, ui->spriteSizeHeightSpinBox->value(), ui->GraphicFilenameComboBox->currentText().toStdString());
}


void EnemyEdit::on_spriteSizeHeightSpinBox_valueChanged(int arg1)
{
    enemies.enemy_list.at(selectorCombobox->currentIndex()).sprite_size.h = arg1;
    ui->frameSelectWidget->updateGraphic(ui->spriteSizeWidthSpinBox->value(), arg1, ui->GraphicFilenameComboBox->currentText().toStdString());

}

