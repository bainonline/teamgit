TEMPLATE = app
QT = gui core
CONFIG += qt warn_on console debug_and_release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui ui/settings.ui
HEADERS = src/mainwindowimpl.h \
 src/gitthread.h \
 src/gitprocess.h \
 src/settingsimpl.h \
 src/projectsettings.h \
 src/projectmanager.h \
 src/defs.h \
 src/settings.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/gitprocess.cpp \
 src/gitthread.cpp \
 src/settingsimpl.cpp \
 src/projectmanager.cpp
