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
    void on_areaComboBox_currentIndexChanged(int index);

signals:


private:
    Ui::AreaEditor *ui;
    bool data_loading = false;
};

#endif // AREAEDITOR_H
