#ifndef OBJECTEDIT_H
#define OBJECTEDIT_H

#include <QMainWindow>

#include "data/model/platform_object_data.h"
#include "data/model/quest_object_data.h"
#include "data/model/recovery_object_data.h"
#include "data/model/util_object_data.h"

namespace Ui {
class ObjectEdit;
}

class ObjectEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit ObjectEdit(QWidget *parent = nullptr);
    ~ObjectEdit();
    void start(std::string data_directory);
    void reload_data();

private slots:


    void on_addButton_released();

private:
    Ui::ObjectEdit *ui;
    std::vector<ObjectData> object_list;

};

#endif // OBJECTEDIT_H
