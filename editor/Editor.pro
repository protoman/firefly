# -------------------------------------------------
# Project created by QtCreator 2009-12-03T12:16:29
# -------------------------------------------------

#CONFIG += win32
CONFIG += linux
#CONFIG += macosx

QT += widgets

QMAKE_CCFLAGS += -fpermissive
QMAKE_CXXFLAGS += -fpermissive


macosx {
    QMAKE_LIBS_QT =
    QMAKE_LIBS_QT_THREAD =

    QMAKE_CXX = /usr/local/bin/g++-4.8
    QMAKE_LINK = /usr/local/bin/g++-4.8
    QMAKE_CXXFLAGS += -I/usr/local/Trolltech/Qt-4.8.6/include -I/usr/local/Trolltech/Qt-4.8.6/include/QtGui -DMACOSX


    QT = core gui
    LIBS += -static-libgcc -static-libstdc++ /usr/local/Trolltech/Qt-4.8.6/lib/libQtCore.a /usr/local/Trolltech/Qt-4.8.6/lib/libQtGui.a -lz -framework Carbon
    QMAKE_LFLAGS -= -framework QtGui
    QMAKE_CFLAGS_PPC_64             -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_OBJECTIVE_CFLAGS_PPC_64   -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_CFLAGS_X86_64             -= -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_OBJECTIVE_CFLAGS_X86_64   -= -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_CXXFLAGS_PPC_64           -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_CXXFLAGS_X86_64           -= -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_LFLAGS_PPC_64             -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_LFLAGS_X86_64             -= -Xarch_x86_64 -mmacosx-version-min=10.5
}

linux {
    QMAKE_CXXFLAGS += -std=c++0x -g -I/usr/include/SDL2
    INCLUDES = -I/usr/include/SDL2
    CONFIG += console
}

win32 {
    QMAKE_CCFLAGS += -std=c++0x -fpermissive
    QMAKE_CXXFLAGS += -std=c++0x -fpermissive
    CXXFLAGS += -std=c++0x
    LIBS = -lmingw32 -mwindows -lqtmaind
    QT += core gui
    QMAKE_CCFLAGS += -DWIN32
    QMAKE_CXXFLAGS += -DWIN32
    CXXFLAGS += -DWIN32
    CONFIG -= console
}


TARGET = ../build/editor
TEMPLATE = app
SOURCES += main.cpp \
    ../aux_tools/ability_name.cpp \
    ../game_data.cpp \
    aux_tools/dataconverter.cpp \
    dialogs/npcdialogeditor.cpp \
    mainwindow.cpp \
    editorarea.cpp \
    editortilepallete.cpp \
    mainwindow_tab/areaedittab.cpp \
    mainwindow_tab/enemy_edit.cpp \
    mainwindow_tab/npcedittab.cpp \
    mainwindow_tab/roomstyle.cpp \
    mainwindow_tab/text/dialogeditform.cpp \
    mainwindow_tab/text/model/dialogtableitemmodel.cpp \
    mediator.cpp \
    addwizard.cpp \
    loadgamepicker.cpp \
    dialognpcedit.cpp \
    dialogobjectedit.cpp \
    projectilepreviewarea.cpp \
    player_preview_area.cpp \
    common.cpp \
    mainwindow_tab/object_tab.cpp \
    sprite_preview_area.cpp \
    mainwindow_tab/artificial_inteligence_tab.cpp \
    mainwindow_tab/projectile_edit.cpp \
    mainwindow_tab/map_tab.cpp \
    ../file/convert.cpp \
    mainwindow_tab/player_edit.cpp \
    scenes/sceneeditorwindow.cpp \
    scenes/tab_text.cpp \
    mainwindow_tab/animtitle.cpp \
    aboutwindow.cpp \
    scenes/tab_image.cpp \
    scenes/imageareapreview.cpp \
    scenes/scenesmediator.cpp \
    scenes/tab_viewpoint.cpp \
    scenes/textpreviewarea.cpp \
    scenes/tab_scenelist.cpp \
    scenes/model/objectlistmodel.cpp \
    scenes/comboboxdelegate.cpp \
    scenes/tab_sfx.cpp \
    scenes/tab_music.cpp \
    scenes/tab_cleararea.cpp \
    scenes/tab_animation.cpp \
    scenes/animation_previewarea.cpp \
    files_editor/fileseditor.cpp \
    files_editor/image_directories_model.cpp \
    model/combolistdelegate.cpp \
    model/buttondelegate.cpp \
    newgamedialog.cpp \
    files_editor/stringseditor.cpp \
    ../aux_tools/stringutils.cpp \
    model/stringeditmodel.cpp \
    mainwindow_tab/anim_tiles_edit.cpp \
    framespreviewarea.cpp \
    widgets/animtilepalette.cpp \
    mainwindow_tab/anim/animpackimport.cpp \
    dialog_pick_color.cpp \
    files_editor/gametextcreditstab.cpp \
    ../aux_tools/exception_manager.cpp \
    mainwindow_tab/game_properties_tab.cpp \
    mainwindow_tab/animtileedit.cpp \
    ../file/fio_common.cpp \
    ../file/fio_scenes.cpp \
    ../file/fio_strings.cpp \
    ../file/file_io.cpp \
    ../data/shareddata.cpp \
    mainwindow_tab/slopeedittab.cpp \
    widgets/areaeditpreviewwidget.cpp \
    widgets/npcpreviewarea.cpp \
    widgets/roomstylepreviewarea.cpp \
    widgets/slopepreviewwidget.cpp \
    widgets/slopepickerwidget.cpp \
    widgets/worldmapeditorArea.cpp

HEADERS += mainwindow.h \
    ../aux_tools/ability_name.h \
    ../file/v0/file_game_v0.h \
    ../game_data.h \
    aux_tools/dataconverter.h \
    dialogs/npcdialogeditor.h \
    editorarea.h \
    editortilepallete.h \
    mainwindow_tab/areaedittab.h \
    mainwindow_tab/enemy_edit.h \
    mainwindow_tab/npcedittab.h \
    mainwindow_tab/roomstyle.h \
    mainwindow_tab/text/dialogeditform.h \
    mainwindow_tab/text/model/dialogtableitemmodel.h \
    mediator.h \
    addwizard.h \
    addwizard.h \
    loadgamepicker.h \
    dialognpcedit.h \
    dialogobjectedit.h \
    projectilepreviewarea.h \
    player_preview_area.h \
    common.h \
    mainwindow_tab/object_tab.h \
    sprite_preview_area.h \
    mainwindow_tab/artificial_inteligence_tab.h \
    mainwindow_tab/projectile_edit.h \
    enum_names.h \
    mainwindow_tab/game_properties_tab.h \
    mainwindow_tab/map_tab.h \
    ../file/convert.h \
    mainwindow_tab/player_edit.h \
    scenes/sceneeditorwindow.h \
    scenes/tab_text.h \
    mainwindow_tab/animtitle.h \
    ../defines.h \
    ../file/format/st_characterState.h \
    ../file/format/st_common.h \
    ../file/format/st_hitPoints.h \
    ../file/format/st_platform.h \
    ../file/format/st_projectile.h \
    ../file/format/st_teleporter.h \
    ../file/version.h \
    aboutwindow.h \
    scenes/tab_image.h \
    scenes/imageareapreview.h \
    scenes/scenesmediator.h \
    scenes/tab_viewpoint.h \
    scenes/textpreviewarea.h \
    scenes/tab_scenelist.h \
    scenes/model/objectlistmodel.h \
    scenes/comboboxdelegate.h \
    scenes/tab_sfx.h \
    scenes/tab_music.h \
    scenes/tab_cleararea.h \
    scenes/tab_animation.h \
    scenes/animation_previewarea.h \
    files_editor/fileseditor.h \
    files_editor/image_directories_model.h \
    model/combolistdelegate.h \
    model/buttondelegate.h \
    newgamedialog.h \
    ../file/v4/file_config_v4.h \
    ../file/v4/file_game_v4.h \
    ../file/v4/file_save_v4.h \
    ../file/v4/file_scene_v4.h \
    ../file/v4/file_stage_v4.h \
    ../file/v4/file_strings.h \
    files_editor/stringseditor.h \
    ../aux_tools/stringutils.h \
    model/stringeditmodel.h \
    ../file/v4/file_strings.h \
    mainwindow_tab/anim_tiles_edit.h \
    framespreviewarea.h \
    widgets/animtilepalette.h \
    mainwindow_tab/anim/animpackimport.h \
    dialog_pick_color.h \
    files_editor/gametextcreditstab.h \
    ../aux_tools/exception_manager.h \
    mainwindow_tab/animtileedit.h \
    ../file/v4/file_anim_block.h \
    ../file/v4/file_config_v4.h \
    ../file/v4/file_game_v4.h \
    ../file/v4/file_map.h \
    ../file/v4/file_save_v4.h \
    ../file/v4/file_scene_v4.h \
    ../file/v4/file_strings.h \
    ../file/fio_common.h \
    ../file/fio_scenes.h \
    ../file/fio_strings.h \
    ../data/shareddata.h \
    ../file/file_io.h \
    mainwindow_tab/slopeedittab.h \
    widgets/areaeditpreviewwidget.h \
    widgets/npcpreviewarea.h \
    widgets/roomstylepreviewarea.h \
    widgets/slopepreviewwidget.h \
    widgets/slopepickerwidget.h \
    widgets/worldmapeditorArea.h

FORMS += mainwindow.ui \
    addwizard.ui \
    dialogs/npcdialogeditor.ui \
    loadgamepicker.ui \
    dialognpcedit.ui \
    dialogobjectedit.ui \
    mainwindow_tab/areaedittab.ui \
    mainwindow_tab/enemy_edit.ui \
    mainwindow_tab/npcedittab.ui \
    mainwindow_tab/object_tab.ui \
    mainwindow_tab/roomstyle.ui \
    mainwindow_tab/text/dialogeditform.ui \
    mainwindow_tab/weapon_edit.ui \
    mainwindow_tab/artificial_inteligence_tab.ui \
    mainwindow_tab/projectile_edit.ui \
    mainwindow_tab/game_properties_tab.ui \
    mainwindow_tab/map_tab.ui \
    stage_swap_dialog.ui \
    mainwindow_tab/player_edit.ui \
    scenes/sceneeditorwindow.ui \
    scenes/tab_text.ui \
    mainwindow_tab/armor_edit.ui \
    aboutwindow.ui \
    scenes/tab_image.ui \
    scenes/tab_viewpoint.ui \
    scenes/tab_scenelist.ui \
    scenes/tab_sfx.ui \
    scenes/tab_music.ui \
    scenes/tab_cleararea.ui \
    scenes/tab_animation.ui \
    files_editor/fileseditor.ui \
    newgamedialog.ui \
    files_editor/stringseditor.ui \
    mainwindow_tab/anim_tiles_edit.ui \
    mainwindow_tab/anim/animpackimport.ui \
    dialog_pick_color.ui \
    files_editor/gametextcreditstab.ui \
    mainwindow_tab/slopeedittab.ui

RESOURCES += resources/icons/icons.qrc
INCLUDEPATH += ../common
INCLUDEPATH += ..
INCLUDEPATH += ./mainwindow_tab
OTHER_FILES += ../docs/RoadMap \
    ../docs/ChangeLog.txt






















