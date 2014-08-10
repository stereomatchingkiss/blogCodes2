TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

BOOST_PATH = ../../3rdLibs/boost/boost_1_55_0
BOOST_LIB = $${BOOST_PATH}/mingw32_4_8_2

INCLUDEPATH += $${BOOST_PATH}

win32{
LIBS += $${BOOST_LIB}/libboost_filesystem-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_system-mgw48-mt-s-1_55.a

QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
}

SOURCES += main.cpp \
    moveLibs.cpp \
    getFilePaths.cpp

HEADERS += \
    moveLibs.hpp \
    getFilePaths.hpp

