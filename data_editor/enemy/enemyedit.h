#ifndef ENEMYEDIT_H
#define ENEMYEDIT_H

#include <QMainWindow>
#include "file/data/file_data_enemy.hpp"

namespace Ui {
class EnemyEdit;
}

class EnemyEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnemyEdit(QWidget *parent = nullptr);
    ~EnemyEdit();
    void start();
    void loadData();

private slots:
    void on_actionAdd_triggered();

    void on_actionSave_triggered();

private:
    Ui::EnemyEdit *ui;
    data::file_enemies enemies;
};

#endif // ENEMYEDIT_H
