QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        MdParser.cpp \
        config.cpp \
        directory.cpp \
        filemanager.cpp \
        main.cpp \
        page.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    MdParser.hpp \
    __MdParseHTML.hpp \
    config.h \
    directory.h \
    filemanager.h \
    page.h

DISTFILES += \
    mytest.md \
    test.md \
    yaml/yaml.pri

include ($${PWD}/yaml/yaml.pri)
