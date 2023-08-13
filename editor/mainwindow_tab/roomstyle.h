#ifndef ROOMSTYLE_H
#define ROOMSTYLE_H

#include <QDialog>
#include "data/shareddata.h"

namespace Ui {
class RoomStyle;
}

class RoomStyle : public QDialog
{
    Q_OBJECT

public:
    explicit RoomStyle(QWidget *parent = nullptr);
    ~RoomStyle();
    void reload();

private:
    void fill_data();
    void set_data();
    void set_layer_data();

private slots:
    void on_pushButton_clicked();
    void on_styleNameLineEdit_textChanged(const QString &arg1);
    void on_styleTilesetComboBox_currentIndexChanged(const QString &arg1);
    void on_StyleBackgroundColorPushButton_clicked();
    void on_styleSelectComboBox_currentIndexChanged(int index);
    void on_styleMusicFilenameComboBox_currentIndexChanged(const QString &arg1);
    void on_styleGfxEffectComboBox_currentIndexChanged(int index);
    void on_styleGfxModeComboBox_currentIndexChanged(int index);
    void on_layerSelectorComboBox_currentIndexChanged(int index);
    void on_layerImageComboBox_currentIndexChanged(const QString &arg1);
    void on_layerVerticalPositionSpinBox_valueChanged(int arg1);
    void on_layerSpeedDoubleSpinBox_valueChanged(double arg1);
    void on_layerAutoscrollComboBox_currentIndexChanged(int index);

    void on_layerAnimationTimerSpinBox_valueChanged(int arg1);

    void on_layerAnimationWidthSpinBox_valueChanged(int arg1);

    void on_layerRepeatXCheckBox_toggled(bool checked);

    void on_layerRepeatYCheckBox_toggled(bool checked);

    void on_layerAlphaSpinBox_valueChanged(int arg1);

public slots:
    void on_color_selected1(QColor color);

private:
    Ui::RoomStyle *ui;
    bool data_loading = true;
};

#endif // ROOMSTYLE_H
