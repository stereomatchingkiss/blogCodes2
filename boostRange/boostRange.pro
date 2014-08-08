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

LIBS += $${BOOST_PATH}/mingw32_4_8_2/libboost_date_time-mgw48-1_55.a
}

SOURCES += main.cpp

