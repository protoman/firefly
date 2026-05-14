#include "objectedit.h"
#include "ui_objectedit.h"
#include "util/CommonUtils.hpp"
#include <QComboBox>
#include <fstream>

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

    // Fill the graphic filename combobox with .png files from the parent project's object sprites folder
    std::string objects_sprites_path = CommonUtils::get_instance()->get_objects_sprites_path();
    CommonUtils::fill_files_combo(objects_sprites_path, ui->graphicFilenameComboBox, true);

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
    std::string parent_project_path = CommonUtils::get_instance()->get_parent_project_path();
    std::string object_data_path = parent_project_path + "/game_data/object_data.json";
    
    object_list.clear();
    
    std::ifstream input_file(object_data_path);
    if (input_file.is_open()) {
        try {
            cereal::JSONInputArchive iarchive(input_file);
            iarchive(cereal::make_nvp("objects", object_list));
        } catch (...) {
            // If JSON parsing fails, leave list empty
            object_list.clear();
        }
        input_file.close();
    }

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

void ObjectEdit::saveData()
{
    std::string parent_project_path = CommonUtils::get_instance()->get_parent_project_path();
    std::string object_data_path = parent_project_path + "/game_data/object_data.json";
    
    std::ofstream output_file(object_data_path);
    if (output_file.is_open()) {
        cereal::JSONOutputArchive oarchive(output_file);
        oarchive(cereal::make_nvp("objects", object_list));
        output_file.close();
    }
}

void ObjectEdit::on_actionSave_triggered()
{
    saveData();
}

