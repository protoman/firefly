#include "objectedit.h"
#include "ui_objectedit.h"

ObjectEdit::ObjectEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ObjectEdit)
{
    ui->setupUi(this);
}

ObjectEdit::~ObjectEdit()
{
    delete ui;
}

void ObjectEdit::start(std::string data_directory)
{
    this->show();
}

void ObjectEdit::reload_data() {
}


void ObjectEdit::on_addButton_released()
{
    ObjectData new_object;
    switch (ui->typeSelector->currentIndex()) {
        case 0:
            new_object.set_object_type(ObjectDataType::RECOVERY);
            break;
        case 1:
            new_object.set_object_type(ObjectDataType::QUEST);
            break;
        case 2:
            new_object.set_object_type(ObjectDataType::PLATFORM);
            break;
        case 3:
            new_object.set_object_type(ObjectDataType::UTIL);
            break;
        default:
            break;
    }
    object_list.push_back(new_object);

}
