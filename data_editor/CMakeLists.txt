cmake_minimum_required(VERSION 3.16)

project(FireflyEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        enemy/enemyedit.cpp
        enemy/enemyedit.h
        enemy/enemyedit.ui
        widgets/mapareawidget.cpp
        widgets/mapareawidget.h
        sub_windows/object/objectedit.h
        sub_windows/object/objectedit.cpp
        widgets/enemygraphicpreviewareawidget.h
        widgets/enemygraphicpreviewareawidget.cpp
        ../tiled/TiledWorldLoader.cpp
        ../tiled/TiledWorldLoader.hpp
        ../defines.h
        ../data/model/object_data.cpp
        ../data/model/object_data.h
        ../data/model/platform_object_data.cpp
        ../data/model/platform_object_data.h
        ../data/model/quest_object_data.cpp
        ../data/model/quest_object_data.h
        ../data/model/recovery_object_data.cpp
        ../data/model/recovery_object_data.h
        ../data/model/util_object_data.cpp
        ../data/model/util_object_data.h
        util/CommonUtils.cpp
        util/CommonUtils.hpp
)

set(WIDGETS_INCLUDE_DIR "widgets")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FireflyEditor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        icons.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FireflyEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FireflyEditor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FireflyEditor
            ${PROJECT_SOURCES}
                icons.qrc
                widgets/enemygraphicpreviewareawidget.h widgets/enemygraphicpreviewareawidget.cpp
                quest/questeditwindow.h quest/questeditwindow.cpp quest/questeditwindow.ui
                docs/TODO.txt
                sub_windows/object/objectedit.h sub_windows/object/objectedit.cpp sub_windows/object/objectedit.ui
                docs/ai_instructions.txt
        )

    endif()
endif()

target_include_directories(FireflyEditor PRIVATE .)
target_include_directories(FireflyEditor PRIVATE ..)
target_include_directories(FireflyEditor PRIVATE widgets)

if(LINUX)
    target_link_directories(FireflyEditor PRIVATE
        /usr/local/lib/
    )
endif()

target_link_libraries(FireflyEditor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    tmxlite)
target_link_libraries(FireflyEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.npc)
endif()
set_target_properties(FireflyEditor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FireflyEditor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FireflyEditor)
endif()

add_custom_command(TARGET FireflyEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:FireflyEditor> ${CMAKE_SOURCE_DIR}/../build/
    COMMENT "Copying FireflyEditor to ${CMAKE_SOURCE_DIR}/../build/"
)
