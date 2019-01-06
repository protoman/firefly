#ifndef CLASS_CONFIG_H
#define CLASS_CONFIG_H

#include "data/st_common.h"
#include "character/classplayer.h"

#define WPN_COLS 2
#define WPN_ROWS 6

/**
 * @brief
 *
 */
enum e_tank_type {TANK_ENERGY, TANK_WEAPON, TANK_SPECIAL};

// ********************************************************************************************** //
// This class handles all configuration menus and in-game weapon and item selection screen        //
// ********************************************************************************************** //
/**
 * @brief
 *
 */
class class_config
{
public:
	class_config();
    void draw_ingame_menu();
    void set_player_ref(classPlayer* set_player_ref);
    void disable_ingame_menu();




private:
    bool ingame_menu_active; /**< TODO */
    st_position ingame_menu_pos; /**< Cursor porion in menu. X = 0 means left column, x=1 means right column */
    classPlayer* player_ref; /**< TODO */
};

#endif // CLASS_CONFIG_H
