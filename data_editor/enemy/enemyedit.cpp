#include "enemyedit.h"
#include "ui_enemyedit.h"
#include "file/data/map_elements.hpp"

#include <QComboBox>

EnemyEdit::EnemyEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EnemyEdit)
{
    ui->setupUi(this);

    QComboBox* myComboBox = new QComboBox();
    // Add items to the combo box
    myComboBox->addItem("Option 1");
    myComboBox->addItem("Option 2");
    // Add the combobox to the mainToolBar
    ui->toolBar->addWidget(myComboBox);
    // Connect signals and slots as needed
    //connect(myComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleComboBoxIndexChanged(int)));
    loadData();
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
    // need at least one npc
    if (enemies.enemy_list.size() == 0) {
        data::file_enemy enemy = data::file_enemy();
        enemy.id = 0;
        enemy.hp = 1;
        enemy.name = "NPC #0";
        enemies.enemy_list.emplace_back(enemy);
    }
    enemies = data::loadEnemies();

}

void EnemyEdit::on_actionAdd_triggered()
{
    data::map_elements new_data;
    data::map_element_pos enemy1 = data::map_element_pos(1, 10, 10, false, false);
    data::map_element_pos npc1 = data::map_element_pos(2, 20, 20, false, false);
    data::map_element_pos object1 = data::map_element_pos(3, 30, 30, false, false);
    new_data.enemies.emplace_back(enemy1);
    new_data.npcs.emplace_back(npc1);
    new_data.objects.emplace_back(object1);
    data::saveMapElements(new_data);
}


void EnemyEdit::on_actionSave_triggered()
{
    data::saveEnemies(enemies);
}

