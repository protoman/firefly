#include "areaedittab.h"
#include "ui_areaedittab.h"

#include "mediator.h"
#include "common.h"


#include <QColorDialog>

AreaEditTab::AreaEditTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditTab)
{
    ui->setupUi(this);
    fill_data();
    set_data();
}

AreaEditTab::~AreaEditTab()
{
    delete ui;
}

void AreaEditTab::on_currentAreaComboBox_currentIndexChanged(int index)
{
    if (index == -1) {
        return;
    }
    SharedData::get_instance()->v6_selected_area = index;
    Mediator::get_instance()->load_area_rooms(SharedData::get_instance()->v6_selected_area);
    set_data();
    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();
}

void AreaEditTab::reload()
{
    fill_data();
}

void AreaEditTab::fill_data()
{
    is_loading = true;

    // MAP-FIELDS //
    ui->currentAreaComboBox->clear();
    int map_list_size = SharedData::get_instance()->v6_area_list.size();
    for (unsigned int i=0; i<SharedData::get_instance()->v6_area_list.size(); i++) {
        ui->currentAreaComboBox->addItem(QString("[") + QString::number(i) + "] - " + QString(SharedData::get_instance()->v6_area_list.at(i).map_name));
    }
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

    common::fill_files_combo("/music", ui->musicFilenameComboBox);

    set_data();
    is_loading = false;
}

void AreaEditTab::set_data()
{
    // MAP-DATA //
    unsigned int currentMap = ui->currentAreaComboBox->currentIndex();
    if (currentMap >= SharedData::get_instance()->v6_area_list.size()) {
        return;
    }
    bool must_reset_is_loading = false;
    if (is_loading == false) {
        is_loading = true;
        must_reset_is_loading = true;
    }

    ui->areaNameLineEdit->setText(SharedData::get_instance()->v6_area_list.at(currentMap).map_name);
    ui->tilesetComboBox->setCurrentIndex(ui->tilesetComboBox->findText(QString(SharedData::get_instance()->v6_area_list.at(currentMap).tileset_filename)));
    ui->gfxEffectComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).gfx_effect);
    ui->gfxModeComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).gfx_mode);

    st_color bg_color = SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).background_color;
    QString color_css = QString("background-color: rgb(") + QString::number(bg_color.r) + QString(", ") + QString::number(bg_color.g) + QString(", ") + QString::number(bg_color.b) + QString(")");
    //std::cout << "color_css[" << color_css.toStdString() << "]" << std::endl;
    ui->backgroundColorWidget->setStyleSheet(color_css);


    // LAYERS-DATA //
    int currentLayer = ui->layerSelectorComboBox->currentIndex();
    ui->layerImageComboBox->setCurrentIndex(ui->layerImageComboBox->findText(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].filename));
    ui->layerSpeedDoubleSpinBox->setValue((float)SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].speed/(float)10);
    //std::cout << ">>>>>>>>>>>>>>>>> speed on area[" << currentMap << "], layer[" << currentLayer << "] set to [" << (float)SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].speed/(float)10 << "]" << std::endl;
    ui->layerVerticalPositionSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].adjust_y);
    ui->layerAutoscrollComboBox->setCurrentIndex(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].auto_scroll);
    ui->layerAlphaSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].alpha);
    ui->layerAnimationTimerSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].animation_timer);
    ui->layerAnimationWidthSpinBox->setValue(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].animation_width);
    ui->layerRepeatXCheckBox->setChecked(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].repeatX);
    ui->layerRepeatYCheckBox->setChecked(SharedData::get_instance()->v6_area_list.at(currentMap).layers[currentLayer].repeatY);

    ui->areaEditPreviewWidgetObj->update_files();
    ui->areaEditPreviewWidgetObj->repaint();

    ui->musicFilenameComboBox->setCurrentIndex(ui->musicFilenameComboBox->findText(SharedData::get_instance()->v6_area_list.at(currentMap).music_filename));

    if (must_reset_is_loading) {
        is_loading = false;
    }

}


bool AreaEditTab::checkLoadingAndValid()
{
    if (is_loading) { return true; }
    int currentMap = ui->currentAreaComboBox->currentIndex();
    if (currentMap >= SharedData::get_instance()->v6_area_list.size()) {
        return true;
    }
    return false;
}


void AreaEditTab::on_areaNameLineEdit_textEdited(const QString &arg1)
{
    if (checkLoadingAndValid()) { return; }
    sprintf(SharedData::get_instance()->v6_area_list.at(ui->currentAreaComboBox->currentIndex()).map_name, "%s", arg1.toStdString().c_str());
    fill_data();
    ui->currentAreaComboBox->setCurrentIndex(ui->currentAreaComboBox->currentIndex());
}

void AreaEditTab::on_addAreaPushButton_clicked()
{
    SharedData::get_instance()->v6_area_list.push_back(file_v6_area());
    int list_size = SharedData::get_instance()->v6_area_list.size();
    sprintf(SharedData::get_instance()->v6_area_list.at(list_size-1).map_name, "AREA [%d]", list_size);
    ui->currentAreaComboBox->addItem(QString("[") + QString::number(list_size) + "] - " + QString(SharedData::get_instance()->v6_area_list.at(list_size-1).map_name));
}


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

void AreaEditTab::on_layerSelectorComboBox_currentIndexChanged(int index)
{
    set_data();
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
