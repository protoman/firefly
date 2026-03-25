#include "objectedit.h"
#include "ui_objectedit.h"
#include "util/CommonUtils.hpp"
#include <QComboBox>

ObjectEdit::ObjectEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ObjectEdit)
{
    ui->setupUi(this);
    this->setWindowTitle("Objects Editor");
    connect(ui->currentObjectSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectorIndexChanged(int)));
}

ObjectEdit::~ObjectEdit()
{
    delete ui;
}

void ObjectEdit::start(std::string data_directory)
{
    game_data_directory = data_directory;
    this->show();

    ui->centralwidget->blockSignals(true);
    loadData();
    fillSelectorCombobox();
    if (!object_list.empty()) {
        fillFormWithData(0);
    }

    ui->centralwidget->blockSignals(false);
}

void ObjectEdit::reload_data() {
    loadData();
    fillSelectorCombobox();
}

void ObjectEdit::loadData()
{
    data::file_objects loaded_objects = data::loadObjects(game_data_directory);
    object_list = loaded_objects.object_list;

    if (object_list.empty()) {
        ObjectData new_object;
        ObjectDataTypeInfo info = ObjectData::get_type_info(ObjectDataType::RECOVERY);
        new_object.set_name("[0][" + info.name + "] - Object Name");
        new_object.set_object_type(ObjectDataType::RECOVERY);
        object_list.push_back(new_object);
    }
}

void ObjectEdit::handleSelectorIndexChanged(int index) {
    if (index >= 0 && index < static_cast<int>(object_list.size())) {
        fillFormWithData(index);
    }
}

void ObjectEdit::fillSelectorCombobox() {
    ui->currentObjectSelector->blockSignals(true);
    ui->currentObjectSelector->clear();
    for (const auto& obj : object_list) {
        ui->currentObjectSelector->addItem(QString::fromStdString(obj.get_name()));
    }
    ui->currentObjectSelector->blockSignals(false);
}

void ObjectEdit::fillFormWithData(int index) {
    if (index < 0 || index >= static_cast<int>(object_list.size())) return;

    const ObjectData& obj = object_list.at(index);
    // UI mapping will go here
}

void ObjectEdit::on_addButton_released()
{
    int index = object_list.size();
    ObjectDataType obj_type = ObjectData::get_type_from_index(ui->typeSelector->currentIndex());
    ObjectDataTypeInfo info = ObjectData::get_type_info(obj_type);

    ObjectData new_object;
    new_object.set_object_type(obj_type);

    std::string name = "[" + std::to_string(index) + "][" + info.name + "] - Object Name";
    new_object.set_name(name);

    object_list.push_back(new_object);
    ui->currentObjectSelector->addItem(QString::fromStdString(name));
    ui->currentObjectSelector->setCurrentIndex(index);
}
