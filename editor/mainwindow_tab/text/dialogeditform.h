#ifndef DIALOGEDITFORM_H
#define DIALOGEDITFORM_H

#include <QWidget>

namespace Ui {
class DialogEditForm;
}

class DialogEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit DialogEditForm(QWidget *parent = nullptr);
    ~DialogEditForm();

private:
    Ui::DialogEditForm *ui;
};

#endif // DIALOGEDITFORM_H
