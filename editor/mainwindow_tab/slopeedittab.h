#ifndef SLOPEEDITTAB_H
#define SLOPEEDITTAB_H

#include <QWidget>

namespace Ui {
class SlopeEditTab;
}

class SlopeEditTab : public QWidget
{
    Q_OBJECT

public:
    explicit SlopeEditTab(QWidget *parent = nullptr);
    ~SlopeEditTab();

private:
    Ui::SlopeEditTab *ui;
};

#endif // SLOPEEDITTAB_H
