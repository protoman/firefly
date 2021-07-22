#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defines.h"
#include "addwizard.h"
#include "dialognpcedit.h"
#include "dialogobjectedit.h"
#include "loadgamepicker.h"
#include "newgamedialog.h"

#include <QListView>
#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>

#include "../defines.h"
#include "../file/version.h"
#include "file/fio_scenes.h"


#include "common.h"

#include "data/shareddata.h"

bool background_filled = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _npcedit_tab_selectednpc(0), _data_loading(false)
{
	ui->setupUi(this);
    QString window_title = QString("FIREFLY EDITOR ") + QString(VERSION_NUMBER);
    setWindowTitle(window_title);

	// insert NPC tab form
	npc_edit_tab = new npc_edit();
    ui->NPCScrollArea->setWidget(npc_edit_tab);


	// insert AI tab form
	ai_edit_tab = new artificial_inteligence_tab();
    ui->AIScrollArea->setWidget(ai_edit_tab);

	// insert OBJECT tab form
	object_edit_tab = new object_tab();
    ui->ObjectScrollArea->setWidget(object_edit_tab);


    // insert PROJECTILE tab form
    projectile_edit_tab = new projectile_edit();
    ui->ProjectileScrollArea->setWidget(projectile_edit_tab);


    map_edit_tab = new map_tab();
    ui->MapScrollArea->setWidget(map_edit_tab);

    anim_tiles_edit_tab = new anim_tiles_edit();
    ui->anim_tab_scrollArea->setWidget(anim_tiles_edit_tab);

    slope_edit_tab = new SlopeEditTab;
    ui->SlopeEditScrollArea->setWidget(slope_edit_tab);

    //areaEditScrollArea
    area_edit_tab = new AreaEditTab;
    ui->areaEditScrollArea->setWidget(area_edit_tab);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadData()
{
    Mediator::get_instance()->load_game();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "FIREFLY EDITOR :: Game Editor", tr("Save data before leaving?\n"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        Mediator::get_instance()->save_game();
    } else if (resBtn == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    event->accept();

    /*
    QMessageBox msgBox;
    msgBox.setWindowTitle("FIREFLY EDITOR");
    msgBox.setText("Save data before leaving?\n");
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);

    QMessageBox::StandardButton resBtn = msgBox.exec();
    if (resBtn == QMessageBox::Yes) {
        on_actionSave_triggered();
    } else if (resBtn == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    */
    event->accept();
}

void MainWindow::show_critial_error(QString error_msg)
{
    QMessageBox msgBox;
    msgBox.setText(error_msg);
    msgBox.exec();
    close();
}

void MainWindow::copy_path(QString src, QString dst)
{
    src = src.replace(QString("//"), QString("/"));
    dst = dst.replace(QString("//"), QString("/"));
    //std::cout << "MainWindow::copy_path::src: " << src.toStdString() << ", dst: " << dst.toStdString() << std::endl;
    //exit(-1);
    QDir dir(src);
    if (! dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        QDir dst_dir(dst_path);
        dst_dir.mkpath(dst_path);
        copy_path(src+ QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
    }
}

void MainWindow::reload()
{
    Mediator::get_instance()->selectedNPC = 0;
    Mediator::get_instance()->currentGame = 0;
    Mediator::get_instance()->current_player = 0;
    Mediator::get_instance()->current_weapon = 0;
    projectile_edit_tab->reload();
    object_edit_tab->reload();
    ai_edit_tab->reload();
    npc_edit_tab->reload();
    map_edit_tab->reload();
    anim_tiles_edit_tab->reload();
    slope_edit_tab->reload();
    area_edit_tab->reload();
    this->show();
}





void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionSave_triggered()
{
    Mediator::get_instance()->save_game();
}

void MainWindow::on_actionOpen_triggered()
{
    this->hide();
	QDialog *open = new loadGamePicker;
    QObject::connect(open, SIGNAL(game_picked()), this, SLOT(reload()));
	open->show();
}

void MainWindow::on_pallete_signalPalleteChanged()
{
	printf("DEBUG on_pallete_signalPalleteChanged\n");
}



void MainWindow::on_actionNew_triggered()
{
    NewGameDialog *new_game_dialog = new NewGameDialog();
    QObject::connect(new_game_dialog, SIGNAL(on_accepted(QString)), this, SLOT(on_new_game_accepted(QString)));
    new_game_dialog->show();
}


// ------------------- EDIT BUTTONS --------------------- //






void MainWindow::on_MainWindow_iconSizeChanged(QSize iconSize)
{
	Q_UNUSED (iconSize);
	saveGeometry();
}







void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    Mediator::get_instance()->layerLevel = index+1;
    map_edit_tab->update_edit_area();
}


void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
	printf(">>>> MainWindow::on_listWidget_currentRowChanged, row: %d\n", currentRow);
    Mediator::get_instance()->selectedNPC = currentRow;
}

void MainWindow::on_editNPCButton_clicked()
{
    Mediator::get_instance()->editModeNPC = 1;
	QDialog *npc_editor = new DialogNPCEdit;
	npc_editor->show();
	QObject::connect(npc_editor, SIGNAL(finishedNPCEditor()), this, SLOT(reloadComboItems()));
}


void MainWindow::on_actionOne_triggered()
{
    Mediator::get_instance()->layerLevel = 1;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionTwo_triggered()
{
    Mediator::get_instance()->layerLevel = 2;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionThree_triggered()
{
    Mediator::get_instance()->layerLevel = 3;
    map_edit_tab->update_edit_area();
}




void MainWindow::on_toolBox_currentChanged(int index)
{
	Q_UNUSED (index);
    Mediator::get_instance()->selectedNPC = -1;
}


void MainWindow::on_listWidget_2_currentRowChanged(int currentRow)
{
    Mediator::get_instance()->terrainType = currentRow+1;
}

void MainWindow::on_spinBox_valueChanged(int value)
{
        Mediator::get_instance()->zoom = value;
}


void MainWindow::on_npc_direction_combo_currentIndexChanged(int index)
{
    Mediator::get_instance()->npc_direction = index;
}








void MainWindow::on_comboBox_6_currentIndexChanged(int index)
{
    Mediator::get_instance()->current_player = index;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    reload();
}




void MainWindow::on_checkBox_clicked(bool checked)
{
    Mediator::get_instance()->show_background_color = checked;
    map_edit_tab->update_edit_area();
}




void MainWindow::on_checkBox_2_clicked(bool checked)
{
    Mediator::get_instance()->show_bg1 = checked;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_checkBox_3_clicked(bool checked)
{
    Mediator::get_instance()->show_fg_layer = checked;
    map_edit_tab->update_edit_area();
}


void MainWindow::on_players_tab_maxshots_valueChanged(int arg1)
{
    if (_data_loading == true) {
		return;
	}
    std::cout << "Mediator::get_instance()->current_player: " << Mediator::get_instance()->current_player << ", max_shots: " << Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].max_shots << std::endl;
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].max_shots = arg1;
    std::cout << "max_shots: " << Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].max_shots << std::endl;
}

void MainWindow::on_can_slide_checkbox_toggled(bool checked)
{
    if (_data_loading == true) {
        return;
    }
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].can_slide = checked;
}

void MainWindow::on_players_tab_movespeed_valueChanged(int arg1)
{
    if (_data_loading == true) {
        return;
    }
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].move_speed = arg1;
}

void MainWindow::on_players_tab_hasshield_toggled(bool checked)
{
    if (_data_loading == true) {
        return;
    }
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].have_shield = checked;
}

void MainWindow::on_players_tab_hp_valueChanged(int arg1)
{
    if (_data_loading == true) {
        return;
    }
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].HP = arg1;
}

void MainWindow::on_players_tab_name_textChanged(const QString &arg1)
{
    if (_data_loading == true) {
        return;
    }
    sprintf(Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].name, "%s", arg1.toStdString().c_str());
}

void MainWindow::on_chargedshot_combo_currentIndexChanged(int index)
{
    if (_data_loading == true) {
        return;
    }
    Mediator::get_instance()->player_list_v3_1[Mediator::get_instance()->current_player].full_charged_projectile_id = index;
}

void MainWindow::on_players_tab_list_combo_2_currentIndexChanged(int index)
{
    Mediator::get_instance()->current_player = index;
    std::cout << "MainWindow::on_players_tab_list_combo_2_currentIndexChanged - index: " << index << ", max_shots: " << Mediator::get_instance()->player_list_v3_1[index].max_shots << std::endl;
}

void MainWindow::on_actionObjects_toggled(bool arg1)
{
    Mediator::get_instance()->show_objects_flag = arg1;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionNPCs_toggled(bool arg1)
{
    Mediator::get_instance()->show_npcs_flag = arg1;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionTeleporters_toggled(bool arg1)
{
    Mediator::get_instance()->show_teleporters_flag = arg1;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionAbout_triggered()
{
    about_window = new AboutWindow();
    about_window->show();

}

void MainWindow::on_actionImage_Browser_triggered()
{
    files_editor_window = new FilesEditor();
    files_editor_window->show();
}


void MainWindow::on_new_game_accepted(QString name)
{
    /// @TODO: create game files
    QString games_folder_path = QString(SharedData::get_instance()->GAMEPATH.c_str()) + QString("/games/");
    if (QDir(games_folder_path).exists() == false) {
        if (QDir().mkdir(games_folder_path) == false) {
            QString error_msg = QString("Can't create games container folder '") + games_folder_path + QString("'.");
            show_critial_error(error_msg);
            return;
        }
    }
    QString filepath = QString(SharedData::get_instance()->GAMEPATH.c_str()) + QString("/games/") + name;
    if (QDir().mkdir(filepath) == false) {
        QString error_msg = QString("Can't create new game folder '") + filepath + QString("'.");
        show_critial_error(error_msg);
        return;
    }

    // copy data directories
    QString template_path = QString(SharedData::get_instance()->GAMEPATH.c_str()) + QString("/template/");
    copy_path(template_path, filepath);

    SharedData::get_instance()->FILEPATH = SharedData::get_instance()->GAMEPATH + std::string("/games/") + name.toStdString() + std::string("/");

    // generate empty/default game files
    file_io fio;
    fio.generate_files();

    fio_strings fio_str;
    fio_str.create_files();

    std::string scenes_filename = std::string(SharedData::get_instance()->FILEPATH) + "/scenes/";

    if (QDir(scenes_filename.c_str()).exists() == false) {
        QDir().mkdir(scenes_filename.c_str());
    }

    fio_scenes fio_scenes;
    fio_scenes.generate_files();


    /// @TODO: copy image files
    Mediator::get_instance()->load_game();

    SharedData::get_instance()->GAMENAME = name.toStdString();

    reload();

    this->show();
}

void MainWindow::on_load_game_accepted()
{
    Mediator::get_instance()->load_game();
    reload();
}

void MainWindow::on_actionStrings_Editor_triggered()
{
    strings_editor_window = new StringsEditor(this, 0);
    strings_editor_window->show();
}


void MainWindow::on_actionZoomOne_triggered()
{
    Mediator::get_instance()->zoom = 1;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionZoomTwo_triggered()
{
    Mediator::get_instance()->zoom = 2;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionZoomThree_triggered()
{
    Mediator::get_instance()->zoom = 3;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_actionGRID_toggled(bool arg1)
{
    Mediator::get_instance()->show_grid = !Mediator::get_instance()->show_grid;
    map_edit_tab->update_edit_area();
}

void MainWindow::on_areaEditTabWidget_currentChanged(int index)
{
    map_edit_tab->reload();
}

void MainWindow::on_actionOverlay_toggled(bool arg1)
{
    Mediator::get_instance()->show_fg_layer = arg1;
    map_edit_tab->update_edit_area();
}
