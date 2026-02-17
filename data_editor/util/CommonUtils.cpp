//
// Created by iuri on 16/02/2026.
//

#include "CommonUtils.hpp"

#include <qdir.h>
#include <QMessageBox>

CommonUtils* CommonUtils::_instance = nullptr;

CommonUtils::CommonUtils() = default;

CommonUtils *CommonUtils::get_instance()
{
    if (!_instance) {
        _instance = new CommonUtils();
    }
    return _instance;
}

void CommonUtils::fill_files_combo(std::string directory, QComboBox *combo, bool show_none) {
    combo->clear(); // delete all previous entries

    combo->addItem(QString("")); // for "empty"
    QString filepath(directory.c_str());
    QDir dir = QDir(filepath);

    if (!dir.exists()) {
        show_directory_error_message(directory);
    }

    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    if (show_none == true) {
        combo->addItem(QString("None"));
    }

    foreach (const QFileInfo &fileInfo, dir.entryInfoList()) {
        QString filename(fileInfo.fileName());
        if (filename.length() > 0) {
            combo->addItem(filename);
        }
    }
    combo->repaint();
}

void CommonUtils::show_directory_error_message(std::string directory) {
    QString error_msg = QString("WARNING: Directory '") + QString(directory.c_str()) + QString("' does not exist and will be created empty.");
    QMessageBox msgBox;
    msgBox.setText(error_msg);
    msgBox.exec();
    // create directory now
    QDir().mkdir(directory.c_str());
}

