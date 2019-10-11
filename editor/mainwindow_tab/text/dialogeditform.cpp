#include "dialogeditform.h"
#include "ui_dialogeditform.h"

#include "data/shareddata.h"

DialogEditForm::DialogEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogEditForm)
{
    ui->setupUi(this);



    ui->dialogTableView->setModel(&tableModel);

    fill_data();
}

DialogEditForm::~DialogEditForm()
{
    delete ui;
}

void DialogEditForm::on_dialogAddPushButton_clicked()
{
    int currentIndex = ui->diagloSeelectComboBox->currentIndex();
    SharedData::get_instance()->dialog_list.push_back(std::vector<std::string>());
    fill_data();
    ui->diagloSeelectComboBox->setCurrentIndex(currentIndex);
}

void DialogEditForm::on_diagloSeelectComboBox_currentIndexChanged(int index)
{
    if (data_loading) { return; }
    // set current valud in table model //
    SharedData::get_instance()->currentDialog = index;
    tableModel.updateData();
}

void DialogEditForm::fill_data()
{
    data_loading = true;
    ui->diagloSeelectComboBox->clear();
    for (int i=0; i<SharedData::get_instance()->dialog_list.size(); i++) {
        ui->diagloSeelectComboBox->addItem(QString::number(i));
    }
    data_loading = false;
}
