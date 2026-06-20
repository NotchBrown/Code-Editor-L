# CodeEditorLite Project
# Qt5.14.2 qmake project file

QT       += core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeEditorLite
TEMPLATE = app

# Version
VERSION = 1.0.0

# QScintilla configuration
QSCINTILLA_PATH = $$PWD/lib/qscintilla_mingw64
LIBS += -L$$QSCINTILLA_PATH/lib -lqscintilla2_qt5
INCLUDEPATH += $$QSCINTILLA_PATH/include/Qsci

# Tree-sitter configuration
TREE_SITTER_PATH = $$PWD/lib/tree_sitter_mingw64
LIBS += -L$$TREE_SITTER_PATH/lib -llibtree-sitter
INCLUDEPATH += $$TREE_SITTER_PATH/include

# Source files
SOURCES += \
    src/main.cpp \
    src/widget/main_window/main_window.cpp \
    src/widget/main_window/status_bar.cpp \
    src/widget/editor/code_editor.cpp \
    src/widget/navigator/navigator.cpp \
    src/widget/segment/segment.cpp \
    src/widget/errors_and_warnings/errors_and_warnings.cpp \
    src/widget/ipc_message/ipc_message.cpp \
    src/widget/properties/properties.cpp \
    src/widget/go_line/go_line.cpp \
    src/ipc/ipc_server.cpp \
    src/project/project_manager.cpp \
    src/function/start/on_start_new_instance.cpp \
    src/function/start/on_start_exit.cpp \
    src/function/file/on_file_new.cpp \
    src/function/file/on_file_open.cpp \
    src/function/file/on_file_save.cpp \
    src/function/file/on_file_save_as.cpp \
    src/function/file/on_file_properties.cpp \
    src/function/file/on_file_print.cpp \
    src/function/file/on_file_encoding.cpp \
    src/function/edit/on_edit_undo.cpp \
    src/function/edit/on_edit_redo.cpp \
    src/function/edit/on_edit_cut.cpp \
    src/function/edit/on_edit_copy.cpp \
    src/function/edit/on_edit_paste.cpp \
    src/function/edit/on_edit_delete.cpp \
    src/function/edit/on_edit_go_line.cpp \
    src/function/edit/on_edit_comment.cpp \
    src/function/edit/on_edit_uncomment.cpp \
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
    src/function/mark/on_mark_clean_break.cpp \
    src/function/settings/on_settings_general.cpp \
    src/function/settings/on_settings_editor.cpp \
    src/function/settings/on_settings_ipc.cpp \
    src/function/settings/on_settings_toolbar.cpp \
    src/function/settings/on_settings_hotkey.cpp \
    src/function/settings/on_settings_addons.cpp \
    src/component/addon_manager.cpp \
    src/widget/addons/addons_dialog.cpp \
    src/function/window/on_window_close_tab.cpp \
    src/function/window/on_window_close_all_saved.cpp \
    src/function/window/on_window_close_all.cpp \
    src/function/window/on_window_close_other.cpp \
    src/function/window/on_window_navigator.cpp \
    src/function/window/on_window_segment.cpp \
    src/function/window/on_window_errors_warnings.cpp \
    src/function/window/on_window_ipc_message.cpp \
    src/function/window/on_window_find_and_replace.cpp \
    src/function/view/on_view_fullscreen.cpp \
    src/function/view/on_view_statusbar.cpp \
    src/function/view/on_view_toolbar.cpp \
    src/function/help/on_help_about.cpp \
    src/function/help/on_help_help.cpp \
    src/function/tab/on_tab_close_requested.cpp \
    src/function/tab/on_current_tab_changed.cpp \
    src/function/editor/on_editor_modification_changed.cpp \
    src/function/editor/on_editor_cursor_position_changed.cpp \
    src/function/editor/on_cursor_pos_changed.cpp \
    src/util/recent_files_manager.cpp \
    src/util/logger.cpp \
    src/util/resource_manager.cpp \
    src/util/tree_sitter_manager.cpp \
    src/util/query_rules.cpp \
    src/component/component_manager.cpp \
    src/util/hotkey_manager.cpp \
    src/widget/print/print_wizard.cpp \
    src/widget/print/print_wizard_page_printer.cpp \
    src/widget/print/print_wizard_page_page_setup.cpp \
    src/widget/print/print_wizard_page_output.cpp \
    src/widget/print/print_wizard_page_layout.cpp \
    src/widget/print/print_wizard_page_preview.cpp \
    src/widget/find_and_replace/find_and_replace.cpp \
    src/widget/hotkey/hotkey.cpp \
    src/widget/bookmark/go_bookmark.cpp \
    src/widget/bookmark/manage_bookmark.cpp

# Header files
HEADERS += \
    inc/widget/main_window/main_window.h \
    inc/widget/main_window/status_bar.h \
    inc/widget/editor/code_editor.h \
    inc/widget/navigator/navigator.h \
    inc/widget/segment/segment.h \
    inc/widget/errors_and_warnings/errors_and_warnings.h \
    inc/widget/ipc_message/ipc_message.h \
    inc/widget/properties/properties.h \
    inc/widget/go_line/go_line.h \
    inc/ipc/ipc_server.h \
    inc/project/project_manager.h \
    inc/util/recent_files_manager.h \
    inc/util/logger.h \
    inc/util/resource_manager.h \
    inc/util/tree_sitter_manager.h \
    inc/util/query_rules.h \
    inc/component/language_component.h \
    inc/component/component_manager.h \
    inc/component/addon_info.h \
    inc/component/addon_manager.h \
    inc/widget/addons/addons_dialog.h \
    inc/util/hotkey_manager.h \
    inc/widget/print/print_wizard.h \
    inc/widget/print/print_wizard_page_printer.h \
    inc/widget/print/print_wizard_page_page_setup.h \
    inc/widget/print/print_wizard_page_output.h \
    inc/widget/print/print_wizard_page_layout.h \
    inc/widget/print/print_wizard_page_preview.h \
    inc/widget/find_and_replace/find_and_replace.h \
    inc/widget/hotkey/hotkey.h \
    inc/widget/bookmark/go_bookmark.h \
    inc/widget/bookmark/manage_bookmark.h

# UI files
FORMS += \
    src/widget/main_window/main_window.ui \
    src/widget/editor/code_editor.ui \
    src/widget/navigator/navigator.ui \
    src/widget/segment/segment.ui \
    src/widget/find_and_replace/find_and_replace.ui \
    src/widget/errors_and_warnings/errors_and_warnings.ui \
    src/widget/ipc_message/ipc_message.ui \
    src/widget/properties/properties.ui \
    src/widget/go_line/go_line.ui \
    src/widget/print/print_wizard.ui \
    src/widget/print/print_wizard_page_printer.ui \
    src/widget/print/print_wizard_page_page_setup.ui \
    src/widget/print/print_wizard_page_output.ui \
    src/widget/print/print_wizard_page_layout.ui \
    src/widget/print/print_wizard_page_preview.ui \
    src/widget/hotkey/hotkey.ui \
    src/widget/bookmark/go_bookmark.ui \
    src/widget/bookmark/manage_bookmark.ui

# Resource files
RESOURCES += \
    src/resource/resources.qrc

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
    inc/widget/properties \
    inc/widget/go_line \
    inc/ipc \
    inc/project \
    inc/util \
    inc/widget/print \
    src \
    src/widget \
    src/widget/main_window \
    src/widget/editor \
    src/widget/navigator \
    src/widget/segment \
    src/widget/errors_and_warnings \
    src/widget/ipc_message \
    src/widget/properties \
    src/widget/find_and_replace \
    inc/widget/find_and_replace \
    inc/widget/hotkey \
    src/widget/hotkey \
    inc/widget/bookmark \
    src/widget/bookmark \
    src/ipc \
    src/project \
    inc/component \
    inc/widget/addons \
    src/util \
    src/widget/print

# Output directories
DESTDIR = bin
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Post-build: copy tree-sitter grammar DLLs and runtime library
TREE_SITTER_GRAMMAR_SRC = $$PWD/lib/tree_sitter_mingw64/grammars
TREE_SITTER_BIN_SRC = $$PWD/lib/tree_sitter_mingw64/bin
win32 {
    QMAKE_POST_LINK += $$quote(mkdir $$shell_path($$DESTDIR/grammars) 2>nul &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_c.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_cpp.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_python.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_javascript.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_bash.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_java.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_c-sharp.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_ruby.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_html.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_css.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_json.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_GRAMMAR_SRC/ts_verilog.dll) $$shell_path($$DESTDIR/grammars/) &)
    QMAKE_POST_LINK += $$quote(copy /y $$shell_path($$TREE_SITTER_BIN_SRC/libtree-sitter.dll) $$shell_path($$DESTDIR/) &)
}

# Compiler flags
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra

# Application icon
# RC_ICONS = resources/icon.ico

# Deployment
target.path = $$PWD/bin
INSTALLS += target