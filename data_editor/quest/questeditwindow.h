#ifndef QUESTEDITWINDOW_H
#define QUESTEDITWINDOW_H

#include <QMainWindow>

namespace Ui {
class QuestEditWindow;
}

class QuestEditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuestEditWindow(QWidget *parent = nullptr);
    ~QuestEditWindow();

private:
    Ui::QuestEditWindow *ui;
};

#endif // QUESTEDITWINDOW_H
