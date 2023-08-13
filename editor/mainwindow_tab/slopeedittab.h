#ifndef SLOPEEDITTAB_H
#define SLOPEEDITTAB_H

#include <QWidget>

#include "data/shareddata.h"
#include "common.h"

namespace Ui {
class SlopeEditTab;
}

class SlopeEditTab : public QWidget
{
    Q_OBJECT

public:
    explicit SlopeEditTab(QWidget *parent = nullptr);
    ~SlopeEditTab();
    void reload();

private:
    void fill_data();

private slots:
    void on_addSlopePushButton_clicked();

    void on_currentSlopeComboBox_currentIndexChanged(int index);

    void on_imageComboBox_currentIndexChanged(const QString &arg1);

    void on_left_spinBox_valueChanged(int arg1);

    void on_right_spinBox_valueChanged(int arg1);

    void currentPosChanged(int pos);

private:
    Ui::SlopeEditTab *ui;
    bool data_loading = false;
    int currentSlopePos = 0;
    int selectedSlopePos = 0;
};

#endif // SLOPEEDITTAB_H
