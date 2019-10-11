#ifndef DEFINES_H
#define DEFINES_H

#define RES_W 1280
#define RES_H 720
#define VIDEO_MODE_COLORS 32
#define HUD_H 80
#define TILESIZE 64
#define AREA_H (RES_H-HUD_H)
#define HUD_GRID_SIZE 21
#define HUD_GRID_BORDER 2
#define HUD_GRID_CENTER_X 1207
#define HUD_GRID_CENTER_Y 28


// INPUT //
#define DOUBLE_TAP_DELTA 500
#define JOYVAL 30000
// FILE NAMES //
#define FS_CHAR_FILENAME_SIZE 255
#define V5_CHAR_NAME 512
// FILES //
#define PROJECTILE_FILE_V3 "data/game_projectile_list_v3.dat"
#define FILE_V5_MAP_HEADER_LIST "data/map_header_list.dat"
#define FILE_V5_MAP_LINK_LIST "data/map_link_list.dat"
#define FILE_V5_MAP_SLOPE_LIST "data/slope_list.dat"
#define FILE_V5_AREA_LIST "data/area_list.dat"
#define FILE_V5_ROOM_LIST "data/room_list.dat"
#define MAXPATHLEN 1024
#define FS_FILENAME_SIZE 30

#define LAYERS_COUNT 10
#define LAYERS_BG_COUNT 5
#define LAYERS_FG_COUNT 5

#define FILE_V6_LEVEL_LIST "data/v6_level_list.dat"
#define FILE_V6_VISITED_LEVEL_LIST "data/v6_visited_level_list.dat"
#define FILE_V6_MAP_LIST "data/v6_map_list.dat"

#define FS_MAX_PLAYERS 4
#define PLAYER_INITIAL_HP 100
#define PLAYER_SPRITE_SIZE 162
#define CHAR_NAME_SIZE 30
#define FS_CHAR_NAME_SIZE 30
#define FS_NAME_SIZE 30
#define FS_ANIM_TILE_MAX_FRAMES 6 // is we want to use more than 6 frames, we are doing it wrong...

#define PROJECTILE_DEFAULT_SPEED 6
#define PROJECTILE_DEFAULT_DAMAGE 1
#define PROJECTILE_DEFAULT_ANIMATION_TIME 100
#define OLD_ANIM_TYPE_COUNT 29
#define ANIM_FRAMES_COUNT 10 // max number of animation frames for each animation_type
#define FS_DIALOG_LINES 6
#define DIALOG_LINE_LIMIT 60
#define STRINGS_LINE_SIZE 60

#define SHOW_TILESIZE 32

#define SCENES_LINE_H_DIFF 12
#define SCENES_TEXT_BOTTOM_POSY 160

#define EDITOR_ANIM_PALETE_MAX_COL 8

#define FONT_SIZE 32
#define FONT_ACTUAL_SIZE_W 19
#define FONT_ACTUAL_SIZE_H 22
#define FONT_SIZE_SMALL 18

#define GRAVITY_MAX_SPEED 16
#define GRAVITY_TELEPORT_MAX_SPEED 16

#define BOSS_HIT_DURATION 800

#define CURSOR_SPACING 12

#define ENERGY_ITEM_SMALL 3
#define ENERGY_ITEM_BIG 8

// FRAMES //
#define ANIM_TILE_MAX_FRAMES 10
#define PREVIOUS_FRAMES_MAX 12
#define WALK_FRAME_DELAY 110
#define CHAR_ANIM_DIRECTION_COUNT 2 // characters use only left/right, not like projectiles


#define TOUCH_DAMAGE_SMALL 2
#define TOUCH_DAMAGE_BIG 4

// MULTIPLIERS //
#define WATER_SPEED_MULT 0.7
#define STAIRS_MOVE_MULTIPLIER 0.8

#define PLAYER_RIGHT_TO_LEFT_DIFF 5
#define PLAYER_LEFT_TO_RIGHT_DIFF 5

#define BOSS_INITIAL_HP 100

// TIMES AND DELAYS //
#define ATTACK_DELAY 300 // how many milisseconds the attack frame must last before returning to stand/jump/walk
#define HIT_BLINK_ANIMATION_LAPSE 100
#define CHARGED_SHOT_INITIAL_TIME 1000
#define CHARGED_SHOT_TIME 2200
#define SUPER_CHARGED_SHOT_TIME 3200
#define TURBO_ATTACK_INTERVAL 200


#define AI_MAX_STATES 20

#define OBJ_JUMP_Y_ADJUST 6

#define CHAR_OBJ_COLlISION_KILL_ADJUST 4

#define PLAYER_NPC_COLLISION_REDUTOR 6

#define MAX_PLAYER_SPRITES 23
#define MAX_NPC_SPRITES 15


#define MOVING_GROUND 10 //TREADMILL

#define TRANSITION_STEP 16

#define MAIN_MENU_CHEAT_RETURN 99

#define CONFIG_BGCOLOR_R 8
#define CONFIG_BGCOLOR_G 25
#define CONFIG_BGCOLOR_B 42

#define BOSS_INTRO_BG_TEXT_Y 184
#define BOSS_INTRO_BG_POS_Y 96

// DAMAGES //
#define TOUCH_DAMAGE_SMALL 2
#define TOUCH_DAMAGE_SMALL 2
#define TOUCH_DAMAGE_BIG 4
#define SPIKES_DAMAGE 188
#define PROJECTILE_DEFAULT_DAMAGE 1

#define SAVE_MAX_SLOT_NUMBER 9

#define TEXT_DEFAUL_COLOR_VALUE 240

#define CONFIG_MENU_LEFT_SPACING 24
#define CONFIG_MENU_TOP_SPACING 40

#define AUTOSCROLL_START_DELAY_FRAMES 15

#define PLAYER_INITIAL_X_POS 144 // half-screen

#define SLOPE_MAX_TILES 6

#define GAME_AREA_SIZE 30
#define GAME_AREA_W 20
#define GAME_AREA_H 10
#define AREA_ROOM_W 20
#define AREA_ROOM_H 10

// SAVE/ITEMS //
#define HEART_PIECES 30
#define ITEM_SLOTS 3
#define WEAPON_SLOTS 2
#define ABILITIES_SLOTS 2
#define GAME_ITEM_SLOTS 3
#define ITEM_UPGRADES 200  // items like armor, special weapons, etc

#define SLIDE_TILES_MAX_DISTANCE 10

#define GAME_TEXT_LINE_SEPARATOR_CHAR ';'
#define GAME_TEXT_DIALOG_SEPARATOR_CHAR '#'

enum e_KEY_TYPE {
    KEY_TYPE_NONE,
    KEY_TYPE_PINK,
    KEY_TYPE_BLUE,
    KEY_TYPE_GREEN,
    KEY_TYPE_RED,
    KEY_TYPE_PURPLE,
    KEY_TYPE_ORANGE,
    KEY_TYPE_BROWN,
    KEY_TYPE_YELLOW,
    KEY_TYPE_AQUA,
    KEY_TYPE_COUNT
};

enum e_WALL_DIRECTIONS {
    WALL_DIRECTION_NORTH,
    WALL_DIRECTION_SOUTH,
    WALL_DIRECTION_WEST,
    WALL_DIRECTION_EAST,
    WALL_DIRECTION_COUNT
};

enum e_BG_EXTRA_WIDTH_HANDLER {
    BG_EXTRA_WIDTH_HANDLER_EXPAND,
    BG_EXTRA_WIDTH_HANDLER_ANIMATE,
    BG_EXTRA_WIDTH_HANDLER_COUNT
};

enum e_PROPERTIES_NAMES {
    PROPERTY_NAME_SLIDE,
    PROPERTY_NAME_COUNT
};

enum e_AREA_EDIT_MODE {
    AREA_EDIT_MODE_NORMAL,
    AREA_EDIT_MODE_VLINK,
    AREA_EDIT_MODE_HLINK,
    AREA_EDIT_MODE_COUNT
};

enum SFX_LIST {
    SFX_NPC_HIT,
    SFX_NPC_KILLED,
    SFX_PLAYER_HIT,
    SFX_PLAYER_SHOT,
    SFX_PLAYER_JUMP,
    SFX_CURSOR,
    SFX_STAGE_SELECTED,
    SFX_GOT_ENERGY,
    SFX_GOT_ITEM,
    SFX_SHOT_REFLECTED,
    SFX_DOOR_OPEN,
    SFX_GOT_WEAPON,
    SFX_TELEPORT,
    SFX_IMPLOSION,
    SFX_PLAYER_DEATH,
    SFX_PLAYER_CHARGED_SHOT,
    SFX_CHARGING1,
    SFX_CHARGING2,
    SFX_BIG_EXPLOSION,
    SFX_WATER_ENTER,
    SFX_WATER_LEAVE,
    SFX_DISAPPEARING_BLOCK,
    SFX_SHORYUKEN_GIRL,
    SFX_BEAM,
    SFX_GOT_ENERGY_BIG,
    SFX_COUNT // not used as sfx, this is a way to measure size of the enum list
};

enum e_PLAYERS {
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    PLAYER_COUNT
};

enum e_transition_types {
    NO_TRANSITION,
    TRANSITION_TOP_TO_BOTTOM,
    TRANSITION_BOTTOM_TO_TOP,
    TRANSITION_LEFT_TO_RIGHT,
    TRANSITION_RIGHT_TO_LEFT
};

enum DEAD_STATE_LIST {
    DEAD_STATE_ALIVE,
    DEAD_STATE_DYING,
    DEAD_STATE_DEAD,
    DEAD_STATE_IGNORE,
    DEAD_STATE_COUNT
};


enum AI_ACTION_JUMP_OPTION_LIST {
    AI_ACTION_JUMP_OPTION_AHEAD,
    AI_ACTION_JUMP_OPTION_TO_RANDOM_POINT,
    AI_ACTION_JUMP_OPTION_UP,
    AI_ACTION_JUMP_OPTION_TO_PLAYER,
    AI_ACTION_JUMP_OPTION_TO_ROOF,
    AI_ACTION_JUMP_OPTION_TO_SAVED_POINT,
    AI_ACTION_JUMP_OPTION_ONCE
};

enum AI_ACTION_WALK_OPTION_LIST {
    AI_ACTION_WALK_OPTION_HORIZONTAL_AHEAD,
    AI_ACTION_WALK_OPTION_HORIZONTAL_TURN, // execute turn animation, ends when animation is compelted
    AI_ACTION_WALK_OPTION_TO_PLAYER,
    AI_ACTION_WALK_OPTION_TO_SAVED_POINT,
    AI_ACTION_WALK_OPTION_TURN_TO_PLAYER,
    AI_ACTION_WALK_OPTION_TO_RANDOM_DIRECTION,
    AI_ACTION_WALK_OPTION_TO_OPPOSITE_DIRECTION,
    AI_ACTION_WALK_OPTION_COUNT
};

enum AI_ACTION_TELEPORT_OPTION_LIST {
    AI_ACTION_TELEPORT_OPTION_LEFT,
    AI_ACTION_TELEPORT_OPTION_RIGHT,
    AI_ACTION_TELEPORT_OPTION_TO_PLAYER,
    AI_ACTION_TELEPORT_OPTION_RANDOM_X,
    AI_ACTION_TELEPORT_OPTION_RANDOM_Y,
    AI_ACTION_TELEPORT_OPTION_RANDOM_POINT,
    AI_ACTION_TELEPORT_OPTION_AHEAD,
    AI_ACTION_TELEPORT_OPTION_COUNT
};

enum AI_ACTION_DASH_OPTION_LIST {
    AI_ACTION_DASH_OPTION_LEFT,
    AI_ACTION_DASH_OPTION_RIGHT,
    AI_ACTION_DASH_OPTION_TO_PLAYER,
    AI_ACTION_DASH_OPTION_OPPOSITE_DIRECTION,
    AI_ACTION_DASH_OPTION_AHEAD,
    AI_ACTION_DASH_OPTION_COUNT
};

enum AI_ACTION_GRAB_WALL_OPTION_LIST {
    AI_ACTION_GRAB_WALL_OPTION_LEFT,
    AI_ACTION_GRAB_WALL_OPTION_RIGHT
};

enum AI_ACTION_AIR_WALK_OPTION_LIST {
    AI_ACTION_AIR_WALK_OPTION_LEFT,
    AI_ACTION_AIR_WALK_OPTION_RIGHT,
    AI_ACTION_AIR_WALK_OPTION_TO_PLAYER
};

enum AI_ACTION_FLY_OPTION_LIST {
    AI_ACTION_FLY_OPTION_HORIZONTAL_AHEAD,
    AI_ACTION_FLY_OPTION_HORIZONTAL_TURN,
    AI_ACTION_FLY_OPTION_UP,
    AI_ACTION_FLY_OPTION_DOWN,
    AI_ACTION_FLY_OPTION_TO_PLAYER,
    AI_ACTION_FLY_OPTION_TO_SAVED_POINT,
    AI_ACTION_FLY_OPTION_TO_RANDOM_POINT,
    AI_ACTION_FLY_OPTION_FALL, // fall until out of screen or hitting ground (ignores distance)
    AI_ACTION_FLY_OPTION_DRILL_DOWN,
    AI_ACTION_FLY_OPTION_VERTICAL_CENTER,
    AI_ACTION_FLY_OPTION_PLAYER_DIRECTION,
    AI_ACTION_FLY_OPTION_OPOSITE_WALL,
    AI_ACTION_FLY_OPTION_OPOSITE_SHOOT_1,
    AI_ACTION_FLY_OPTION_TO_PLAYER_X, // move only in the X axis
    AI_ACTION_FLY_OPTION_TO_PLAYER_Y, // move only in the Y axis
    AI_ACTION_FLY_OPTION_ZIGZAG_AHEAD, // similar to horizontal-aahead but moving a bit on Y axias up/down
    AI_ACTION_FLY_OPTION_RANDOM_X,
    AI_ACTION_FLY_OPTION_RANDOM_Y,
    AI_ACTION_FLY_OPTION_SIN_AHEAD,     // moves ahead making a sin on y-axis
    AI_ACTION_FLY_OPTION_DASH_TO_PLAYER,
    AI_ACTION_FLY_OPTION_COUNT
};

enum OBJ_STATE {
    OBJ_STATE_STAND,
    OBJ_STATE_MOVE,
    OBJ_STATE_RETURN,
    OBJ_STATE_COUNT
};

enum e_PRELOADED_IMAGES {
    PRELOADED_IMAGES_EXPLOSION_BUBBLE,
    PRELOADED_IMAGES_COUNT
};

enum e_flip_type {
    flip_type_horizontal,
    flip_type_vertical,
    flip_type_both
};


enum IA_TYPE_LIST {
    IA_STAND,
    IA_WAIT,
    IA_FOLLOW,
    IA_ZIGZAG,
    IA_SIDETOSIDE,
    IA_BAT,
    IA_ROOF_SHOOTER,
    IA_GROUND_SHOOTER,
    IA_SHOOT_AND_GO,
    IA_FLY_ZIG_ZAG,
    IA_BUTTERFLY,
    IA_HORIZONTAL_GO_AHEAD,
    IA_HORIZONTAL_TURN,
    IA_FIXED_JUMPER,
    IA_SIDE_SHOOTER,
    IA_GHOST,
    IA_FISH,
    IA_DOLPHIN,
    IA_VERTICAL_ZIGZAG, IA_TYPES_COUNT };


enum IA_STATES {
        IA_INITIAL_STATE,
        IA_STATE_SLEEP,
        IA_STATE_FOLLOW,
        IA_STATE_RETURN,
        IA_STATE_SHOT_RAINBOW,
        IA_STATE_CROSS_SCREEN,
        IA_STATE_SHOT_FLOWER,
        IA_STATE_WAIT_TIME,
        IA_STATE_START_MOVE,
        IA_STATE_START_STAND,
        IA_STATE_INTRO,
        IA_STATE_JUMP_TO_ROOF,
        IA_STATE_JUMP_TO_PLAYER,
        IA_STATE_AIR_WALK,
        IA_STATE_QUAKE_ATTACK,
        IA_STATE_JUMP_FALL,
        IA_STATE_GROUND_ATTACK,
        IA_STATE_TELEPORT,
        IA_STATE_WALK_TO_POINT,
        IA_STATE_CHARGED_DASH,
        IA_STATE_SHOT_STOP_TIME,
        IA_STATE_JUMP_TO_WALL,
        IA_STATE_FLOAT_TO_POINT,
        IA_STATE_COUNT
};

enum IA_CONDITIONALS {
    IA_CONDITION_NO_CONDITION,
    IA_CONDITION_NEAR_PLAYER,
    IA_CONDITION_ABOUT_TO_DIE,
    IA_CONDITION_ABOVE_PLAYER,
    IA_CONDITION_UNDER_PLAYER
};



enum E_BG_SCROLL_MODE {
    BG_SCROLL_MODE_NONE,
    BG_SCROLL_MODE_UP,
    BG_SCROLL_MODE_DOWN,
    BG_SCROLL_MODE_LEFT,
    BG_SCROLL_MODE_RIGHT,
    BG_SCROLL_MODE_DIAGONAL
};

enum e_shield_types {
    SHIELD_NO,
    SHIELD_FULL,
    SHIELD_FRONT,
    SHIELD_STAND,
    SHIELD_DISGUISE,
    SHIELD_STAND_FRONT,
    SHIELD_STAND_AND_WALK,
    SHIELD_COUNT
};

enum DROP_ITEMS_LIST {
    DROP_ITEM_1UP,
    DROP_ITEM_ENERGY_SMALL,
    DROP_ITEM_ENERGY_BIG,
    DROP_ITEM_WEAPON_SMALL,
    DROP_ITEM_WEAPON_BIG,
    DROP_ITEM_COIN,
    DROP_ITEM_COUNT
};

enum OBJECT_TYPE {
    OBJ_MOVING_PLATFORM_UPDOWN,
    OBJ_MOVING_PLATFORM_LEFTRIGHT,
    OBJ_DISAPPEARING_BLOCK,
    OBJ_ENERGY_TANK,
    OBJ_ENERGY_PILL_BIG,
    OBJ_WEAPON_PILL_BIG,
    OBJ_ENERGY_PILL_SMALL,
    OBJ_WEAPON_PILL_SMALL,
    OBJ_FALL_PLATFORM,
    OBJ_FLY_PLATFORM,
    OBJ_ITEM_FLY,
    OBJ_ITEM_JUMP,
    OBJ_ACTIVE_DISAPPEARING_BLOCK, // disappear only after player activates it
    OBJ_RAY_HORIZONTAL,
    OBJ_RAY_VERTICAL,
    OBJ_TRACK_PLATFORM,
    OBJ_BOSS_TELEPORTER,
    OBJ_DESTRUCTIBLE_WALL,
    OBJ_DEATHRAY_VERTICAL,
    OBJ_DEATHRAY_HORIZONTAL,
    OBJ_FINAL_BOSS_TELEPORTER,
    OBJ_ACTIVE_OPENING_SLIM_PLATFORM,
    OBJ_DAMAGING_PLATFORM,
    OBJ_CHECKPOINT,
    OBJ_BOSS_DOOR,
    OBJ_PLATFORM_TELEPORTER,
    OBJ_STAGE_BOSS_TELEPORTER,
    OBJ_DOOR_AREA_LINK,
    OBJ_ABILITY_ITEM,
    OBJ_DOOR_KEY,
    OBJ_DOOR_LOCKED,
    OBJ_TREASURE_CHEST,
    OBJ_GAME_ITEM_PICKEABLE,                                // the actions are determined by the sub-type. By default, does nothing, just stays there
    OBJ_GAME_ITEM_STATIC,
    OBJ_FRONT_DOOR_TELEPORTER,                              // like teleporter, but player needs to press up to activate
    OBJ_TYPE_PUSH_BOX,
    OBJ_TYPE_COUNT
};

enum COLlISION_TYPES {
    BLOCK_UNBLOCKED,
    BLOCK_X,
    BLOCK_Y,
    BLOCK_XY,
    BLOCK_STAIR_X,
    BLOCK_STAIR_Y,
    BLOCK_WATER,
    BLOCK_MOVE_LEFT,
    BLOCK_MOVE_RIGHT,
    BLOCK_INSIDE_OBJ
};



enum e_energy_types {
    ENERGY_TYPE_HP,
    ENERGY_TYPE_WEAPON ,
    ENERGY_TYPE_COUNT
};

enum collision_modes {
    COLlISION_MODE_XY,
    COLlISION_MODE_X,
    COLlISION_MODE_Y
};

enum e_LANGUAGES {
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_SPANISH,
    LANGUAGE_ITALIAN,
    LANGUAGE_PORTUGUESE,
    LANGUAGE_COUNT
};

enum e_DIFFICULTY_MODES {
    DIFFICULTY_MODE_GREATER,
    DIFFICULTY_MODE_EQUAL,
    DIFFICULTY_MODE_COUNT
};

enum E_DIFFICULTY_MODES  {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_COUNT
};

enum joystick_input_types {
    JOYSTICK_INPUT_TYPE_NONE,
    JOYSTICK_INPUT_TYPE_BUTTON,
    JOYSTICK_INPUT_TYPE_HAT,
    JOYSTICK_INPUT_TYPE_AXIS,
    JOYSTICK_INPUT_TYPE_COUNT
};

enum E_INPUT_TYPES {
    INPUT_TYPE_DOUBLE,
    INPUT_TYPE_KEYBOARD,
    INPUT_TYPE_JOYSTICK
};

enum e_PERFORMANCE_MODES {
    PERFORMANCE_MODE_LOW,
    PERFORMANCE_MODE_NORMAL,
    PERFORMANCE_MODE_HIGH,
    PERFORMANCE_MODE_COUNT
};

enum E_INPUT_MODES {
    INPUT_MODE_DOUBLE, // uses both keyboard and joystick
    INPUT_MODE_ANALOG,
    INPUT_MODE_DIGITAL
};

enum E_PLATFORM {
    PLATFORM_PS2,
    PLATFORM_WII,
    PLATFORM_PSP,
    PLATFORM_LINUX,
    PLATFORM_WINDOWS,
    PLATFORM_DINGUX,
    PLATFORM_PANDORA,
    PLATFORM_DREAMCAST,
    PLATFORM_RASPBERRY,
    PLATFORM_COUNT
};


/**
 * @brief
 *
 */
enum INPUT_COMMANDS {
    BTN_UP,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_JUMP,
    BTN_ATTACK,
    BTN_QUIT,
    BTN_START,
    BTN_L,
    BTN_R,
    BTN_DASH,
    BTN_ITEM,
    BTN_JUMP_TIMER,
    BTN_FULL_SCREEN,
    BTN_COUNT
};


enum ANIM_DIRECTION {
    ANIM_DIRECTION_LEFT,
    ANIM_DIRECTION_RIGHT,
    ANIM_DIRECTION_UP,
    ANIM_DIRECTION_DOWN,
    ANIM_DIRECTION_UP_LEFT,
    ANIM_DIRECTION_UP_RIGHT,
    ANIM_DIRECTION_DOWN_LEFT,
    ANIM_DIRECTION_DOWN_RIGHT,
    ANIM_DIRECTION_COUNT
};

enum e_LIGHT_POINT_COLOR {
    LIGHT_POINT_COLOR_WHITE,
    LIGHT_POINT_COLOR_YELLOW,
    LIGHT_POINT_COLOR_RED,
    LIGHT_POINT_COLOR_COUNT
};


enum PROJECTILE_TRAJECTORIES {
    TRAJECTORY_LINEAR,
    TRAJECTORY_ARC,
    TRAJECTORY_SIN,
    TRAJECTORY_CHAIN,
    TRAJECTORY_QUAKE,
    TRAJECTORY_BOMB,
    TRAJECTORY_FOLLOW,                          // follow enemy
    TRAJECTORY_PROGRESSIVE,
    TRAJECTORY_FREEZE,
    TRAJECTORY_DIAGONAL_UP,
    TRAJECTORY_DIAGONAL_DOWN,
    TRAJECTORY_CENTERED,                        // centered around character
    TRAJECTORY_ZIGZAG,                          // linear until reach a wall, then return (repeat until reflection number = 3)
    TRAJECTORY_TARGET_DIRECTION,                // adjust to linear, diagonal up or diagonal down depending on player position when shoot
    TRAJECTORY_ARC_TO_TARGET,                   // forms an arn that will end at player's position
    TRAJECTORY_TARGET_EXACT,                    // will go exactly to the point the target is in
    TRAJECTORY_FALL_BOMB,                       // falls until ground then explodes
    TRAJECTORY_LASER,
    TRAJECTORY_PUSH_BACK,                       // pushes player or enemies into opposite direction
    TRAJECTORY_ARC_SMALL,
    TRAJECTORY_RING,                            // increases size and on last frame, oponent can pass in the middle
    TRAJECTORY_LIGHTING,                        // appears sequentially in some parts of the screen
    TRAJECTORY_SPIRAL,
    TRAJECTORY_BOUNCING,
    TRAJECTORY_INVERSE_LINEAR,
    TRAJECTORY_DOUBLE_LINEAR,                   // simultaneous shots left/right
    TRAJECTORY_DOUBLE_DIAGONAL,                 // imperfect diagonal (only 45 degrees)
    TRAJECTORY_BOMB_RAIN,                       // creates small drop-bombs progressively
    TRAJECTORY_LARGE_BEAM,
    TRAJECTORY_PULL,                            // pull player or enemies into shooter
    TRAJECTORY_SLASH,                           // stay in place, runs until animation finishes
    PROJECTILE_TRAJECTORIES_COUNT
};


enum ANIM_TYPE {
    ANIM_TYPE_STAND,
    ANIM_TYPE_WALK,
    ANIM_TYPE_JUMP,
    ANIM_TYPE_ATTACK,
    ANIM_TYPE_TELEPORT,
    ANIM_TYPE_JUMP_ATTACK,
    ANIM_TYPE_WALK_ATTACK,
    ANIM_TYPE_HIT,
    ANIM_TYPE_STAIRS,
    ANIM_TYPE_STAIRS_SEMI,
    ANIM_TYPE_STAIRS_ATTACK,
    ANIM_TYPE_STAIRS_MOVE,
    ANIM_TYPE_THROW,
    ANIM_TYPE_TURN,
    ANIM_TYPE_MOVE_SEMI, // like turn, the start of the movement
    ANIM_TYPE_WALK_AIR, // for those that can walk on air
    ANIM_TYPE_INTRO, // for bosses
    ANIM_TYPE_SPECIAL_ATTACK, // for bosses, charging weapon, etc
    ANIM_TYPE_SLIDE,
    ANIM_TYPE_SHIELD,
    ANIM_TYPE_VERTICAL_TURN,
    ANIM_TYPE_ATTACK_THROW,
    ANIM_TYPE_ATTACK_DIAGONAL_UP,
    ANIM_TYPE_ATTACK_DIAGONAL_DOWN,
    ANIM_TYPE_ATTACK_UP,
    ANIM_TYPE_ATTACK_DOWN,
    ANIM_TYPE_ATTACK_SPECIAL, // (hadouken, shouryuken, etc)
    ANIM_TYPE_LOOK_UP,
    ANIM_TYPE_GRAB_WALL,
    ANIM_TYPE_HIT_SPECIAL,
    ANIM_TYPE_GOT_WEAPON,
    ANIM_TYPE_HOLDING_HAND,
    // placeholders for future use
    ANIM_TYPE_EFFECT_FROZE,
    ANIM_TYPE_EFFECT_BURN,
    ANIM_TYPE_EFFECT_SHOCK,
    ANIM_TYPE_GOT_ITEM,
    ANIM_TYPE_PLACEHOLDER4,
    ANIM_TYPE_PLACEHOLDER5,
    ANIM_TYPE_PLACEHOLDER6,
    ANIM_TYPE_PLACEHOLDER7,
    ANIM_TYPE_PLACEHOLDER8,
    ANIM_TYPE_PLACEHOLDER9,
    ANIM_TYPE_COUNT
};

#define AI_MAX_STATES 20

enum AI_REACTIONS {
    AI_REACTION_PLAYER_ON_RANGE,            // player is on range-distance
    AI_REACTION_HIT,                        // npc was damaged
    AI_REACTION_DEAD,                       // npc was killed
    AI_REACTION_PLAYER_SAME_Y,              // player on same y (tolerance=tilesize)
    AI_REACTION_PLAYER_CLOSE,               // player on a close distance (1/4 range)
    AI_REACTION_TBD,                        // to be later determined
    MAX_AI_REACTIONS

};

enum E_STRINGS_INGAME {
    strings_ingame_newgame,
    strings_ingame_loadgame,
    strings_ingame_config,
    strings_ingame_about,
    strings_ingame_extras,
    strings_ingame_password,
    strings_ingame_gameover,
    strings_ingame_and,
    strings_ingame_yougot_singular,
    strings_ingame_yougot_plural,
    strings_ingame_selectgame,
    strings_ingame_savegameerror1,
    strings_ingame_savegameerror2,
    strings_ingame_savegameerror3,
    strings_ingame_copyrightline,
    strings_ingame_audio,
    strings_ingame_input,
    strings_ingame_video,
    strings_ingame_leavestage,
    strings_ingame_language,
    strings_ingame_video_windowed,
    strings_ingame_video_fullscreen,
    strings_ingame_video_noscale,
    strings_ingame_video_size2x,
    strings_ingame_video_scale2x,
    strings_ingame_video_show_fps,
    strings_ingame_config_restart1,
    strings_ingame_config_restart2,
    strings_ingame_config_restart3,
    strings_ingame_config_presstorestart,
    strings_ingame_pressanykey,
    strings_ingame_config_set,
    strings_ingame_config_key_up,
    strings_ingame_config_key_down,
    strings_ingame_config_key_left,
    strings_ingame_config_key_right,
    strings_ingame_config_key_jump,
    strings_ingame_config_key_dash,
    strings_ingame_config_key_shield,
    strings_ingame_config_key_fire,
    strings_ingame_config_key_start,
    strings_ingame_config_key_keyl,
    strings_ingame_config_key_keyr,
    strings_ingame_config_key_pressnew,
    strings_ingame_config_key_reset,
    strings_ingame_config_key_directional,
    strings_ingame_config_key_directional_analog,
    strings_ingame_config_key_directional_digital,

    strings_ingame_config_android_cloud_save1,
    strings_ingame_config_android_cloud_save2,
    strings_ingame_config_android_cloud_save3,
    strings_ingame_config_android_cloud_save4,

    strings_ingame_config_android_play_services1,
    strings_ingame_config_android_play_services2,
    strings_ingame_config_android_play_services3,
    strings_ingame_config_android_play_services4,

    strings_ingame_config_android_hide_controls1,
    strings_ingame_config_android_hide_controls2,
    strings_ingame_config_android_hide_controls3,

    strings_ingame_config_extras_music_player,
    strings_ingame_config_extras_sound_player,
    strings_ingame_config_extras_art_gallery,


    strings_ingame_passwordinvalid,
    strings_ingame_pressstart,
    strings_ingame_engineerror,
    strings_ingame_nogames,
    strings_ingame_quitgame,
    strings_ingame_yes,
    strings_ingame_no,
    strings_ingame_life,
    strings_ingame_item,
    strings_ingame_coin,
    strings_ingame_gotarmor_type_ability,
    strings_ingame_gotarmor_type_arms,
    strings_ingame_gotarmor_type_arms_msg,
    strings_ingame_gotarmor_type_legs,
    strings_ingame_gotarmor_type_legs_msg1,
    strings_ingame_gotarmor_type_legs_msg2,
    strings_ingame_gotarmor_type_legs_msg3,
    strings_ingame_gotarmor_type_legs_msg4,
    strings_ingame_gotarmor_type_body,
    strings_ingame_gotarmor_type_body_msg1,
    strings_ingame_gotarmor_type_body_msg2,
    strings_ingame_gotarmor_type_body_msg3,
    strings_ingame_gotarmor_type_body_msg4,

    strings_ingame_difficulty_select,
    strings_ingame_difficulty_easy,
    strings_ingame_difficulty_normal,
    strings_ingame_difficulty_hard,

    strings_ingame_config_input_selected_joystick,
    strings_ingame_config_input_buttons,
    strings_ingame_config_input_turbo_mode,
    strings_ingame_config_input_autocharge_mode,

    strings_ingame_config_on,
    strings_ingame_config_off,
    strings_ingame_config_quitgame,
    strings_ingame_armor,
    strings_ingame_config_audio_volume_music,
    strings_ingame_config_audio_volume_sfx,
    strings_ingame_config_enabled,
    strings_ingame_config_disabled,

    strings_ingame_ready_message,

    strings_ingame_config_select_player,

    strings_ingame_config_press_start_to_select,

    strings_ingame_manual,
    strings_ingame_mode,
    strings_ingame_video_scale_mode,

    strings_config_android_hidescreencontrols,
    strings_config_android_screencontrolssize,
    strings_config_android_useplayservices,
    strings_config_android_usecloudsave,
    strings_config_wii_joysticktype,

    strings_config_android_screencontrolssize_SMALL,
    strings_config_android_screencontrolssize_MEDIUM,
    strings_config_android_screencontrolssize_BIG,
    strings_config_wii_joysticktype_WIIMOTE,
    strings_config_wii_joysticktype_CLASSIC,
    strings_config_wii_joysticktype_GAMECUBE,

    strings_config_wii_platformspecific,

    strings_config_keys_unet,
    strings_config_return,

    strings_config_low,
    strings_config_medium,
    strings_config_high,

    strings_ingame_config_graphics_performance,


    strings_ingame_enable_playservices_dialog,
    strings_ingame_enable_cloudsave_dialog,
    strings_ingame_requires_network,

    string_stage_select_stage,
    string_stage_select_lair_of,
    string_stage_select_enter_stage,
    string_stage_select_unavailable,

    string_intro_presents,
    string_intro_upperland_studios,
    string_intro_engine1,
    string_intro_engine2,
    string_intro_engine3,
    string_intro_engine4,
    string_intro_engine5,
    string_intro_engine6,
    string_intro_engine7,
    string_intro_engine8,

    string_intro_demo_warning_title,
    string_intro_demo_warning1,
    string_intro_demo_warning2,
    string_intro_demo_warning3,
    string_intro_demo_warning4,
    string_intro_demo_warning5,
    string_intro_demo_warning6,
    string_intro_demo_warning7,
    string_intro_demo_warning8,
    string_intro_demo_warning9,
    string_intro_demo_warning10,
    string_intro_demo_warning11,
    string_press_key_or_button,

    STRING_ENDING_CONCEPT,
    STRING_ENDING_DESIGN,

    strings_ingame_COUNT
};

// === EDITOR === //
enum TERRAIN_TYPES {
    TERRAIN_UNBLOCKED,
    TERRAIN_SOLID,
    TERRAIN_STAIR,
    TERRAIN_SPIKE,
    TERRAIN_WATER,
    TERRAIN_ICE,
    TERRAIN_MOVE_LEFT,
    TERRAIN_MOVE_RIGHT,
    TERRAIN_SAND,
    TERRAIN_HSCROLL_LOCK,
    TERRAIN_VSCROLL_LOCK,
    TERRAIN_SLOPE,
    TERRAIN_SCROLL_LOCK,      // locks the scrolling but player can cross it (use with caution)
    TERRAIN_TYPES_COUNT
};

enum AI_ACTION_LIST {
    AI_ACTION_WALK,
    AI_ACTION_FLY,
    AI_ACTION_JUMP,
    AI_ACTION_WAIT_UNTIL_PLAYER_IS_IN_RANGE,
    AI_ACTION_SAVE_POINT,
    AI_ACTION_SHOT_PROJECTILE_AHEAD,
    AI_ACTION_SHOT_PROJECTILE_PLAYER_DIRECTION,
    AI_ACTION_SHOT_PROJECTILE_INVERT_DIRECTION,
    AI_ACTION_AIR_WALK,
    AI_ACTION_FALL_TO_GROUND,
    AI_ACTION_TELEPORT,
    AI_ACTION_DASH,
    AI_ACTION_GRAB_WALL,
    AI_ACTION_SPAWN_NPC,
    AI_ACTION_CHANGE_MOVE_TYPE,
    AI_ACTION_REPLACE_NPC,                          // similar to spawn npc, but removes itself
    AI_ACTION_CIRCLE_PLAYER,
    AI_ACTION_CHANGE_MOVE_TYPE_REVERSE,
    AI_ACTION_JUMP_ATTACK_UP,
    AI_ACTION_JUMP_ATTACK_AHEAD_ONCE,
    AI_ACTION_WAIT_RANDOM_TIME,
    AI_ACTION_MORPH_INTO_NPC,                       // instead of a new enemy with full hp and such, just morph into a new one
    AI_ACTION_PLAY_SFX,                             // play a sound-effect
    AI_ACTION_LIST_SIZE
};


enum AI_ACTION_SHOT_OPTIONS {
    AI_ACTION_SHOT_FACEPLAYER,
    AI_ACTION_SHOT_AHEAD
};


enum AI_ACTION_GOTO_LIST {
    AI_ACTION_GOTO_CHANCE,
    AI_ACTION_GOTO_1,
    AI_ACTION_GOTO_2,
    AI_ACTION_GOTO_3,
    AI_ACTION_GOTO_4
};

enum EDITOR_MODES {
    EDITMODE_NORMAL,
    EDITMODE_LOCK,
    EDITMODE_ERASER,
    EDITMODE_FILL,
    EDITMODE_LINK,
    EDITMODE_LINK_DEST,
    EDITMODE_NPC,
    EDITMODE_ADDNPC,
    EDITMODE_OBJECT,
    EDITMODE_SET_BOSS,
    EDITMODE_SET_SUBBOSS,
    EDITMODE_ANIM_TILE,
    EDITMODE_OBJECT_LINK_PLACING,
    EDITMODE_SELECT,
    EDITMODE_PASTE,
    EDITMODE_SLOPE,
    EDITMODE_COUNT
};

enum e_TILE_TYPE {
    TILE_TYPE_SOLID,
    TILE_TYPE_ANIM,
    TILE_TYPE_SLOPE,
    TILE_TYPE_UNUSED,
    TILE_TYPE_COUNT
};


enum STAGE_SELECT_EDIT_MODES {
    STAGE_SELECT_EDIT_MODE_LOCKED,
    STAGE_SELECT_EDIT_MODE_PATH,
    STAGE_SELECT_EDIT_MODE_STAGE,
    STAGE_SELECT_EDIT_MODE_CASTLE,
    STAGE_SELECT_EDIT_MODE_COUNT
};


enum e_SCREEN_GFX_MODE {
    SCREEN_GFX_MODE_FULLMAP,
    SCREEN_GFX_MODE_BACKGROUND,
    SCREEN_GFX_MODE_OVERLAY,
    SCREEN_GFX_MODE_COUNT
};

enum e_INPUT_IMAGES {
    INPUT_IMAGES_DPAD_LEFTRIGHT,
    INPUT_IMAGES_A,
    INPUT_IMAGES_B,
    INPUT_IMAGES_X,
    INPUT_IMAGES_Y,
    INPUT_IMAGES_R,
    INPUT_IMAGES_START,
    INPUT_IMAGES_DPAD_UP,
    INPUT_IMAGES_DPAD_DOWN,
    INPUT_IMAGES_DPAD_LEFT,
    INPUT_IMAGES_DPAD_RIGHT,
    INPUT_IMAGES_COUNT
};

enum e_SCREEN_GFX {
    SCREEN_GFX_NONE,
    SCREEN_GFX_RAIN,
    SCREEN_GFX_SNOW,
    SCREEN_GFX_WATER,
    SCREEN_GFX_TRAIN,
    SCREEN_GFX_FLASH,
    SCREEN_GFX_LIGHTINGBOLT,
    SCREEN_GFX_SHADOW_TOP,
    SCREEN_GFX_INFERNO,
    SCREEN_GFX_COUNT
};

enum e_LINK_DIRECTION {
    LINK_DIRECTION_VERTICAL,
    LINK_DIRECTION_HORIZONTAL,
    LINK_DIRECTION_COUNT
};

enum e_LINK_TYPES {
    LINK_HORIZONTAL,
    LINK_VERTICAL,
    LINK_TELEPORTER,
    LINK_FADE_TELEPORT,
    LINK_TELEPORT_LEFT_LOCK,
    LINK_TELEPORT_RIGHT_LOCK,
    LINK_TYPE_COUNT
};

enum e_GAME_INTERRUPT_MODE {
    GAME_INTERRUPT_MODE_NONE,
    GAME_INTERRUPT_MODE_FULL,               // everything can't move
    GAME_INTERRUPT_MODE_PLAYER,             // player can't move
    GAME_INTERRUPT_MODE_COUNT
};

// enum that stores types for objects that need a key
// the sub-type determines what the object will do when the key is used on it
// only a few are meant to be generic, each other should describe a different behavior
enum e_OBJ_SUB_TYPE {
    OBJ_SUB_TYPE_NONE,
    OBJ_SUB_TYPE_BLOCKED_CAVERN,            // needs dynamite, will explode and disappear
    OBJ_SUB_TYPE_BLOCKED_DOOR,              // opens with a given key
    OBJ_SUB_TYPE_ELEVATOR,                  // can be activated once key is used

    OBJ_SUB_TYPE_TNT,
    OBJ_SUB_TYPE_COUNT,
};


#endif // DEFINES_H
