#include "enemyedit.h"
#include "ui_enemyedit.h"
#include "file/data/map_elements.hpp"

#include <QComboBox>

EnemyEdit::EnemyEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EnemyEdit)
{
    ui->setupUi(this);

    selectorCombobox = new QComboBox();
    ui->toolBar->addWidget(selectorCombobox); // Add the combobox to the mainToolBar
    connect(selectorCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectorIndexChanged(int))); // Connect signals and slots as needed
    loadData();
    fillSelectorCombobox();
    fillFormWithData(0);
}

EnemyEdit::~EnemyEdit()
{
    delete ui;
}

void EnemyEdit::start()
{
    this->show();
}

void EnemyEdit::loadData()
{
    enemies = data::loadEnemies();
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
    data::saveEnemies(enemies);
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
}

