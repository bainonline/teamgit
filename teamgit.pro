TEMPLATE = app
QT = gui core
CONFIG += qt \
 warn_on \
 console \
 build_all \
 debug_and_release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui \
 ui/settings.ui \
 ui/newproject.ui \
 ui/outputdialog.ui \
 ui/commitdialog.ui \
 ui/resetdialog.ui
HEADERS = src/mainwindowimpl.h \
 src/gitthread.h \
 src/gitprocess.h \
 src/settingsimpl.h \
 src/defs.h \
 src/settings.h \
 src/gsettings.h \
 src/newprojectimpl.h \
 src/outputdialogimpl.h \
 src/projectsmodel.h \
 src/commitdialogimpl.h \
 src/diffviewer.h \
 src/resetdialogimpl.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/gitprocess.cpp \
 src/gitthread.cpp \
 src/settingsimpl.cpp \
 src/newprojectimpl.cpp \
 src/outputdialogimpl.cpp \
 src/projectsmodel.cpp \
 src/commitdialogimpl.cpp \
 src/diffviewer.cpp \
 src/resetdialogimpl.cpp

unix {
	SOURCES+=src/kpty.cpp
	HEADERS+=src/kpty_p.h \
		 src/kpty.h
}

RESOURCES += ui/icons.qrc
target.path = /usr/bin/
desktop_file.path = /usr/share/applications/
desktop_file.files = teamgit.desktop
pixmaps.path = /usr/share/icons/hicolor/48x48/apps
pixmaps.files = teamgit.png
INSTALLS += desktop_file pixmaps target
