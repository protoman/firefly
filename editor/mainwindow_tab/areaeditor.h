#ifndef AREAEDITOR_H
#define AREAEDITOR_H

#include <QWidget>

namespace Ui {
class AreaEditor;
}

class AreaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AreaEditor(QWidget *parent = nullptr);
    ~AreaEditor();
    void reload();
    void reload_map_combo();

private slots:
    void on_addAreaPushButton_clicked();

    void on_addMapPushButton_clicked();

    void on_areaNameLineEdit_textChanged(const QString &arg1);

    void on_areaComboBox_currentIndexChanged(int index);

    void on_generateTilesetPushButton_clicked();

private:
    Ui::AreaEditor *ui;
    bool data_loading = false;
};

#endif // AREAEDITOR_H
