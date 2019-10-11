#ifndef DIALOGEDITFORM_H
#define DIALOGEDITFORM_H

#include <QWidget>
#include "./model/dialogtableitemmodel.h"

namespace Ui {
class DialogEditForm;
}

class DialogEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit DialogEditForm(QWidget *parent = nullptr);
    ~DialogEditForm();

private slots:
    void on_dialogAddPushButton_clicked();
    void on_diagloSeelectComboBox_currentIndexChanged(int index);

private:
    void fill_data();

private:
    Ui::DialogEditForm *ui;
    DialogTableItemModel tableModel;
    bool data_loading = false;
};

#endif // DIALOGEDITFORM_H
