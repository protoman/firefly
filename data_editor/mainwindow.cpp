#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Game Data Editor");

    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(ui->actionPlace_Enemy);
    toolGroup->addAction(ui->actionPlace_Object);
    toolGroup->setExclusive(true); // Only one action can be checked at a time
    ui->actionPlace_Enemy->setChecked(true);

    loadConfig();
    ui->editAreaWidget->setGameDataDirectory(config.game_data_directory);
    ui->editAreaWidget->loadMapData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadConfig() {
    std::ifstream is(config_filename);

    if (is.is_open()) {
        cereal::JSONInputArchive iarchive(is); // Create an input archive
        std::string directory;
        iarchive(directory); // Read the data from the archive
        config.game_data_directory = directory;
    } else {
        QString dir;
        while (dir.isEmpty()) {
            QString dir = QFileDialog::getExistingDirectory(this, tr("Select Game Data Folder"),
                                                                QDir::currentPath(),
                                                                QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);

            if (!dir.isEmpty()) {
                config.game_data_directory = dir.toStdString();
                std::ofstream os(config_filename);
                cereal::JSONOutputArchive oarchive(os);
                oarchive(config.game_data_directory);
                break;
            }
        }
    }
}

void MainWindow::on_actionEnemy_Editor_triggered()
{
    enemyEdit.start(config.game_data_directory);
}


void MainWindow::on_actionPlace_Enemy_triggered()
{
    editMode = e_editMode_ENEMY;
}


void MainWindow::on_actionPlace_Object_triggered()
{
    editMode = e_editMode_OBJECT;
}


void MainWindow::on_actionObject_Editor_triggered()
{
    objectEdit.start(config.game_data_directory);
}

