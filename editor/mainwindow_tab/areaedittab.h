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
    void reload();

private slots:
    void on_currentAreaComboBox_currentIndexChanged(int index);

    void on_areaNameLineEdit_textEdited(const QString &arg1);

    void on_addAreaPushButton_clicked();

    void on_tilesetComboBox_currentIndexChanged(const QString &arg1);

    void on_gfxEffectComboBox_currentIndexChanged(int index);

    void on_gfxModeComboBox_currentIndexChanged(int index);

    void on_layerSelectorComboBox_currentIndexChanged(int index);

    void on_layerImageComboBox_currentIndexChanged(const QString &arg1);

    bool checkLoadingAndValid();

    void on_layerSpeedDoubleSpinBox_valueChanged(double arg1);

    void on_layerVerticalPositionSpinBox_valueChanged(int arg1);

    void on_layerAutoscrollComboBox_currentIndexChanged(int index);

    void on_layerAlphaSpinBox_valueChanged(int arg1);

    void on_layerAnimationTimerSpinBox_valueChanged(int arg1);

    void on_layerAnimationWidthSpinBox_valueChanged(int arg1);

    void on_layerRepeatXCheckBox_toggled(bool checked);

    void on_layerRepeatYCheckBox_toggled(bool checked);

    void on_backgroundColorPushButton_clicked();

    void on_musicFilenameComboBox_currentIndexChanged(const QString &arg1);

private:
    void fill_data();
    void set_data();

public slots:
    void on_color_selected1(QColor color);


private:
    Ui::AreaEditTab *ui;
    bool is_loading = false;
};

#endif // AREAEDITTAB_H
