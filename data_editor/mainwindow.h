#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "file/data/file_data_enemy.hpp"
#include "enemy/enemyedit.h"
#include "sub_windows/object/objectedit.h"

enum e_editMode {
    e_editMode_ENEMY,
    e_editMode_NPC,
    e_editMode_OBJECT
};

namespace Ui {
class MainWindow;
}


struct editor_config {
    std::string game_data_directory;

    template <class Archive>
    void serialize(Archive & ar) {
        ar(CEREAL_NVP(game_data_directory));
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void loadConfig();

private slots:
    void on_actionEnemy_Editor_triggered();
    void on_actionPlace_Enemy_triggered();
    void on_actionPlace_Object_triggered();

    void on_actionObject_Editor_triggered();

private:
    Ui::MainWindow *ui;
    EnemyEdit enemyEdit;
    ObjectEdit objectEdit;
    e_editMode editMode = e_editMode_ENEMY;
    editor_config config;
    std::string config_filename = "editor_config.json";

};

#endif // MAINWINDOW_H
