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
    void on_currentStageComboBox_currentIndexChanged(int index);
    void on_stageNameLineEdit_textEdited(const QString &arg1);
    void on_addAreaPushButton_clicked();
    bool checkLoadingAndValid();
    void on_musicFilenameComboBox_currentIndexChanged(const QString &arg1);
    void on_styleComboBox_currentIndexChanged(int index);
    void on_areaSelectionComboBox_currentIndexChanged(int index);
    void on_addAreaPushButton_2_clicked();

private:
    void fill_data();
    void set_data();
    void fill_area_combo();
    int get_area_style_number();

public slots:


private:
    Ui::AreaEditTab *ui;
    bool is_loading = false;
};

#endif // AREAEDITTAB_H
