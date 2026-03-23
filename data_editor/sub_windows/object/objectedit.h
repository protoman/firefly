#ifndef OBJECTEDIT_H
#define OBJECTEDIT_H

#include <QMainWindow>

namespace Ui {
class ObjectEdit;
}

class ObjectEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit ObjectEdit(QWidget *parent = nullptr);
    ~ObjectEdit();

private:
    Ui::ObjectEdit *ui;
};

#endif // OBJECTEDIT_H
