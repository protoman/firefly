#ifndef FIO_STRINGS_H
#define FIO_STRINGS_H

#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <map>

#include "file/v4/file_strings.h"

// namespace
class fio_strings
{
public:
    fio_strings();

    // === GAME STRINGS === //
    std::string get_ingame_string(int n, int language, bool convert_symbols);
    std::vector<std::string> load_game_strings(int language, bool convert_symbols);
    std::vector<std::string> load_game_strings_from_file(std::string filename, int language, bool convert_symbols);
    void save_game_strings(std::vector<std::string> list, std::string filename);

    // === DIALOGS === //
    void save_game_dialogs(std::vector<std::vector<std::string>>);
    std::vector<std::vector<std::string>> load_game_dialogs();
    void save_npc_dialog(int language_id, int npc_id, std::string text);
    std::string get_npc_dialog_filename(int language_id, int npc_id);
    std::string load_npc_dialog(int language_id, int npc_id);

    // === COMMON STRINGS === //
    std::string get_common_strings_filename(int language);
    std::string get_game_strings_filename(int language);
    std::vector<std::string> get_common_strings(int language, bool convert_symbols);
    std::string get_common_string(int id, int language, bool convert_symbols);
    std::string get_stage_dialog(short stage_id, int id, int language, bool convert_symbols);
    std::vector<std::string> get_stage_dialogs(short stage_id, int language, bool convert_symbols);
    void save_common_strings(std::vector<std::string> data, int language);
    void create_files();
    void save_stage_dialogs(short stage_id, int language, std::vector<std::string> data);

    std::string get_language_prefix(int config);

    // === GENERIC FROM-FILE STRINGS === //
    std::vector<std::string> get_string_list_from_file(std::string filename);


    // === scenes string files === //
    std::vector<std::string> get_string_list_from_scene_text_file(int text_scene_n, int language);
    void write_scene_text_file(int text_scene_n, std::vector<std::string> list, int language);

    std::string get_language_filename_prefix(int language);


private:
    // === GAME STRINGS === //
    bool file_exists(std::string filename);
    std::vector<std::string> get_default_ingame_strings_list(int language);
    void create_default_ingame_strings();
    std::vector<std::string> add_missing_default_ingame_strings(std::vector<std::string> list, int language);
    std::string get_stage_dialogs_filename(short stage_id, int language);
    std::string convert_text_symbols(std::string text);


    // === COMMON STRINGS === //
    void create_default_common_strings();


private:
    std::vector<std::string> string_list;
    std::vector<std::string> common_strings_list;
    std::vector<std::string> scenes_strings_list;
    short _dialogs_stage_id;    // sotres the loaded stage dialogs id, if this changes, we get the dialogs form the new id
    std::vector<std::string> dialogs_strings_list;
    std::map<int,std::string> code_map;

};


#endif // FIO_STRINGS_H
