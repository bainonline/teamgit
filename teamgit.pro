TEMPLATE = app
QT = gui core
CONFIG += qt warn_on console debug_and_release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui ui/settings.ui ui/newproject.ui ui/outputdialog.ui
HEADERS = src/mainwindowimpl.h \
 src/gitthread.h \
 src/gitprocess.h \
 src/settingsimpl.h \
 src/defs.h \
 src/settings.h \
 src/gsettings.h \
 src/newprojectimpl.h \
 src/outputdialogimpl.h \
 src/kpty_p.h \
 src/kpty.h \
 src/projectsmodel.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/gitprocess.cpp \
 src/gitthread.cpp \
 src/settingsimpl.cpp \
 src/newprojectimpl.cpp \
 src/outputdialogimpl.cpp \
 src/kpty.cpp \
 src/projectsmodel.cpp
RESOURCES += ui/icons.qrc
