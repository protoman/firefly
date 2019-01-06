#ifndef SCENESLIB_H
#define SCENESLIB_H

#include "defines.h"
#include "scenes/game_menu.h"
#include "file/file_io.h"

/**
 * @brief
 *
 */
class scenesLib
{
public:
/**
 * @brief
 *
 */
    scenesLib();
    void preloadScenes();
    void draw_main();
    void main_screen();
    void show_cheats_menu();

private:
    // preload/unload
    void preload_faces();/// @TODO: move to graphLib and create a function show_face(st_position)
    void unload_faces();/// @TODO: move to graphLib and create a function show_face(st_position)




    void config_int_value(Uint8 &value_ref, int min, int max);
    Uint8 select_difficulty();
    short select_save(bool is_new_game);
    void draw_save_details(int n, st_save save);


private:

    unsigned int _timer; // animation timer
    short _state; // animation state
    st_imageData* _blink_surface; // animation surface (light/dark)
    std::map<Uint8, Uint8> game_scenes_map;
    game_menu menu;
    file_io fio;

};

#endif // SCENESLIB_H
