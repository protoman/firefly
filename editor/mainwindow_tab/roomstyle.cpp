#include "roomstyle.h"

#include <QColorDialog>

#include "ui_roomstyle.h"
#include "common.h"
#include "game_data.h"

RoomStyle::RoomStyle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomStyle)
{
    ui->setupUi(this);
    fill_data();
}

RoomStyle::~RoomStyle()
{
    delete ui;
}

void RoomStyle::fill_data()
{
    if (GameData::get_instance()->v6_style_list.size() < 1) {
        return;
    }
    data_loading = true;
    common::fill_room_styles_combo(ui->styleSelectComboBox);
    common::fill_files_combo("/images/tilesets", ui->styleTilesetComboBox);
    common::fill_files_combo("/music", ui->styleMusicFilenameComboBox);
    common::fill_gfx_effect(ui->styleGfxEffectComboBox);
    common::fill_gfx_mode(ui->styleGfxModeComboBox);
    ui->layerSelectorComboBox->clear();
    for (unsigned int i=0; i<LAYERS_COUNT; i++) {
        if (i < LAYERS_BG_COUNT) {
            ui->layerSelectorComboBox->addItem(QString("BACKGROUND [") + QString::number(i) + "]");
        } else {
            ui->layerSelectorComboBox->addItem(QString("FOREGROUND [") + QString::number(i-LAYERS_BG_COUNT) + "]");
        }
    }
    common::fill_files_combo("/images/map_backgrounds", ui->layerImageComboBox);
    common::fill_layer_scroll_mode(ui->layerAutoscrollComboBox);

    set_data();

    data_loading = false;
}

void RoomStyle::set_data()
{
    ui->styleNameLineEdit->setText(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).style_name);
    ui->styleTilesetComboBox->setCurrentIndex(ui->styleTilesetComboBox->findText(QString(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).tileset_filename)));
    st_color bg_color = GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color;
    QString color_css = QString("background-color: rgb(") + QString::number(bg_color.r) + QString(", ") + QString::number(bg_color.g) + QString(", ") + QString::number(bg_color.b) + QString(")");
    ui->StyleBackgroundColorPushButton->setStyleSheet(color_css);
    ui->styleMusicFilenameComboBox->setCurrentIndex(ui->styleMusicFilenameComboBox->findText(QString(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).music_filename)));
    ui->styleGfxEffectComboBox->setCurrentIndex(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).gfx_effect);
    ui->styleGfxModeComboBox->setCurrentIndex(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).gfx_mode);
    SharedData::get_instance()->room_style_selected_layer = 0;
    ui->layerSelectorComboBox->setCurrentIndex(SharedData::get_instance()->room_style_selected_layer);

    set_layer_data();
}

void RoomStyle::set_layer_data()
{
    std::cout << "#RoomStyle::set_layer_data - style[" << SharedData::get_instance()->room_style_selected_style << "], layer[" << SharedData::get_instance()->room_style_selected_layer << "], speed[" << GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].speed << "]" << std::endl;
    file_v6_style_layer debug = GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer];
    ui->layerSpeedDoubleSpinBox->setValue((float)GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].speed/(float)10);
    ui->layerVerticalPositionSpinBox->setValue(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].adjust_y);
    ui->layerAutoscrollComboBox->setCurrentIndex(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].auto_scroll);
    ui->layerAlphaSpinBox->setValue(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].alpha);
    ui->layerAnimationTimerSpinBox->setValue(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].animation_timer);
    ui->layerAnimationWidthSpinBox->setValue(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].animation_width);
    ui->layerRepeatXCheckBox->setChecked(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].repeatX);
    ui->layerRepeatYCheckBox->setChecked(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].repeatY);
    ui->layerImageComboBox->setCurrentIndex(ui->layerImageComboBox->findText(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].filename));

    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}

void RoomStyle::reload()
{
    fill_data();
}

void RoomStyle::on_pushButton_clicked()
{
    GameData::get_instance()->v6_style_list.push_back(file_v6_style());
    data_loading = true;
    common::fill_room_styles_combo(ui->styleSelectComboBox);
    data_loading = false;
}


/*
    ui->tilesetComboBox->setCurrentIndex(ui->tilesetComboBox->findText(QString(SharedData::get_instance()->v6_area_list.at(currentMap).tileset_filename)));


    ui->layerSpeedDoubleSpinBox->setValue((float)SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].speed/(float)10);
    //std::cout << ">>>>>>>>>>>>>>>>> speed on area[" << currentMap << "], layer[" << currentLayer << "] set to [" << (float)SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].speed/(float)10 << "]" << std::endl;
    ui->layerVerticalPositionSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].adjust_y);
    ui->layerAutoscrollComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].auto_scroll);
    ui->layerAlphaSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].alpha);
    ui->layerAnimationTimerSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].animation_timer);
    ui->layerAnimationWidthSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].animation_width);
    ui->layerRepeatXCheckBox->setChecked(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].repeatX);
    ui->layerRepeatYCheckBox->setChecked(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].repeatY);

    ui->gfxEffectComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).gfx_effect);
    ui->gfxModeComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).gfx_mode);

    st_color bg_color = SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color;
    QString color_css = QString("background-color: rgb(") + QString::number(bg_color.r) + QString(", ") + QString::number(bg_color.g) + QString(", ") + QString::number(bg_color.b) + QString(")");
    //std::cout << "color_css[" << color_css.toStdString() << "]" << std::endl;
    ui->backgroundColorWidget->setStyleSheet(color_css);

    int currentLayer = ui->layerSelectorComboBox->currentIndex();
    ui->layerImageComboBox->setCurrentIndex(ui->layerImageComboBox->findText(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].filename));




// fill-data
    common::fill_files_combo("/images/tilesets", ui->tilesetComboBox);
    common::fill_gfx_effect(ui->gfxEffectComboBox);
    common::fill_gfx_mode(ui->gfxModeComboBox);
    // LAYERS-FIELDS //
    ui->layerSelectorComboBox->clear();
    for (unsigned int i=0; i<LAYERS_COUNT; i++) {
        if (i < LAYERS_BG_COUNT) {
            ui->layerSelectorComboBox->addItem(QString("BACKGROUND [") + QString::number(i) + "]");
        } else {
            ui->layerSelectorComboBox->addItem(QString("FOREGROUND [") + QString::number(i-LAYERS_BG_COUNT) + "]");
        }
    }
    common::fill_files_combo("/images/map_backgrounds", ui->layerImageComboBox);
    common::fill_layer_scroll_mode(ui->layerAutoscrollComboBox);





void AreaEditTab::on_tilesetComboBox_currentIndexChanged(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).tileset_filename, "%s", arg1.toStdString().c_str());
}

void AreaEditTab::on_gfxEffectComboBox_currentIndexChanged(int index)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).gfx_effect = index;
}

void AreaEditTab::on_gfxModeComboBox_currentIndexChanged(int index)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).gfx_mode = index;
}



void AreaEditTab::on_layerImageComboBox_currentIndexChanged(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].filename, "%s", arg1.toStdString().c_str());
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}


void AreaEditTab::on_layerSpeedDoubleSpinBox_valueChanged(double arg1)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].speed = arg1*10;
    //std::cout << ">>>>>>>>>>>>>>>>> speed on area[" << ui->currentAreaComboBox->currentIndex() << "], layer[" << ui->layerSelectorComboBox->currentIndex() << "] set to [" << SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].speed << "]" << std::endl;
}

void AreaEditTab::on_layerVerticalPositionSpinBox_valueChanged(int arg1)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].adjust_y = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_layerAutoscrollComboBox_currentIndexChanged(int index)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].auto_scroll = index;
}

void AreaEditTab::on_layerAlphaSpinBox_valueChanged(int arg1)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].alpha = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_layerAnimationTimerSpinBox_valueChanged(int arg1)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].animation_timer = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_layerAnimationWidthSpinBox_valueChanged(int arg1)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].animation_width = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_layerRepeatXCheckBox_toggled(bool checked)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].repeatX = checked;
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_layerRepeatYCheckBox_toggled(bool checked)
{
    if (checkLoadingAndValid()) { return; }
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).layers[ui->layerSelectorComboBox->currentIndex()].repeatY = checked;
    ui->areaEditPreviewWidgetObj->repaint();
}


void AreaEditTab::on_color_selected1(QColor color)
{
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.r = color.red();
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.g = color.green();
    SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.b = color.blue();

    st_color bg_color = SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color;
    QString color_css = QString("background-color: rgb(") + QString::number(bg_color.r) + QString(", ") + QString::number(bg_color.g) + QString(", ") + QString::number(bg_color.b) + QString(")");
    ui->backgroundColorWidget->setStyleSheet(color_css);
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::on_backgroundColorPushButton_clicked()
{
    QColor currentColor = QColor(SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.r, SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.g, SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color.b, 255);
    QColorDialog *colorDialog = new QColorDialog(currentColor, this);
    colorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
    QObject::connect(colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(on_color_selected1(QColor)));
    colorDialog->show();
}

void AreaEditTab::on_musicFilenameComboBox_currentIndexChanged(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).music_filename, "%s", arg1.toStdString().c_str());
}



*/

void RoomStyle::on_styleSelectComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->room_style_selected_style = index;
    data_loading = true;
    set_data();
    data_loading = false;
}

void RoomStyle::on_color_selected1(QColor color)
{
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.r = color.red();
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.g = color.green();
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.b = color.blue();

    st_color bg_color = GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color;
    QString color_css = QString("background-color: rgb(") + QString::number(bg_color.r) + QString(", ") + QString::number(bg_color.g) + QString(", ") + QString::number(bg_color.b) + QString(")");
    ui->StyleBackgroundColorPushButton->setStyleSheet(color_css);
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_styleNameLineEdit_textChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).style_name, "%s", arg1.toStdString().c_str());
}


void RoomStyle::on_styleTilesetComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).tileset_filename, "%s", arg1.toStdString().c_str());
}


void RoomStyle::on_StyleBackgroundColorPushButton_clicked()
{
    QColor currentColor = QColor(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.r, GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.g, GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).background_color.b, 255);
    QColorDialog *colorDialog = new QColorDialog(currentColor, this);
    colorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
    QObject::connect(colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(on_color_selected1(QColor)));
    colorDialog->show();
}

void RoomStyle::on_styleMusicFilenameComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).music_filename, "%s", arg1.toStdString().c_str());
}


void RoomStyle::on_styleGfxEffectComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).gfx_effect = index;
}


void RoomStyle::on_styleGfxModeComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).gfx_mode = index;
}


void RoomStyle::on_layerSelectorComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    SharedData::get_instance()->room_style_selected_layer = index;
    data_loading = true;
    set_layer_data();
    data_loading = false;
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerImageComboBox_currentIndexChanged(const QString &arg1)
{
    if (data_loading) return;
    sprintf(GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].filename, "%s", arg1.toStdString().c_str());
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerVerticalPositionSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].adjust_y = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerSpeedDoubleSpinBox_valueChanged(double arg1)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].speed = arg1*10;
    std::cout << "#RoomStyle::on_layerSpeedDoubleSpinBox_valueChanged - style[" << SharedData::get_instance()->room_style_selected_style << "], layer[" << SharedData::get_instance()->room_style_selected_layer << "], speed[" << GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].speed << "]" << std::endl;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerAutoscrollComboBox_currentIndexChanged(int index)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].auto_scroll = index;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerAnimationTimerSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].animation_timer = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerAnimationWidthSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].animation_width = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerRepeatXCheckBox_toggled(bool checked)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].repeatX = checked;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerRepeatYCheckBox_toggled(bool checked)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].repeatY = checked;
    ui->areaEditPreviewWidgetObj->repaint();
}


void RoomStyle::on_layerAlphaSpinBox_valueChanged(int arg1)
{
    if (data_loading) return;
    GameData::get_instance()->v6_style_list.at(SharedData::get_instance()->room_style_selected_style).layers[SharedData::get_instance()->room_style_selected_layer].alpha = arg1;
    ui->areaEditPreviewWidgetObj->repaint();
}

