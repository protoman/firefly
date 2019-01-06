#ifndef CLASSPLAYER_H
#define CLASSPLAYER_H

#include "character/character.h"
#include "character/classnpc.h"
#include "file/file_io.h"

class classnpc; // advance declaration

/**
 * @brief child of character, this class is used to store data for a player (human controlled character)
 */
class classPlayer : public character
{
public:
    /**
     * @brief constructor for player class
     * @param std::string the name of the player
     * @param int number of the player. to be later used when we re-add support for the simultaneous two player mode
     */
    classPlayer(int playerNumber);

    void set_player_name(std::string set_name);

    // called after game was picked and loaded, so we have player data
    void initialize();

    /**
     * @brief hardcoded method for setting each frame for a player (@TODO: replace by user driven data)
     */
    void initFrames();

    /**
     * @brief execute all player actions (move, jump, slide, damage, projectiles)
     */
    void execute();

    /**
     * @brief move the projectiles created by this player and damage any npcs that are in it's way
     */
    void execute_projectiles();

    /**
     * @brief this is used when player enters a boss-teleport that teleport the player back to it's origin point
     * @param n id of the teleporter link
     */
    void set_teleporter(int n);

    /**
     * @brief returns the teleporter id the player is using
     * @return int teleporter-link id, -1 if none is being used
     */
    int get_teleporter();

    /**
     * @brief set the player HP (hit points) back to the default value
     */
    void reset_hp();

    /**
     * @brief don't stop teleport because of collision before reaching this Y position (used when player is dead and returning)
     * @param y point that indicates the minimal y position the teleport gravity will check collision
     */
    void set_teleport_minimal_y(int y);

    /**
     * @brief virtual function from character. returns always false, as a player don't fly
     * @return bool returns false always
     */
    bool can_fly();

    /**
     * @brief
     *
     */
    void reset_charging_shot();

    bool is_teleporting();

    /**
     * @brief recharge player's HP or current weapon
     * @param _en_type HP or WEAPON to be recharged
     * @param value total amount of points that will be recharged
     */
    void recharge(e_energy_types _en_type, int value);

    void damage(unsigned int damage_points, bool ignore_hit_timer);

    void damage_spikes(bool ignore_hit_timer);

    // to be used when game is paused
    void save_input();
    void restore_input();

    Uint8 get_max_hp();


private:
    void move();
    void death();
    bool get_item(object_collision& obj_info);


    /**
     * @brief execute an attack, including weapon usage
     */
    void attack(bool dont_update_colors = false);

    /**
     * @brief damage all NPCs that are touching ground and inside game-screen area
     */
    void damage_ground_npcs();



    void clean_move_commands();

    bool can_shoot_diagonal();

    bool can_double_jump();


    bool can_air_dash();

    bool shoryuken();

    void consume_weapon(int value);



private:
    file_io fio;
    int teleporter_n; /**< current teleporter being used, -1 if none */
    short selected_weapon; /**< current selected weapon */
    bool l_key_released; /**< avoid changing weapon continuously if L key is held */
    bool r_key_released;            // < avoid changing weapon continuously if R key is held
    bool _weapons_array[WEAPON_COUNT];
};

#endif // CLASSPLAYER_H
