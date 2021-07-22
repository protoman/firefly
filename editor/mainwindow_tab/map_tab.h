#ifndef MAP_TAB_H
#define MAP_TAB_H

#include <QWidget>

#include "file/file_io.h"

namespace Ui {
class map_tab;
}

class map_tab : public QWidget
{
    Q_OBJECT
    
public:
    explicit map_tab(QWidget *parent = 0);
    ~map_tab();
    void reload();
    void set_current_box(short n);
    void update_edit_area();

public slots:


private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_listWidget_2_currentRowChanged(int currentRow);
    void on_npc_listWidget_currentRowChanged(int currentRow);
    void on_npc_direction_combo_currentIndexChanged(int index);
    void on_objectListWidget_currentRowChanged(int currentRow);
    void on_object_direction_combo_currentIndexChanged(int index);
    void on_editTile_button_clicked();
    void on_editObject_button_clicked();
    void on_editLink_button_clicked();
    void on_editNpc_button_clicked();
    void on_editSetSubBoss_button_clicked();
    void on_editSetBoss_button_clicked();
    void on_editModeNormal_button_clicked();
    void on_editModeLock_button_clicked();
    void on_editModeErase_button_clicked();

    void on_addAnimTile_toolButton_clicked();

    void on_toolButton_clicked();

    void on_paste_toolButton_clicked();

    void fill_map_selector();

    void on_mapSelector_comboBox_currentIndexChanged(int index);

    void on_editModeSlope_Button_clicked();


private:
    void fill_data();
    
private:
    Ui::map_tab *ui;
    bool _data_loading;
    file_io fio;
    int propertiesH;
    bool properties_hidden = false;
};

#endif // MAP_TAB_H
