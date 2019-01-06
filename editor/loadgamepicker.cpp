#include "loadgamepicker.h"
#include "ui_loadgamepicker.h"
#include <mediator.h>
#include <stdio.h>
#include <vector>

#include "file/file_io.h"
#include "mainwindow.h"

#include "data/shareddata.h"

loadGamePicker::loadGamePicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadGamePicker)
{
	ui->setupUi(this);

    file_io fio;
    std::vector<std::string> game_list = fio.read_game_list();

    for (int i=0; i<game_list.size(); i++) {
        ui->gameList->addItem(QString(game_list.at(i).c_str()));
    }
    ui->gameList->setCurrentRow(0);


}

loadGamePicker::~loadGamePicker()
{
    delete ui;
}

void loadGamePicker::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void loadGamePicker::on_buttonBox_accepted()
{
    std::cout << ">>>>>>>>>> ui->gameList->currentRow(): " << ui->gameList->currentRow() << std::endl;

    SharedData::get_instance()->FILEPATH = SharedData::get_instance()->GAMEPATH + std::string("/games/") + ui->gameList->currentItem()->text().toStdString() + std::string("/");
    SharedData::get_instance()->GAMENAME = ui->gameList->currentItem()->text().toStdString();
    Mediator::get_instance()->load_game();
    emit game_picked();

	//printf("DEBUG - loadGamePicker::on_buttonBox_accepted - selectedItem: %d\n", ui->gameList->currentRow());
}
