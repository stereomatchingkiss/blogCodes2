TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

win32{
BOOST_PATH = ../../3rdLibs/boost/boost_1_55_0

INCLUDEPATH += $${BOOST_PATH}

include(mingw32.pri)
#include(vc90_32.pri)
}

SOURCES += main.cpp \
    connectTest.cpp

OTHER_FILES += \
    mingw32.pri \
    vc90_32.pri

HEADERS += \
    connectTest.hpp

