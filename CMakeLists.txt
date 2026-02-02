cmake_minimum_required(VERSION 3.16)
project(FireFlyGame VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Find the SDL3 package
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

add_executable(FireFlyGame
    GameManager.cpp GameManager.h
    aux_tools/ability_name.cpp aux_tools/ability_name.h
    aux_tools/exception_manager.cpp aux_tools/exception_manager.h
    aux_tools/fps_control.cpp aux_tools/fps_control.h
    aux_tools/stringutils.cpp aux_tools/stringutils.h
    aux_tools/trajectory_parabola.cpp aux_tools/trajectory_parabola.h
    box2d/Box2dManager.cpp box2d/Box2dManager.h
    box2d/box2ddebugdraw.cpp box2d/box2ddebugdraw.h
    character/GameEnemy.cpp character/GameEnemy.h
    character/GameNpc.cpp character/GameNpc.h
    character/artificial_inteligence.cpp character/artificial_inteligence.h
    character/character.cpp character/character.h
    character/character_animation.cpp character/character_animation.h
    character/classplayer.cpp character/classplayer.h
    character/custom/ai_boss_001.cpp character/custom/ai_boss_001.h
    character/gamenpc.cpp character/gamenpc.h
    character/movement/inertia.cpp character/movement/inertia.h
    character/movement/jump.cpp character/movement/jump.h
    character/movement/moveslopes.cpp character/movement/moveslopes.h
    class_config.cpp class_config.h
    collision_detection.cpp collision_detection.h
    controller/inputcontroller.cpp controller/inputcontroller.h
    controller/mapcontroller.cpp controller/mapcontroller.h
    data/shareddata.cpp data/shareddata.h
    data/sharedmapdata.cpp data/sharedmapdata.h
    data/st_color.hpp
    data/st_common.h
    data/st_imageData.hpp
    defines.h
    file/file_io.cpp file/file_io.h
    file/fio_common.cpp file/fio_common.h
    file/fio_scenes.cpp file/fio_scenes.h
    file/fio_strings.cpp file/fio_strings.h
    file/format/st_characterState.h
    file/format/st_hitPoints.h
    file/format/st_platform.h
    file/format/st_projectile.h
    file/format/st_teleporter.h
    file/v0/file_area_v0.h
    file/v4/file_anim_block.h
    file/v4/file_config_v4.h
    file/v4/file_game_v4.h
    file/v4/file_save_v4.h
    file/v4/file_scene_v4.h
    file/v5/struct_file_game_area_map.h
    file/v5/struct_file_map.h
    file/v6/file_area_v6.h
    file/v6/file_game_object_state.h
    file/v6/file_level_v6.h
    file/v6/file_npc_state_v6.h
    file/v6/file_object_v6.h
    file/v6/file_room_v6.h
    file/v6/file_style_v6.h
    file/v6/file_v6_quest.h
    game_data.cpp game_data.h
    main.cpp
    objects/GameObject.cpp objects/GameObject.h
    options/key_map.cpp options/key_map.h
    options/pausemenu.cpp options/pausemenu.h
    ports/android/android_game_services.cpp ports/android/android_game_services.h
    projectilelib.cpp projectilelib.h
    scenes/dialogs.cpp scenes/dialogs.h
    scenes/game_menu.cpp scenes/game_menu.h
    sceneslib.cpp sceneslib.h
    strings_map.cpp strings_map.h
    text/i18ntext.cpp text/i18ntext.h
    text/npc_dialog_manager.cpp text/npc_dialog_manager.h
    tiled/TiledMap.cpp tiled/TiledMap.h
    tiled/TiledMapLayer.cpp tiled/TiledMapLayer.h
    tiled/TiledMapTexture.cpp tiled/TiledMapTexture.h
    view/animation.cpp view/animation.h
    view/draw.cpp view/draw.h
    view/gfx_sin_wave.cpp view/gfx_sin_wave.h
    view/imageview.cpp view/imageview.h
    view/ingame_presentation.cpp view/ingame_presentation.h
    view/option_picker.cpp view/option_picker.h
    view/soundview.cpp view/soundview.h
    view/textview.cpp view/textview.h
    view/timerview.cpp view/timerview.h
        data/SharedGameData.cpp
        data/SharedGameData.hpp
)

if(LINUX)
    target_compile_definitions(FireFlyGame PRIVATE
        LINUX
        PC
    )

    target_link_directories(FireFlyGame PRIVATE
        /usr/X11R6/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib/
    )

    target_link_libraries(FireFlyGame PRIVATE
        SDL3::SDL3
        # Remove flag: --libs`
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
        X11
        box2d
        dl
        tmxlite
    )

    target_compile_options(FireFlyGame PRIVATE -I/usr/local/include/SDL3 -Werror=return-type -Wno-ignored-qualifiers -Wno-reorder -fpermissive -g)
endif()

install(TARGETS FireFlyGame
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
