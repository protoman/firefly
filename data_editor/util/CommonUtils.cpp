//
// Created by iuri on 16/02/2026.
//

#include "CommonUtils.hpp"

#include <qdir.h>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileInfo>

CommonUtils* CommonUtils::_instance = nullptr;

CommonUtils::CommonUtils() {
    // Determine the path of the executable
    QString appPath = QCoreApplication::applicationDirPath();

    // The editor binary is in Firefly/data_editor/cmake-build-debug/... (or similar)
    // We want to climb up until we reach 'Firefly'
    QDir dir(appPath);
    while (dir.dirName() != "firefly" && !dir.isRoot()) {
        dir.cdUp();
    }

    m_parent_project_path = dir.absolutePath().toStdString();
    m_objects_sprites_path = m_parent_project_path + "/game_data/images/sprites/objects";
}

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

void CommonUtils::set_parent_project_path(const std::string& path) {
    m_parent_project_path = path;
    m_objects_sprites_path = m_parent_project_path + "/game_data/images/sprites/objects";
}

std::string CommonUtils::get_parent_project_path() const {
    return m_parent_project_path;
}

std::string CommonUtils::get_objects_sprites_path() const {
    return m_objects_sprites_path;
}
