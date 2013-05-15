include(common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += plugin themes

OTHER_FILES += \
    voicerecognition/meego-ux-ivi.corpus \
    voicerecognition/meego-ux-ivi.jsgf \
    *.qml \
    *.js

share.files += \
    *.qml \
    *.js \
    applications/ \
    voicerecognition/ \
    virtual-applications/
share.path += $$INSTALL_ROOT/usr/share/$$TARGET

INSTALLS += share

TRANSLATIONS += *.qml *.js
VERSION = 0.1.0
PROJECT_NAME = meego-ux-ivi

dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION} &&
dist.commands += git clone . $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}/.git &&
dist.commands += rm -f $${PROJECT_NAME}-$${VERSION}/.gitignore &&
dist.commands += mkdir -p $${PROJECT_NAME}-$${VERSION}/ts &&
dist.commands += lupdate $${TRANSLATIONS} -ts $${PROJECT_NAME}-$${VERSION}/ts/$${PROJECT_NAME}.ts &&
dist.commands += tar jcpvf $${PROJECT_NAME}-$${VERSION}.tar.bz2 $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}
QMAKE_EXTRA_TARGETS += dist
