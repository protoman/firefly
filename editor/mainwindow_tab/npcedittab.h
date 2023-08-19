#ifndef NPCEDITTAB_H
#define NPCEDITTAB_H

#include <QDialog>

namespace Ui {
class NPCEditTab;
}

class NPCEditTab : public QDialog
{
    Q_OBJECT

public:
    explicit NPCEditTab(QWidget *parent = nullptr);
    ~NPCEditTab();
    void reload();

private:
    void fill_data();
    void set_data();

private slots:
    void on_pushButton_clicked();
    void on_npcSelectorComboBox_currentIndexChanged(int index);
    void on_nameLineEdit_textChanged(const QString &arg1);
    void on_graphicsComboBox_currentIndexChanged(const QString &arg1);
    void on_moveSpeedSpinBox_valueChanged(int arg1);
    void on_moveRangeSpinBox_valueChanged(int arg1);
    void on_animationFrameDurationSpinBox_valueChanged(int arg1);
    void on_frameWidthSpinBox_valueChanged(int arg1);
    void on_hitboxXSpinBox_valueChanged(int arg1);
    void on_hitboxYSpinBox_valueChanged(int arg1);
    void on_hitboxWSpinBox_valueChanged(int arg1);
    void on_hitboxHSpinBox_valueChanged(int arg1);
    void on_questSelectorComboBox_currentIndexChanged(int index);
    void on_moveBehaviorComboBox_currentIndexChanged(int index);
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_2_clicked();

private:
    Ui::NPCEditTab *ui;
    bool data_loading = true;

};



#endif // NPCEDITTAB_H
