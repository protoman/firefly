TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QT       -= core
QT       -= gui
CONFIG -= linux # qtcreator adds linux even if shouldn't, so we remove

CONFIG += linux

linux {
    TARGET = fireflygame
    LIBS = -L/usr/X11R6/lib -lX11 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_gfx `sdl2-config --libs` -ldl

    INCLUDES = -I/usr/include/SDL2 \
            -I/usr/include \
            -I. \
            -I./include \
            -L/usr/lib
    QMAKE_CCFLAGS += -DLINUX -DPC -Wno-reorder -Wno-ignored-qualifiers -fpermissive -Werror=return-type
    QMAKE_CXXFLAGS += -DLINUX -DPC -Wno-reorder -Wno-ignored-qualifiers -fpermissive -Werror=return-type -I/usr/include/SDL2
}


SOURCES += \
    GameManager.cpp \
    aux_tools/ability_name.cpp \
    character/GameEnemy.cpp \
    character/GameNpc.cpp \
    character/custom/ai_boss_001.cpp \
    character/gamenpc.cpp \
    character/movement/moveslopes.cpp \
    game_data.cpp \
        main.cpp \
    text/i18ntext.cpp \
    text/npc_dialog_manager.cpp \
    view/ingame_presentation.cpp \
    view/textview.cpp \
    view/imageview.cpp \
    data/shareddata.cpp \
    view/soundview.cpp \
    controller/inputcontroller.cpp \
    view/timerview.cpp \
    aux_tools/exception_manager.cpp \
    aux_tools/fps_control.cpp \
    aux_tools/stringutils.cpp \
    aux_tools/trajectory_parabola.cpp \
    file/fio_common.cpp \
    file/file_io.cpp \
    file/fio_scenes.cpp \
    file/fio_strings.cpp \
    controller/mapcontroller.cpp \
    view/animation.cpp \
    collision_detection.cpp \
    character/movement/inertia.cpp \
    character/movement/jump.cpp \
    character/artificial_inteligence.cpp \
    character/character.cpp \
    character/character_animation.cpp \
    character/classplayer.cpp \
    view/draw.cpp \
    view/gfx_sin_wave.cpp \
    view/option_picker.cpp \
    projectilelib.cpp \
    sceneslib.cpp \
    strings_map.cpp \
    scenes/game_menu.cpp \
    class_config.cpp \
    options/key_map.cpp \
    ports/android/android_game_services.cpp \
    objects/GameObject.cpp \
    scenes/dialogs.cpp \
    options/pausemenu.cpp

HEADERS += \
    GameManager.h \
    aux_tools/ability_name.h \
    character/GameEnemy.h \
    character/GameNpc.h \
    character/custom/ai_boss_001.h \
    character/gamenpc.h \
    character/movement/moveslopes.h \
    file/v0/file_area_v0.h \
    file/v4/file_save_v4.h \
    file/v5/struct_file_game_area_map.h \
    file/v6/file_area_v6.h \
    file/v6/file_game_object_state.h \
    file/v6/file_level_v6.h \
    file/v6/file_npc_state_v6.h \
    file/v6/file_object_v6.h \
    file/v6/file_room_v6.h \
    file/v6/file_style_v6.h \
    file/v6/file_v6_quest.h \
    game_data.h \
    text/i18ntext.h \
    text/npc_dialog_manager.h \
    view/ingame_presentation.h \
    view/textview.h \
    view/imageview.h \
    defines.h \
    data/shareddata.h \
    data/st_common.h \
    view/soundview.h \
    file/v4/file_config_v4.h \
    file/v4/file_game_v4.h \
    controller/inputcontroller.h \
    view/timerview.h \
    aux_tools/exception_manager.h \
    aux_tools/fps_control.h \
    aux_tools/stringutils.h \
    aux_tools/trajectory_parabola.h \
    file/fio_common.h \
    file/file_io.h \
    file/fio_scenes.h \
    file/fio_strings.h \
    file/v4/file_scene_v4.h \
    file/v4/file_anim_block.h \
    controller/mapcontroller.h \
    view/animation.h \
    collision_detection.h \
    character/movement/inertia.h \
    character/movement/jump.h \
    character/artificial_inteligence.h \
    character/character.h \
    character/character_animation.h \
    character/classplayer.h \
    view/draw.h \
    view/gfx_sin_wave.h \
    view/option_picker.h \
    projectilelib.h \
    sceneslib.h \
    strings_map.h \
    file/format/st_characterState.h \
    file/format/st_platform.h \
    file/format/st_projectile.h \
    file/format/st_teleporter.h \
    file/format/st_hitPoints.h \
    scenes/game_menu.h \
    class_config.h \
    options/key_map.h \
    ports/android/android_game_services.h \
    objects/GameObject.h \
    scenes/dialogs.h \
    options/pausemenu.h \
    file/v5/struct_file_map.h

DISTFILES += \
    docs/TODO \
    docs/planning_characters.txt \
    docs/planning_objects.txt
