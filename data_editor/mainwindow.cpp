#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QActionGroup>

#include <filesystem>
#include <regex>
#include <iostream>
#include <functional>
#include <QDockWidget>
#include <QTextEdit>

#include "file/fio_common.h"
#include "file/v4/file_game_v4.h"
#include "file/v5/struct_file_map.h"
#include "file/v6/file_object_v6.h"
#include "file/v6/file_room_v6.h"
#include "file/v6/file_level_v6.h"
#include "file/v4/file_anim_block.h"
#include "file/v6/file_area_v6.h"
#include "file/v6/file_style_v6.h"
#include "file/v6/file_npc_state_v6.h"
#include "file/v6/file_game_object_state.h"
#include "file/fio_scenes.h"


using namespace std::filesystem;

static void try_convert_dat(const std::string &path, fio_common &fio_cmm, const std::function<void(const std::string&)> &logger) {
    std::string fname = path;
    std::string lower = fname;
    for (auto &c : lower) c = std::tolower((unsigned char)c);
    std::string fname_only = std::filesystem::path(path).filename().string();

    // helper to save only if T is JSON-serializable
    auto maybe_save_json = [&](auto &&vec, const std::string &out) {
        using T = std::decay_t<decltype(vec)>::value_type;
        if (vec.empty()) return;
        if constexpr (cereal::traits::is_output_serializable<T, cereal::JSONOutputArchive>::value) {
            fio_cmm.save_json_data<T>(out, vec);
            logger(std::string("Converted ") + path + " -> " + out);
        } else {
            // skip saving for non-serializable types
            logger(std::string("Skipping conversion (no cereal JSON support) for ") + path);
        }
    };

    try {
        // Text-based string/dialog files: convert lines -> json array
        if (lower.find("/shared/strings_ingame_") != std::string::npos || fname_only.rfind("stage_dialogs_",0) == 0 || lower.find("/dialogs/") != std::string::npos) {
            std::vector<std::string> lines;
            std::ifstream is(path);
            if (is.is_open()) {
                std::string line;
                while (std::getline(is, line)) {
                    // trim CR
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    lines.push_back(line);
                }
                is.close();
                std::string out = path.substr(0, path.size()-4) + ".json";
                // save as JSON array of strings
                fio_cmm.save_json_data<std::string>(out, lines);
                logger(std::string("Converted text file ") + path + " -> " + out);
            } else {
                logger(std::string("Could not open text file for conversion: ") + path);
            }
            return;
        }

        // object state files (level_x_object_state.dat etc.)
        if (lower.find("object_state") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<v6_file_game_object_state>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        if (lower.find("game_enemy_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_enemy_v3_1_2>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_npc_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_npc_v3_1_2>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_ai_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_artificial_intelligence>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_object_list_v6") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<v6_file_object>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        // anim blocks
        if (lower.find("anim_block_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_anim_block>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        // per-map enemies/npcs objects patterns
        std::regex re_v5_enemies("v5_map_\\d+_enemies\\.dat");
        std::regex re_v5_npcs("v5_map_\\d+_npcs\\.dat");
        std::regex re_v6_map_objects("v6_map_\\d+_objects\\.dat");
        std::smatch m;
        std::string fname_only = std::filesystem::path(path).filename().string();
        if (std::regex_search(fname_only, m, re_v5_enemies) || std::regex_search(fname_only, m, re_v5_npcs)) {
            auto v = fio_cmm.load_from_disk<file_v5_map_npc>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        // v6 map objects
        if (std::regex_search(fname_only, m, re_v6_map_objects)) {
            auto v = fio_cmm.load_from_disk<v6_stage_object>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        // slopes
        if (lower.find("map_slope_list") != std::string::npos || lower.find("v5_map_slope") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v5_slope_tile>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        // common v5 lists: headers, links, areas, rooms
        if (lower.find("map_header_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v5_map_header>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("map_link_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v5_map_link>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("area_list") != std::string::npos && lower.find("v6_area") == std::string::npos) {
            // v5 area list (generic name) -> try room data struct
            auto v = fio_cmm.load_from_disk<file_v5_map_room_data>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("room_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v5_map_room_data>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        // generic attempt: try v6_room, v6_level_point, v6_stage_object, v6_area, v6_stage
        // scenes
        if (lower.find("scenes_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_list>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_show_image") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_show_image>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_show_viewpoint") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_show_viewpoint>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_show_text") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_show_text>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_show_animation") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_show_animation>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_play_sfx") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_play_sfx>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("scenes_play_music") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_scene_play_music>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_scenes") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<st_game_scene_item>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("v6_area_rooms_") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v6_room>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("v6_level_list_") != std::string::npos || lower.find("v6_level_point") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v6_level_point>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("v6_area_list_") != std::string::npos || lower.find("v6_area") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v6_area>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("v6_map_list") != std::string::npos || lower.find("v6_stage_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v6_stage>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("v6_style_list") != std::string::npos || lower.find("v6_style") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_v6_style>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_npc_state_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_npc_state>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("game_object_state") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<v6_file_game_object_state>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }
        if (lower.find("player_list_v3_1_1") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_player_v3_1_1>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        // projectiles
        if (lower.find("projectile") != std::string::npos || lower.find("game_projectile_list") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<file_projectilev3>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

        // game dialog state
        if (fname_only == "game_dialog_state.dat" || lower.find("game_dialog_state") != std::string::npos) {
            auto v = fio_cmm.load_from_disk<e_GAME_DIALOG>(path);
            std::string out = path.substr(0, path.size()-4) + ".json";
            maybe_save_json(v, out);
            return;
        }

    } catch (const std::exception &e) {
        std::string msg = std::string("Conversion error for ") + path + ": " + e.what();
        std::cerr << msg << std::endl;
        logger(msg);
    } catch (...) {
        std::string msg = std::string("Unknown conversion error for ") + path;
        std::cerr << msg << std::endl;
        logger(msg);
    }
}

void MainWindow::convert_legacy_dat_to_json(const std::string &base_dir)
{
    fio_common fio_cmm;

    path p(base_dir);
    if (!exists(p)) return;

    // logger that writes to GUI and stdout
    auto logger = [&](const std::string &msg) {
        if (conversion_log) conversion_log->append(QString::fromStdString(msg));
        std::cout << msg << std::endl;
    };

    // scan top-level and data/ subdirectory
    std::vector<path> scan_dirs = {p, p / "data", p / "scenes"};
    for (auto &d : scan_dirs) {
        if (!exists(d) || !is_directory(d)) continue;
        for (auto &entry : recursive_directory_iterator(d)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension().string();
            for (auto &c : ext) c = std::tolower((unsigned char)c);
            if (ext == ".dat") {
                std::string full = entry.path().string();
                std::string jsonp = full.substr(0, full.size()-4) + ".json";
                if (!std::filesystem::exists(jsonp)) {
                    try_convert_dat(full, fio_cmm, logger);
                }
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Game Data Editor");

    // create conversion log dock
    conversion_log = new QTextEdit(this);
    conversion_log->setReadOnly(true);
    conversion_dock = new QDockWidget(tr("Conversion Log"), this);
    conversion_dock->setWidget(conversion_log);
    addDockWidget(Qt::BottomDockWidgetArea, conversion_dock);

    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(ui->actionPlace_Enemy);
    toolGroup->addAction(ui->actionPlace_Object);
    toolGroup->setExclusive(true); // Only one action can be checked at a time
    ui->actionPlace_Enemy->setChecked(true);

    loadConfig();
    // perform best-effort conversion of legacy .dat files to .json when possible
    convert_legacy_dat_to_json(config.game_data_directory);

    ui->editAreaWidget->setGameDataDirectory(config.game_data_directory);
    ui->editAreaWidget->loadMapData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadConfig() {
    std::ifstream is(config_filename);

    if (is.is_open()) {
        cereal::JSONInputArchive iarchive(is); // Create an input archive
        std::string directory;
        iarchive(directory); // Read the data from the archive
        config.game_data_directory = directory;
    } else {
        QString dir;
        while (dir.isEmpty()) {
            QString dir = QFileDialog::getExistingDirectory(this, tr("Select Game Data Folder"),
                                                                QDir::currentPath(),
                                                                QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);

            if (!dir.isEmpty()) {
                config.game_data_directory = dir.toStdString();
                std::ofstream os(config_filename);
                cereal::JSONOutputArchive oarchive(os);
                oarchive(config.game_data_directory);
                break;
            }
        }
    }
}

void MainWindow::on_actionEnemy_Editor_triggered()
{
    enemyEdit.start(config.game_data_directory);
}


void MainWindow::on_actionPlace_Enemy_triggered()
{
    editMode = e_editMode_ENEMY;
}


void MainWindow::on_actionPlace_Object_triggered()
{
    editMode = e_editMode_OBJECT;
}


void MainWindow::on_actionObject_Editor_triggered()
{
    objectEdit.start(config.game_data_directory);
}

