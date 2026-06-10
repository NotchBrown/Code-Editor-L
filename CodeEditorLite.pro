# CodeEditorLite Project
# Qt5.14.2 qmake project file

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeEditorLite
TEMPLATE = app

# Version
VERSION = 1.0.0

# QScintilla configuration
QSCINTILLA_PATH = $$PWD/lib/QScintilla/src
LIBS += -L$$QSCINTILLA_PATH/release -lqscintilla2_qt5
INCLUDEPATH += $$QSCINTILLA_PATH/Qsci

# Source files
SOURCES += \
    src/main.cpp \
    src/widget/main_window/main_window.cpp \
    src/widget/editor/code_editor.cpp \
    src/widget/navigator/navigator.cpp \
    src/widget/segment/segment.cpp \
    src/widget/errors_and_warnings/errors_and_warnings.cpp \
    src/widget/ipc_message/ipc_message.cpp \
    src/ipc/ipc_server.cpp \
    src/project/project_manager.cpp \
    src/function/start/on_start_new_instance.cpp \
    src/function/start/on_start_exit.cpp \
    src/function/file/on_file_new.cpp \
    src/function/file/on_file_open.cpp \
    src/function/file/on_file_save.cpp \
    src/function/file/on_file_save_as.cpp \
    src/function/file/on_file_properties.cpp \
    src/function/edit/on_edit_undo.cpp \
    src/function/edit/on_edit_redo.cpp \
    src/function/edit/on_edit_cut.cpp \
    src/function/edit/on_edit_copy.cpp \
    src/function/edit/on_edit_paste.cpp \
    src/function/edit/on_edit_delete.cpp \
    src/function/edit/on_edit_go_line.cpp \
    src/function/edit/on_edit_comment.cpp \
    src/function/edit/on_edit_uncomment.cpp \
    src/function/edit/on_edit_comment_segment.cpp \
    src/function/edit/on_edit_uncomment_segment.cpp \
    src/function/edit/on_edit_find.cpp \
    src/function/edit/on_edit_find_next.cpp \
    src/function/edit/on_edit_find_prev.cpp \
    src/function/edit/on_edit_replace.cpp \
    src/function/mark/on_mark_go_error.cpp \
    src/function/mark/on_mark_delete_error.cpp \
    src/function/mark/on_mark_clean_error.cpp \
    src/function/mark/on_mark_go_bookmark.cpp \
    src/function/mark/on_mark_create_bookmark.cpp \
    src/function/mark/on_mark_manage_bookmark.cpp \
    src/function/mark/on_mark_create_break.cpp \
    src/function/mark/on_mark_remove_break.cpp \
    src/function/settings/on_settings_general.cpp \
    src/function/settings/on_settings_editor.cpp \
    src/function/settings/on_settings_ipc.cpp \
    src/function/settings/on_settings_toolbar.cpp \
    src/function/settings/on_settings_hotkey.cpp \
    src/function/window/on_window_close_tab.cpp \
    src/function/window/on_window_close_all_saved.cpp \
    src/function/window/on_window_close_all.cpp \
    src/function/window/on_window_close_other.cpp \
    src/function/window/on_window_navigator.cpp \
    src/function/window/on_window_segment.cpp \
    src/function/window/on_window_errors_warnings.cpp \
    src/function/window/on_window_ipc_message.cpp \
    src/function/view/on_view_fullscreen.cpp \
    src/function/view/on_view_statusbar.cpp \
    src/function/view/on_view_toolbar.cpp \
    src/function/help/on_help_about.cpp \
    src/function/help/on_help_help.cpp \
    src/function/tab/on_tab_close_requested.cpp \
    src/function/tab/on_current_tab_changed.cpp \
    src/function/editor/on_editor_modification_changed.cpp \
    src/function/editor/on_editor_cursor_position_changed.cpp

# Header files
HEADERS += \
    inc/widget/main_window/main_window.h \
    inc/widget/editor/code_editor.h \
    inc/widget/navigator/navigator.h \
    inc/widget/segment/segment.h \
    inc/widget/errors_and_warnings/errors_and_warnings.h \
    inc/widget/ipc_message/ipc_message.h \
    inc/ipc/ipc_server.h \
    inc/project/project_manager.h

# UI files
FORMS += \
    src/widget/main_window/main_window.ui \
    src/widget/editor/code_editor.ui \
    src/widget/navigator/navigator.ui \
    src/widget/segment/segment.ui \
    src/widget/errors_and_warnings/errors_and_warnings.ui \
    src/widget/ipc_message/ipc_message.ui

# Include directories
INCLUDEPATH += \
    inc \
    inc/widget \
    inc/widget/main_window \
    inc/widget/editor \
    inc/widget/navigator \
    inc/widget/segment \
    inc/widget/errors_and_warnings \
    inc/widget/ipc_message \
    inc/ipc \
    inc/project \
    src \
    src/widget \
    src/widget/main_window \
    src/widget/editor \
    src/widget/navigator \
    src/widget/segment \
    src/widget/errors_and_warnings \
    src/widget/ipc_message \
    src/ipc \
    src/project

# Output directories
DESTDIR = bin
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Compiler flags
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra

# Application icon
# RC_ICONS = resources/icon.ico

# Deployment
target.path = $$PWD/bin
INSTALLS += target