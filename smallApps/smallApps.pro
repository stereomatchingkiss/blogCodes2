TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
#DEFINES += -Wno-unused-function

win32{
BOOST_PATH = G:/3rdLibs/boost/boost_1_56_0

INCLUDEPATH += $${BOOST_PATH}
INCLUDEPATH += ../libs

#include(../mingw32.pri)
#include(vc90_32.pri)
}

SOURCES += main.cpp \
    processtestResult.cpp \
    parseSVNLog.cpp \
    svnLogStructure.cpp \
    appStatistic.cpp \
    generateNewSVNLog.cpp \
    cpp_programming_lg_4.cpp

HEADERS += \
    processtestResult.hpp \
    parseSVNLog.hpp \
    svnLogStructure.hpp \
    appStatistic.hpp \
    generateNewSVNLog.hpp \
    cpp_programming_lg_4.h

