#ifndef FILE_IO_H
#define FILE_IO_H

#include "format.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include "file/v6/file_level_v6.h"



// ************************************************************************************************************* //

class file_io {

public:
    file_io();
    void read_game(file_game& data_out);
    void write_game(file_game& data_in);
    std::string get_game_properties_filename();

    bool file_exists(std::string filename);
    std::vector<std::string> read_game_list();
    std::vector<std::string> read_directory_list(std::string filename, bool dir_only);
    std::vector<std::string> read_file_list(std::string filename);

    std::string get_save_filename(short save_n);
    bool write_save(st_save& data_in, short save_n);
    bool read_save(st_save& data_out, short save_n);

    std::string get_object_state_filename();
    void write_game_object_state();
    void load_game_object_state();

    std::string get_game_dialog_state_filename();
    void load_game_dialog_state();
    void write_game_dialog_state();

    bool save_exists(short save_n);
    bool have_one_save_file();
    bool can_access_castle(st_save& data_in);

    void load_config(st_game_config &config);
    void save_config(st_game_config &config) const;

    void generate_files();

    int get_heart_pieces_number(st_save game_save);

    std::string get_sufix();

private:
    std::string sufix;
    //fio_common fio_cmm;
};




#endif // FILE_IO_H

