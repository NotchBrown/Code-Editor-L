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
    src/ipc/ipc_server.cpp \
    src/project/project_manager.cpp

# Header files
HEADERS += \
    inc/widget/main_window/main_window.h \
    inc/widget/editor/code_editor.h \
    inc/ipc/ipc_server.h \
    inc/project/project_manager.h

# UI files
FORMS += \
    src/widget/main_window/main_window.ui \
    src/widget/editor/code_editor.ui

# Include directories
INCLUDEPATH += \
    inc \
    inc/widget \
    inc/widget/main_window \
    inc/widget/editor \
    inc/ipc \
    inc/project \
    src \
    src/widget \
    src/widget/main_window \
    src/widget/editor \
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