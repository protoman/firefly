#ifndef AREAEDITOR_H
#define AREAEDITOR_H

#include <QWidget>

namespace Ui {
class AreaEditor;
}

class LevelEditor : public QWidget
{
    Q_OBJECT

public:
    explicit LevelEditor(QWidget *parent = nullptr);
    ~LevelEditor();
    void reload();

private slots:
    void on_addAreaPushButton_clicked();

    void on_areaNameLineEdit_textChanged(const QString &arg1);

    void on_areaComboBox_currentIndexChanged(int index);

    void on_mapComboBox_currentIndexChanged(int index);

    void on_addVerticalLinkPushButton_clicked();

    void on_addHorizontalLinkPushButton_clicked();

private:
    Ui::AreaEditor *ui;
    bool data_loading = false;
};

#endif // AREAEDITOR_H
