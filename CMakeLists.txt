cmake_minimum_required(VERSION 3.16)

# Set policy CMP0135 to NEW for FetchContent timestamp handling
cmake_policy(SET CMP0135 NEW)

project(FireFlyGame VERSION 1.0 LANGUAGES C CXX)
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Find the SDL3 package
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

# Find Box2D
find_package(box2d CONFIG REQUIRED)

# Add tmxlite using find_package (vcpkg) or FetchContent (fallback)
find_package(tmxlite CONFIG QUIET)
if(NOT tmxlite_FOUND)
    message(STATUS "tmxlite not found, using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        tmxlite
        URL https://github.com/fallahn/tmxlite/archive/refs/tags/v1.4.4.tar.gz
        SOURCE_SUBDIR tmxlite
        PATCH_COMMAND sed -i "s/cmake_minimum_required(VERSION 3.1)/cmake_minimum_required(VERSION 3.5)/" tmxlite/CMakeLists.txt
    )
    FetchContent_MakeAvailable(tmxlite)
endif()

# Create an alias for tmxlite if it's not already namespaced
if(NOT TARGET tmxlite::tmxlite)
    add_library(tmxlite::tmxlite ALIAS tmxlite)
endif()

include_directories(${SDL3_INCLUDE_DIRS})

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

set(GAME_SOURCES
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
    character/movement/jump.cpp character/movement/jump.h
    character/movement/moveslopes.cpp character/movement/moveslopes.h
    class_config.cpp class_config.h
    collision_detection.cpp collision_detection.h
    controller/inputcontroller.cpp controller/inputcontroller.h
    controller/mapcontroller.cpp controller/mapcontroller.h
    data/model/object_data.cpp data/model/object_data.h
    data/model/recovery_object_data.cpp data/model/recovery_object_data.h
    data/model/platform_object_data.cpp data/model/platform_object_data.h
    data/model/util_object_data.cpp data/model/util_object_data.h
    data/model/quest_object_data.cpp data/model/quest_object_data.h
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
    tiled/TiledMapObjectLayer.cpp
    tiled/TiledMapObjectLayer.hpp
    data/SharedPlayerData.cpp
    data/SharedPlayerData.hpp
    tiled/TiledWorldLoader.cpp
    tiled/TiledWorldLoader.hpp
    file/data/map_elements.hpp
    file/data/file_data_enemy.hpp
)

add_library(FireFlyGameLib STATIC ${GAME_SOURCES})

add_executable(FireFlyGame main.cpp)

target_include_directories(FireFlyGameLib PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${SDL3_INCLUDE_DIRS}
)

if(WIN32)
    target_compile_definitions(FireFlyGameLib PRIVATE
        WINDOWS
        PC
        NOMINMAX
        _USE_MATH_DEFINES
    )

    target_compile_definitions(FireFlyGame PRIVATE
        NOMINMAX
        _USE_MATH_DEFINES
    )
endif()

if(LINUX)
    target_compile_definitions(FireFlyGameLib PRIVATE
        LINUX
        PC
    )

    target_link_libraries(FireFlyGameLib PUBLIC
        X11
        dl
    )

    target_compile_options(FireFlyGameLib PRIVATE -I/usr/local/include/SDL3 -Werror=return-type -Wno-ignored-qualifiers -Wno-reorder -fpermissive -g)
endif()

target_link_libraries(FireFlyGameLib PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    box2d::box2d
    tmxlite::tmxlite
)

target_link_libraries(FireFlyGame PRIVATE FireFlyGameLib)


install(TARGETS FireFlyGame
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Add GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

# Add the tests subdirectory AFTER googletest is available
add_subdirectory(tests)
