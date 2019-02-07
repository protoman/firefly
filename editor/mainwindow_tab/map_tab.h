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
    void on_color_selected1(QColor);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_listWidget_2_currentRowChanged(int currentRow);
    void on_npc_listWidget_currentRowChanged(int currentRow);
    void on_npc_direction_combo_currentIndexChanged(int index);
    void on_objectListWidget_currentRowChanged(int currentRow);
    void on_bg_color_pick_clicked();
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);
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

    void on_difficulty_comboBox_currentIndexChanged(int index);


    void on_difficultyMode_pushButton_clicked();

    void on_addMap_pushButton_clicked();
    void fill_map_selector();
    void fill_map_v5_data();

    void on_mapSelector_comboBox_currentIndexChanged(int index);

    void on_v5_mapName_lineEdit_textChanged(const QString &arg1);

    void on_mapSizeW_spinBox_valueChanged(int arg1);

    void on_mapSizeH_spinBox_valueChanged(int arg1);

    void on_generateMapTiles_pushButton_clicked();

    void generateMapTiles();

    void on_v5_tileset_comboBox_currentIndexChanged(const QString &arg1);

    void on_layerSelector_comboBox_currentIndexChanged(int index);

    void set_layer_data();

    void on_bg1_filename_currentIndexChanged(const QString &arg1);

    void on_bg1_speed_valueChanged(double arg1);

    void on_bg1_y_pos_valueChanged(int arg1);

    void on_autoScrollBG1_mode_currentIndexChanged(int index);

    void on_repeatX_checkBox_toggled(bool checked);

    void on_repeatY_checkBox_toggled(bool checked);

    void on_layerAlpha_spinBox_valueChanged(int arg1);

    void on_editModeSlope_Button_clicked();

private:
    void fill_data();
    void fill_background_list();
    void fill_anim_tiles_data();
    
private:
    Ui::map_tab *ui;
    bool _data_loading;
    file_io fio;
};

#endif // MAP_TAB_H
