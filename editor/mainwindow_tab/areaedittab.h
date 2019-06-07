#ifndef AREAEDITTAB_H
#define AREAEDITTAB_H

#include <QWidget>

namespace Ui {
class AreaEditTab;
}

class AreaEditTab : public QWidget
{
    Q_OBJECT

public:
    explicit AreaEditTab(QWidget *parent = nullptr);
    ~AreaEditTab();

private:
    Ui::AreaEditTab *ui;
};

#endif // AREAEDITTAB_H
