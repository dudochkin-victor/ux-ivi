include (../common.pri)
TEMPLATE = lib
TARGET = IVI

QT += declarative
CONFIG += qt \
    plugin \
    link_pkgconfig \
    mobility

PKGCONFIG += gconf-2.0 \
    mlite \
    gstreamer-0.10

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = $$TARGET
OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
SOURCES += \
    components.cpp \
    desktop.cpp \
    inputcontrol.cpp \
    menuitem.cpp \
    menumodel.cpp \
    ttscontrol.cpp \
    voicecontrol.cpp

OTHER_FILES += \

HEADERS += \
    components.h \
    desktop.h \
    inputcontrol.h \
    menuitem.h \
    menumodel.h \
    ttscontrol.h \
    voicecontrol.h

qmldir.files += $$TARGET
qmldir.path += $$[QT_INSTALL_IMPORTS]/MeeGo/Labs/
INSTALLS += qmldir
