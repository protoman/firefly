#ifndef OBJECTEDIT_H
#define OBJECTEDIT_H

#include <QMainWindow>
#include <vector>
#include "data/model/object_data.h"
#include "file/data/file_data_object.hpp"

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

public slots:
    void handleSelectorIndexChanged(int index);

private slots:
    void on_addButton_released();

private:
    void loadData();
    void fillSelectorCombobox();
    void fillFormWithData(int index);

private:
    Ui::ObjectEdit *ui;
    std::vector<ObjectData> object_list;
    std::string game_data_directory;

};

#endif // OBJECTEDIT_H
