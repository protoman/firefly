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
        new_object.set_name("[0][RECOVERY] - Object Name");
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
    // Assuming UI elements exist based on ObjectData properties
    // ui->nameLineEdit->setText(QString::fromStdString(obj.get_name()));
    // ui->typeSelector->setCurrentIndex(static_cast<int>(obj.get_object_type()));
    // ... other fields as needed
}

void ObjectEdit::on_addButton_released()
{
    ObjectData new_object;
    int index = object_list.size();
    std::string type_str;
    ObjectDataType obj_type;

    switch (ui->typeSelector->currentIndex()) {
        case 0:
            obj_type = ObjectDataType::RECOVERY;
            type_str = "RECOVERY";
            break;
        case 1:
            obj_type = ObjectDataType::QUEST;
            type_str = "QUEST";
            break;
        case 2:
            obj_type = ObjectDataType::PLATFORM;
            type_str = "PLATFORM";
            break;
        case 3:
            obj_type = ObjectDataType::UTIL;
            type_str = "UTIL";
            break;
        default:
            obj_type = ObjectDataType::RECOVERY;
            type_str = "UNKNOWN";
            break;
    }

    new_object.set_object_type(obj_type);
    std::string name = "[" + std::to_string(index) + "][" + type_str + "] - Object Name";
    new_object.set_name(name);

    object_list.push_back(new_object);
    ui->currentObjectSelector->addItem(QString::fromStdString(name));
    ui->currentObjectSelector->setCurrentIndex(index);
}
