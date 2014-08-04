TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

BOOST_PATH = ../../3rdLibs/boost/boost_1_55_0

INCLUDEPATH += $${BOOST_PATH}
INCLUDEPATH += ../libs

win32{
QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
}

SOURCES += main.cpp \
    find_if.cpp

HEADERS += \
    ../libs/testFunction/testHelper.hpp \
    find_if.hpp

