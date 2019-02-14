#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QComboBox>
#include <QListWidget>

#include "mainwindow_tab/npc_edit.h"
#include "mainwindow_tab/object_tab.h"
#include "mainwindow_tab/projectile_edit.h"
#include "mainwindow_tab/artificial_inteligence_tab.h"
#include "mainwindow_tab/game_properties_tab.h"
#include "mainwindow_tab/map_tab.h"
#include "mainwindow_tab/player_edit.h"
#include "mainwindow_tab/anim_tiles_edit.h"
#include "mainwindow_tab/slopeedittab.h"
#include "mainwindow_tab/areaeditor.h"
#include "scenes/sceneeditorwindow.h"
#include "aboutwindow.h"
#include "files_editor/fileseditor.h"
#include "files_editor/stringseditor.h"


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //QStandardItemModel model;

    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadData();
    void closeEvent(QCloseEvent *event);
	void fillGraphicsList();
	void fillMapList();
	void fillNpcList();
	void fillObjList();
	void fillStageList();

    void show_critial_error(QString error);





private:
	void fill_NPC_edit_tab();
	void fill_background_list();
    void copy_path(QString src, QString dst);





public slots:
    void reload();
    void on_new_game_accepted(QString);
    void on_load_game_accepted();


 void on_npc_direction_combo_currentIndexChanged(int index);
	void on_spinBox_valueChanged(int );
	void on_listWidget_2_currentRowChanged(int currentRow);
	void on_toolBox_currentChanged(int index);
    void on_actionThree_triggered();
    void on_actionTwo_triggered();
    void on_actionOne_triggered();
    void on_editNPCButton_clicked();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_comboBox_currentIndexChanged(int index);
    void on_MainWindow_iconSizeChanged(QSize iconSize);
    void on_actionNew_triggered();
    void on_pallete_signalPalleteChanged();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionQuit_triggered();

	void on_comboBox_6_currentIndexChanged(int index);

	void on_tabWidget_currentChanged(int index);


	void on_checkBox_clicked(bool checked);

	void on_checkBox_2_clicked(bool checked);

	void on_checkBox_3_clicked(bool checked);

	void on_players_tab_maxshots_valueChanged(int arg1);

	void on_can_slide_checkbox_toggled(bool checked);

    void on_players_tab_movespeed_valueChanged(int arg1);

    void on_players_tab_hasshield_toggled(bool checked);

    void on_players_tab_hp_valueChanged(int arg1);

    void on_players_tab_name_textChanged(const QString &arg1);

    void on_chargedshot_combo_currentIndexChanged(int index);

    void on_players_tab_list_combo_2_currentIndexChanged(int index);

    void on_actionObjects_toggled(bool arg1);

    void on_actionNPCs_toggled(bool arg1);

    void on_actionTeleporters_toggled(bool arg1);

    void on_actionAbout_triggered();

    void on_actionImage_Browser_triggered();


private slots:
    void on_actionStrings_Editor_triggered();

    void on_actionZoomOne_triggered();

    void on_actionZoomTwo_triggered();

    void on_actionZoomThree_triggered();



    void on_actionGRID_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    npc_edit *npc_edit_tab;
    artificial_inteligence_tab *ai_edit_tab;
    object_tab *object_edit_tab;
    projectile_edit *projectile_edit_tab;
    map_tab *map_edit_tab;
    anim_tiles_edit *anim_tiles_edit_tab;
    SlopeEditTab* slope_edit_tab;


    // USED IN NPC TAB
	std::string _npcedit_tab_graphic;
	int _npcedit_tab_selectednpc;
	bool _data_loading;
    AboutWindow* about_window;
    FilesEditor* files_editor_window;
    StringsEditor* strings_editor_window;
    AreaEditor* area_editor;
};


#endif // MAINWINDOW_H
