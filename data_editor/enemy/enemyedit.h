#ifndef ENEMYEDIT_H
#define ENEMYEDIT_H

#include <QComboBox>
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

public slots:
    void handleSelectorIndexChanged(int index);

private slots:
    void on_actionAdd_triggered();
    void on_actionSave_triggered();

private:
    void fillSelectorCombobox();
    void fillFormWithData(int selected_enemy);

private:
    Ui::EnemyEdit *ui;
    data::file_enemies enemies;
    QComboBox* selectorCombobox;
};

#endif // ENEMYEDIT_H
