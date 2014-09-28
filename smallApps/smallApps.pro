TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

#DEFINES += -Wno-unused-function

win32{
BOOST_PATH = ../../3rdLibs/boost/boost_1_56_0

INCLUDEPATH += $${BOOST_PATH}
INCLUDEPATH += ../libs

#include(../mingw32.pri)
#include(vc90_32.pri)
}

SOURCES += main.cpp \
    processtestResult.cpp \
    parseSVNLog.cpp \
    svnLogStructure.cpp

HEADERS += \
    processtestResult.hpp \
    parseSVNLog.hpp \
    svnLogStructure.hpp

