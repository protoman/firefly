#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "file_io.h"
#include "convert.h"
#include "../file/convert.h"
#include "aux_tools/stringutils.h"
#include "aux_tools/exception_manager.h"

#ifdef ANDROID
#include <android/log.h>
#endif

// versioned file for config, so we can force resetting it
#define CONFIG_FILENAME "/config_v204.sav"

#include "data/shareddata.h"


// ************************************************************************************************************* //


file_io::file_io()
{
    sufix = "_v301";
}



void file_io::write_game(file_game& data_in) const {
    std::ofstream fp;
    std::string filename = "";


// -------------------------------------- GAME -------------------------------------- //
    filename = std::string(SharedData::get_instance()->FILEPATH) + "/game_properties" + sufix + ".dat";
    fp.open(filename.c_str(), std::ios::out | std::ios::binary | std::ios::ate);
    if (!fp.is_open()) {
        std::cout << "ERROR::write_game - could not write to file '" << filename << "'. Will create new one." << std::endl;
        fp.open(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    }



    fp.write(reinterpret_cast<char *>(&data_in.version), sizeof(float));
    fp.write(reinterpret_cast<char *>(&data_in.name), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.semi_charged_projectile_id), sizeof(Sint8));

    fp.write(reinterpret_cast<char *>(&data_in.boss_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.final_boss_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.got_weapon_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.game_over_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.stage_select_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);
    fp.write(reinterpret_cast<char *>(&data_in.game_start_screen_music_filename), sizeof(char) * FS_CHAR_NAME_SIZE);

    fp.write(reinterpret_cast<char *>(&data_in.use_second_castle), sizeof(bool));
    fp.write(reinterpret_cast<char *>(&data_in.game_style), sizeof(Uint8));
    fp.write(reinterpret_cast<char *>(&data_in.final_boss_id), sizeof(Uint8));

    fp.close();

}







void file_io::read_game(file_game& data_out) const {
    FILE *fp;
    std::string filename = "";


    bool USE_NEW_READ = true;
    if (USE_NEW_READ == false) {
        filename = std::string(SharedData::get_instance()->FILEPATH) + "/game" + sufix + ".dat";
        fp = fopen(filename.c_str(), "rb");
        if (!fp) {
            std::cout << ">>file_io::read_game - file '" << filename << "' not found." << std::endl;
            fflush(stdout);
            return;
        }
        int read_result = fread(&data_out, sizeof(struct file_game), 1, fp);
        if (read_result  == -1) {
            std::cout << ">>file_io::read_game - Error reading struct data from game file '" << filename << "'." << std::endl;
            fflush(stdout);
            exception_manager::throw_general_exception(std::string("file_io::read_game - Error reading data from file."), filename);
        }
        fclose(fp);
        return;
    }
// -------------------------------------- GAME -------------------------------------- //
    // float version;
    // char name[FS_CHAR_NAME_SIZE];
    // Sint8 semi_charged_projectile_id;
    // Sint8 player_items[FS_PLATER_ITEMS_N];
    // char stage_face_filename[MAX_STAGES][FS_FACE_FILENAME_MAX]
    filename = std::string(SharedData::get_instance()->FILEPATH) + "/game_properties" + sufix + ".dat";
    fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        std::cout << ">>file_io::read_game - file '" << filename << "' not found." << std::endl;
        return;
    }


    if (unsigned int res = fread(&data_out.version, sizeof(float), 1, fp) != 1) {
        std::cout << ">>file_io::read_game - res: " << res << ", sizeof(float): " << sizeof(float) << ", Error reading struct data [version2] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (unsigned int res = fread(&data_out.name, sizeof(char), FS_CHAR_NAME_SIZE, fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game - res: " << res << ", sizeof(char): " << sizeof(char) << ", Error reading struct data from [name] game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.semi_charged_projectile_id, sizeof(Sint8), 1, fp) != 1) {
        std::cout << ">>file_io::read_game - Error reading struct data [semi_charged_projectile_id] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.boss_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [boss_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.final_boss_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [final_boss_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.got_weapon_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [got_weapon_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.game_over_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [game_over_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.stage_select_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [stage_select_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }

    if (fread(&data_out.game_start_screen_music_filename, sizeof(char), (FS_CHAR_NAME_SIZE), fp) != FS_CHAR_NAME_SIZE) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [stage_select_music_filename] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }

    if (fread(&data_out.use_second_castle, sizeof(bool), 1, fp) != 1) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [use_second_castle] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.game_style, sizeof(Uint8), 1, fp) != 1) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [game_style] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }
    if (fread(&data_out.final_boss_id, sizeof(Uint8), 1, fp) != 1) {
        std::cout << ">>file_io::read_game res: - Error reading struct data [final_boss_id] from game file '" << filename << "'." << std::endl;
        fclose(fp);
        exit(-1);
    }

    fclose(fp);
}



bool file_io::file_exists(std::string filename) const
{
    std::cout << "file_io::file_exists.filename[" << filename << "]" << std::endl;
    bool res = false;
    FILE *fp;
    fp = fopen(filename.c_str(), "rb");
    if (fp) {
        res = true;
        fclose(fp);
    }
    return res;
}

std::vector<std::string> file_io::read_game_list()
{
    std::string filename = SharedData::get_instance()->GAMEPATH + "/games/";
    filename = StringUtils::clean_filename(filename);
    return read_directory_list(filename, true);
}



// @TODO: make this work in multiplatform
// http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
std::vector<std::string> file_io::read_directory_list(std::string filename, bool dir_only)
{
    std::vector<std::string> res;
    filename = StringUtils::clean_filename(filename);


    DIR *dir = opendir(filename.c_str());


    struct dirent *entry = readdir(dir);

    while (entry != nullptr) {

        //std::cout << ">>>>>>>>> entry->d_name: " << entry->d_name << std::endl;

        std::string dir_name = std::string(entry->d_name);
        if (dir_name != "." && dir_name != "..") {
            DIR *child_dir;
            std::string child_dir_path = filename + std::string("/") + dir_name;
            child_dir = opendir (child_dir_path.c_str());

            if (dir_only == true && child_dir != nullptr) {
                res.push_back(dir_name);
            } else if (dir_only == false && child_dir == nullptr) {
                res.push_back(dir_name);
            }
        }
        entry = readdir(dir);

    }
    closedir(dir);

    return res;

}



std::vector<std::string> file_io::read_file_list(std::string filename)
{
    return read_directory_list(filename, false);
}





void file_io::load_config(st_game_config& config)
{
    FILE *fp;
    std::string filename = std::string(SharedData::get_instance()->SAVEPATH) + CONFIG_FILENAME;
    filename = StringUtils::clean_filename(filename);
    fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        std::cout << "WARNING: Could not read config file '" << filename.c_str() << "'." << std::endl;
    } else {
        int read_result = fread(&config, sizeof(struct st_game_config), 1, fp);
        if (read_result  == -1) {
            printf(">>file_io::read_game - Error reading struct data from game file '%s'.\n", filename.c_str());
            fflush(stdout);
            exit(-1);
        }
        fclose(fp);
    }
    if (config.get_current_platform() != config.platform) {
        config.reset();
    }

    if (config.volume_music == 0) {
        config.volume_music = 128;
    }
    if (config.volume_sfx == 0) {
        config.volume_sfx = 128;
    }

}

void file_io::save_config(st_game_config &config) const
{
    FILE *fp;
    std::string filename = std::string(SharedData::get_instance()->SAVEPATH) + CONFIG_FILENAME;
    filename = StringUtils::clean_filename(filename);
    fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cout << "Error: Could not open config file '" << filename << "'." << std::endl;
        exit(-1);
    }
    fwrite(&config, sizeof(struct st_game_config), 1, fp);
    fclose(fp);
}


std::string file_io::get_save_filename(short save_n)
{
    char numbered_file[50];

    sprintf(numbered_file, "_0%d", save_n);
    std::string filename = std::string(SharedData::get_instance()->SAVEPATH) + std::string("/") + SharedData::get_instance()->GAMENAME + std::string(numbered_file) + std::string(".sav");
    filename = StringUtils::clean_filename(filename);

    return filename;
}

bool file_io::read_save(st_save& data_out, short save_n)
{
    FILE *fp;

    std::string filename = get_save_filename(save_n);

    fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        std::cout << "ERROR: Could not read save" << std::endl;
        return false;
    }
    int read_result = fread(&data_out, sizeof(struct st_save), 1, fp);
    if (read_result  == -1) {
        printf(">>file_io::read_game - Error reading struct data from game file '%s'.\n", filename.c_str());
        fflush(stdout);
        return false;
    }


    // ------- DEBUG ------- //
    /*
    data_out.stages[INTRO_STAGE] = 1;
    for (int i=STAGE1; i<=STAGE8; i++) {
        data_out.stages[i] = 1;
    }
    for (int i=CASTLE1_STAGE1; i<CASTLE1_STAGE5; i++) {
        data_out.stages[i] = 1;
    }
    */
    //data_out.stages[INTRO_STAGE] = 1;
    //data_out.stages[STAGE1] = 1;
    //data_out.stages[STAGE2] = 1;
    //data_out.stages[STAGE3] = 1;
    //data_out.stages[STAGE4] = 0;
    //data_out.stages[CASTLE1_STAGE1] = 1;
    //data_out.stages[CASTLE1_STAGE2] = 1;
    //data_out.stages[CASTLE1_STAGE3] = 1;
    //data_out.stages[CASTLE1_STAGE4] = 1;
    //data_out.stages[STAGE3] = 0;
    //data_out.armor_pieces[ARMOR_ARMS] = true;
    //data_out.armor_pieces[ARMOR_BODY] = true;
    //data_out.armor_pieces[ARMOR_LEGS] = true;

    //data_out.items.energy_tanks = 1;
    //data_out.items.weapon_tanks = 2;
    //data_out.items.special_tanks = 9;

    // ------- DEBUG ------- //


    if (data_out.items.lifes > 9) {
        data_out.items.lifes = 3;
    }
    if (data_out.items.weapon_tanks > 9) {
        data_out.items.weapon_tanks = 9;
    }
    if (data_out.items.energy_tanks > 9) {
        data_out.items.energy_tanks = 9;
    }
    if (data_out.items.special_tanks > 1) {
        data_out.items.special_tanks = 1;
    }


    fclose(fp);

    return true;
}



bool file_io::write_save(st_save& data_in, short save_n)
{
    FILE *fp;

    std::string filename = get_save_filename(save_n);

    fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cout << "Error: Could not open save-file '" << filename << "'." << std::endl;
        return false;
    }

    std::cout << "file_io::write_save[" << filename << "]" << std::endl;

    fwrite(&data_in, sizeof(struct st_save), 1, fp);
    fclose(fp);
    return true;
}

bool file_io::save_exists(short save_n)
{
    FILE *fp;
    std::string filename = get_save_filename(save_n);

    fp = fopen(filename.c_str(), "rb");
    if (fp) {
        return true;
    }
    return false;
}

bool file_io::have_one_save_file()
{
    for (int i=0; i<5; i++) {
        if (save_exists(i) == true) {
            return true;
        }
    }
    // check if we can convert an old-format save to new one
    return false;
}







// create and save empty files for new-game
void file_io::generate_files()
{
    file_game data_in;
    write_game(data_in);
}

int file_io::get_heart_pieces_number(st_save game_save)
{
    int res = PLAYER_INITIAL_HP;
    for (int i=0; i<WEAPON_COUNT; i++) {
        if (game_save.items.heart_pieces[i] == true) {
            res++;
        }
    }
    return res;
}



std::string file_io::get_sufix()
{
    return sufix;
}







