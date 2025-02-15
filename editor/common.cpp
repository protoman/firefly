#include "common.h"

extern std::vector<std::string> PROJECTILE_TRAJECTORIES_NAMES;
extern std::vector<std::string> AI_ACTION_NAMES;
extern std::vector<std::string> AI_ACTION_WALK_OPTIONS;
extern std::vector<std::string> AI_ACTION_SHOT_OPTIONS;
extern std::vector<std::string> AI_ACTION_FLY_OPTIONS;
extern std::vector<std::string> AI_ACTION_JUMP_OPTIONS;
extern std::vector<std::string> AI_ACTION_WALL_WALK_OPTIONS;
extern std::vector<std::string> AI_ACTION_AIR_WALK_OPTIONS;
extern std::vector<std::string> AI_ACTION_TELEPORT_OPTIONS;
extern std::vector<std::string> AI_ACTION_DASH_OPTIONS;
extern std::vector<std::string> AI_ACTION_GRAB_WALL_OPTIONS;
extern std::vector<std::string> ANIMATION_TYPE_LIST;
extern std::vector<std::string> AI_ACTION_SFX_OPTION_NAMES;

#include <QString>
#include <QMessageBox>

#include "aux_tools/stringutils.h"
#include "mediator.h"

#include "aux_tools/ability_name.h"

common::common()
{
}

common::~common()
{
}


void common::fill_files_combo(std::string directory, QComboBox* combo, bool show_none)
{
    // ignore as game was not initialized yet. the form will need to call reload to work in this case
    if (SharedData::get_instance()->FILEPATH.length() == 0) {
        return;
    }

    combo->clear(); // delete all previous entries

    combo->addItem(QString("")); // for "empty"
    std::string str_filepath(SharedData::get_instance()->FILEPATH+directory);
    str_filepath = StringUtils::clean_filename(str_filepath);
    QString filepath(str_filepath.c_str());
    QDir dir = QDir(filepath);

    if (!dir.exists()) {
        show_directory_error_message(str_filepath);
    }

    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
	if (show_none == true) {
		combo->addItem(QString("None"));
	}

    foreach (const QFileInfo &fileInfo, dir.entryInfoList()) {
        if (fileInfo.fileName().length() > 30) {
            std::cout << "ERROR: file '" << fileInfo.fileName().toStdString() << "' surpasses the maximum number of file-characters (" << FS_CHAR_NAME_SIZE << ")" << std::endl;
        } else {
            QString filename(fileInfo.fileName());
            if (filename.length() > 0) {
                combo->addItem(filename);
            }
        }
    }
    combo->repaint();
}

void common::fill_graphicfiles_listwidget(std::string directory, QListWidget* listWidget)
{
    listWidget->clear();

    QListWidgetItem* item;

    std::string str_filepath(SharedData::get_instance()->FILEPATH+directory);
    str_filepath = StringUtils::clean_filename(str_filepath);
    QString filepath(str_filepath.c_str());
    QDir dir = QDir(filepath);
    if (!dir.exists()) {
        show_directory_error_message(str_filepath);
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    for (unsigned int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.fileName().length() > 30) {
            std::cout << "ERROR: file '" << fileInfo.fileName().toStdString() << "' surpasses the maximum number of file-characters (" << FS_CHAR_NAME_SIZE << ")" << std::endl;
        } else {
            item = new QListWidgetItem;
            item->setText(fileInfo.fileName());
            std::string filename = SharedData::get_instance()->FILEPATH + directory + "/" + fileInfo.fileName().toStdString();
            //std::cout << ">> MainWindow::fill_graphicfiles_listwidget DEBUG: filename: '" << filename << std::endl;
            QPixmap image(filename.c_str());
            if (image.isNull() == false && image.width() > 0) {
                image = image.copy(0, 0, image.width(), image.height());
                image = image.scaled(32, 32);
                item->setIcon(image);
            }
            listWidget->addItem(item);
        }
    }
    listWidget->repaint();
}

void common::fill_graphicfiles_combobox(std::string directory, QComboBox *comboWidget)
{
    comboWidget->clear(); // delete all previous entries

    std::string str_filepath(SharedData::get_instance()->FILEPATH+directory);
    str_filepath = StringUtils::clean_filename(str_filepath);
    QString filepath(str_filepath.c_str());
    QDir dir = QDir(filepath);
    if (!dir.exists()) {
        show_directory_error_message(str_filepath);
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    for (unsigned int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.fileName().length() > 30) {
            std::cout << "ERROR: file '" << fileInfo.fileName().toStdString() << "' surpasses the maximum number of file-characters (" << FS_CHAR_NAME_SIZE << ")" << std::endl;
        } else {
            std::string filename = SharedData::get_instance()->FILEPATH + directory + "/" + fileInfo.fileName().toStdString();
            QIcon icon(filename.c_str());
            comboWidget->addItem(icon, fileInfo.fileName());
        }
    }
    comboWidget->repaint();
}


void common::fill_enemies_combo(QComboBox* combo)
{
    combo->clear(); // delete all previous entries

    for (unsigned int i=0; i<GameData::get_instance()->enemy_list.size(); i++) {
        QString temp_str = QString("[");
        if (i < 10) {
            temp_str += "0";
        }
        temp_str += QString::number(i) + QString("] - ") + QString(GameData::get_instance()->enemy_list.at(i).name);
        combo->addItem(temp_str);
    }
}

void common::fill_object_combo(QComboBox* combo)
{
    combo->clear(); // delete all previous entries

    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->v6_object_list.at(i).name);
        combo->addItem(temp_str);
    }
}

void common::fill_object_combo(QComboBox *combo, OBJECT_TYPE type)
{
    combo->clear(); // delete all previous entries

    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        std::cout << "%%%%%%%%% obj[" << GameData::get_instance()->v6_object_list.at(i).name << "], type[" << GameData::get_instance()->v6_object_list.at(i).type << "], expected[" << (int)type << "]" << std::endl;
        if (GameData::get_instance()->v6_object_list.at(i).type == type) {
            QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->v6_object_list.at(i).name);
            combo->addItem(temp_str);
        }
    }
}

void common::fill_object_combo_with_none(QComboBox *combo)
{
    combo->clear(); // delete all previous entries
    combo->addItem(QString("[-1] None"));
    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->v6_object_list.at(i).name);
        combo->addItem(temp_str);
    }

}



void common::fill_projectiles_combo(QComboBox *combo, bool add_empty_slot)
{
    combo->clear(); // delete all previous entries

    if (add_empty_slot) {
        combo->addItem(QString(""));
    }
    for (unsigned int i=0; i<GameData::get_instance()->projectile_list_v0.size(); i++) {
        QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->projectile_list_v0.at(i).name.c_str());
        combo->addItem(temp_str);
    }
}


void common::fill_trajectories_combo(QComboBox *combo)
{
    combo->clear(); // delete all previous entries

    for (int i=0; i<PROJECTILE_TRAJECTORIES_COUNT; i++) {
		std::string temp = PROJECTILE_TRAJECTORIES_NAMES[i];
		QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(temp.c_str());
		combo->addItem(temp_str);
	}
}

void common::fill_ai_actions_combo(QComboBox *combo, bool include_null)
{
    combo->clear(); // delete all previous entries
    if (include_null == true) {
        QString temp_null_str = QString("None");
        combo->addItem(temp_null_str);
    }

    for (unsigned int i=0; i<AI_ACTION_NAMES.size(); i++) {
		std::string temp = AI_ACTION_NAMES[i];
		QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(temp.c_str());
		combo->addItem(temp_str);
	}
}


void common::fill_ai_options_combo(int action, QComboBox *combo)
{

    combo->clear(); // delete all previous entries

    //std::cout << "########## common::fill_ai_options_combo action[" << action << "]" << std::endl;

    std::vector<std::string> list;
	if (action == AI_ACTION_WALK) {
		list = AI_ACTION_WALK_OPTIONS;
	} else if (action == AI_ACTION_FLY) {
		list = AI_ACTION_FLY_OPTIONS;
    } else if (action == AI_ACTION_WALL_WALK || action == AI_ACTION_WALL_WALK_SHOOT) {
        list = AI_ACTION_WALL_WALK_OPTIONS;
    } else if (action == AI_ACTION_JUMP) {
		list = AI_ACTION_JUMP_OPTIONS;
	} else if (action == AI_ACTION_AIR_WALK) {
		list = AI_ACTION_AIR_WALK_OPTIONS;
	} else if (action == AI_ACTION_TELEPORT) {
		list = AI_ACTION_TELEPORT_OPTIONS;
	} else if (action == AI_ACTION_DASH) {
		list = AI_ACTION_DASH_OPTIONS;
	} else if (action == AI_ACTION_GRAB_WALL) {
		list = AI_ACTION_GRAB_WALL_OPTIONS;
    } else if (action == AI_ACTION_SPAWN_NPC || action == AI_ACTION_REPLACE_NPC || action == AI_ACTION_MORPH_INTO_NPC) {
        list = common::get_npc_names_list();
    } else if (action == AI_ACTION_SHOT_PROJECTILE_AHEAD || action == AI_ACTION_SHOT_PROJECTILE_PLAYER_DIRECTION || action == AI_ACTION_SHOT_PROJECTILE_INVERT_DIRECTION || action == AI_ACTION_JUMP_ATTACK_UP || action == AI_ACTION_JUMP_ATTACK_AHEAD_ONCE) {
        //list = AI_ACTION_SHOT_OPTIONS;
        list = common::get_weapon_names_list();
    } else if (action == AI_ACTION_CHANGE_MOVE_TYPE || action == AI_ACTION_CHANGE_MOVE_TYPE_REVERSE) {
        list = ANIMATION_TYPE_LIST;
    } else if (action == AI_ACTION_CIRCLE_PLAYER) {
        std::vector<std::string> dist_list;
        dist_list.push_back("1");
        dist_list.push_back("2");
        dist_list.push_back("3");
        dist_list.push_back("4");
        list = dist_list;
    } else if (action == AI_ACTION_WAIT_RANDOM_TIME) {
        std::vector<std::string> dist_list;
        dist_list.push_back("1");
        dist_list.push_back("2");
        dist_list.push_back("3");
        dist_list.push_back("4");
        dist_list.push_back("5");
        dist_list.push_back("10");
        dist_list.push_back("20");
        list = dist_list;
    } else if (action == AI_ACTION_PLAY_SFX) {
        list = AI_ACTION_SFX_OPTION_NAMES;
    } else if (action == AI_ACTION_ROTATE_GRAPHIC) {
        std::vector<std::string> dist_list;
        dist_list.push_back("RESTORE ORIGINAL");
        dist_list.push_back("PLUS 10");
        dist_list.push_back("PLUS 30");
        dist_list.push_back("PLUS 45");
        dist_list.push_back("PLUS 90");
        dist_list.push_back("PLUS 360");
        dist_list.push_back("MINUS 10");
        dist_list.push_back("MINUS 30");
        dist_list.push_back("MINUS 45");
        dist_list.push_back("MINUS 90");
        dist_list.push_back("MINUS 360");
        list = dist_list;
    } else {
        return;
    }

	// add options
    combo->clear(); // delete all previous entries
    for (unsigned int i=0; i<list.size(); i++) {
		std::string temp = list[i];
		QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(temp.c_str());
		combo->addItem(temp_str);
	}
}

void common::fill_ai_list(QComboBox *combo)
{
    combo->clear(); // delete all previous entries
    for (unsigned int i=0; i<GameData::get_instance()->ai_list.size(); i++) {
        QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->ai_list.at(i).name);
		combo->addItem(temp_str);
    }
}

void common::fill_players_combo(QComboBox* combo)
{
    combo->clear(); // delete all previous entries
    for (unsigned int i=0; i<FS_MAX_PLAYERS; i++) {
        combo->addItem(QString::number(i+1)+QString(" [")+QString(Mediator::get_instance()->player_list_v3_1[i].name)+QString("]"));
    }
}


void common::fill_map_list_combo(QComboBox *combo)
{
    combo->clear(); // delete all previous entries
    for (unsigned int i=0; i<3; i++) {
        char buffer[512];
        Mediator::get_instance()->centNumberFormat(i);
        sprintf(buffer, "%s", Mediator::get_instance()->centNumber);
        combo->addItem(buffer);
    }
}


void common::fill_npc_listwidget(QListWidget *listWidget)
{
    QListWidgetItem* item;

    listWidget->clear();

    for (unsigned int i=0; i<GameData::get_instance()->enemy_list.size(); i++) {
        item = new QListWidgetItem;
        QString temp_str = QString("[");
        if (i < 10) {
            temp_str += "0";
        }
        temp_str += QString::number(i) + QString("] - ") + QString(GameData::get_instance()->enemy_list.at(i).name);
        item->setText(temp_str);
        std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/" + GameData::get_instance()->enemy_list.at(i).graphic_filename;
        QPixmap image(filename.c_str());
        image = image.copy(0, 0, GameData::get_instance()->enemy_list.at(i).frame_size.width, GameData::get_instance()->enemy_list.at(i).frame_size.height);
        if (image.isNull() == false && image.width() > 0) {
            image = image.scaled(32, 32);
        }
        item->setIcon(image);
        listWidget->addItem(item);
    }

    listWidget->update();
}

void common::fill_object_listWidget(QListWidget *listWidget)
{
    QListWidgetItem* item;

    listWidget->clear();

    for (unsigned int i=0; i<GameData::get_instance()->v6_object_list.size(); i++) {
        item = new QListWidgetItem;
        QString temp_str = QString("[");
        if (i < 10) {
            temp_str += "0";
        }

        temp_str += QString::number(i) + QString("] - ") + QString(GameData::get_instance()->v6_object_list.at(i).name);
        item->setText(temp_str);
        std::string filename = SharedData::get_instance()->FILEPATH + "/images/sprites/objects/" + GameData::get_instance()->v6_object_list.at(i).graphic_filename;
        QPixmap image(filename.c_str());
        image = image.copy(0, 0, GameData::get_instance()->v6_object_list.at(i).size.width, GameData::get_instance()->v6_object_list.at(i).size.height);
        if (image.isNull() == false && image.width() > 0) {
            image = image.scaled(32, 32);
        }
        item->setIcon(image);
        listWidget->addItem(item);
    }


    listWidget->update();
}

void common::fill_scenes_combo(QComboBox *combo)
{
    combo->clear();
    // add empty item to be used to unset scene
    combo->addItem(QString(""));
    for (unsigned int i=0; i<Mediator::get_instance()->scene_list.size(); i++) {
        QString temp_str = QString("[") + QString::number(i) + QString("] - ") + QString(Mediator::get_instance()->scene_list.at(i).name);
        combo->addItem(temp_str);
    }
}

void common::fill_languages_combo(QComboBox *combo)
{
    combo->clear();
    // add empty item to be used to unset scene
    combo->addItem(QString("English"));
    combo->addItem(QString("French"));
    combo->addItem(QString("Spanish"));
    combo->addItem(QString("Italian"));
    combo->addItem(QString("Portuguese"));
}

void common::fill_numbered_combo(QComboBox *combo, int start, int end)
{
    combo->clear();
    for (int i=start; i<=end; i++) {
        combo->addItem(QString::number(i));
    }
}

std::vector<std::string> common::get_npc_names_list()
{
    std::vector<std::string> res;
    for (unsigned int i=0; i<GameData::get_instance()->enemy_list.size(); i++) {
        res.push_back(std::string(GameData::get_instance()->enemy_list.at(i).name));
    }
    return res;
}

std::vector<std::string> common::get_weapon_names_list()
{
    std::vector<std::string> res;
    for (unsigned int i=0; i<GameData::get_instance()->projectile_list_v0.size(); i++) {
        res.push_back(std::string(GameData::get_instance()->projectile_list_v0.at(i).name.c_str()));
    }
    return res;
}

st_size common::calc_image_size(std::string file)
{
    std::string filename = SharedData::get_instance()->FILEPATH + std::string("/images/scenes/animations/") + file;
    QPixmap image(filename.c_str());

    if (image.isNull() == true || image.width() <= 0) {
        return st_size(0, 0);
    }
    return st_size(image.width(), image.height());
}

void common::fill_gfx_effect(QComboBox *combo)
{
    combo->clear();
    combo->addItem("None");
    combo->addItem("Rain");
    combo->addItem("Snow");
    combo->addItem("Underwater");
    combo->addItem("Train");
    combo->addItem("Flash Stopper");
    combo->addItem("Lighting Bolt");
    combo->addItem("Shadow Top");
    combo->addItem("Inferno");
}

void common::fill_gfx_mode(QComboBox *combo)
{
    combo->clear();
    combo->addItem("Full");
    combo->addItem("Background");
    combo->addItem("Overlay");
}

void common::fill_layer_scroll_mode(QComboBox *combo)
{
    combo->clear();
    combo->addItem("None");
    combo->addItem("Up");
    combo->addItem("Down");
    combo->addItem("Left");
    combo->addItem("Right");
    combo->addItem("Diagonal [TBD]");
}

void common::fill_anim_block_combo(QComboBox *combo)
{
    combo->clear();
    for (int i=0; i<Mediator::get_instance()->anim_block_list.size(); i++) {
        QString name = QString::number(i);
        combo->addItem(name);
    }
}

void common::fill_abilities_combo(QComboBox *combo)
{
    combo->clear();
    combo->addItem(QString("None"));
    for (unsigned int i=0; i<PROPERTY_NAME_COUNT; i++) {
        combo->addItem(QString(AbilityName::get_instance()->get_name_from_number(i).c_str()));
    }
}

void common::show_directory_error_message(std::string directory)
{
    QString error_msg = QString("WARNING: Directory '") + QString(directory.c_str()) + QString("' does not exist and will be created empty.");
    QMessageBox msgBox;
    msgBox.setText(error_msg);
    msgBox.exec();
    // create directory now
    QDir().mkdir(directory.c_str());
}

void common::fill_direction_combo(QComboBox *combo)
{
    combo->clear();
    combo->addItem(QString("LEFT"));
    combo->addItem(QString("RIGHT"));
    combo->addItem(QString("UP"));
    combo->addItem(QString("DOWN"));
}

void common::fill_room_styles_combo(QComboBox *combo)
{
    combo->clear();
    for (unsigned int i=0; i<GameData::get_instance()->v6_style_list.size(); i++) {
        QString name = QString("[") + QString::number(i) + QString("] - ") + QString(GameData::get_instance()->v6_style_list.at(i).style_name);
        combo->addItem(name);
    }
}

void common::fill_npcs_combo(QComboBox *combo)
{
    combo->clear();
    for (unsigned int i=0; i<GameData::get_instance()->npc_list.size(); i++) {
        QString temp_str = QString("[");
        if (i < 10) {
            temp_str += "0";
        }
        temp_str += QString::number(i) + QString("] - ") + QString(GameData::get_instance()->npc_list.at(i).name);
        combo->addItem(temp_str);
    }
}





