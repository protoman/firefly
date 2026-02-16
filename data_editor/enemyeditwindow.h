#ifndef ENEMYEDITWINDOW_H
#define ENEMYEDITWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class EnemyEditWindow : public QMainWindow
{
    Q_OBJECT

public:
    EnemyEditWindow(QWidget *parent = nullptr);
    ~EnemyEditWindow();

private:
    void load_data();

private slots:
    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // ENEMYEDITWINDOW_H
