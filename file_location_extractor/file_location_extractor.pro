TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../pri/boost.pri)
INCLUDEPATH += ../..

SOURCES += main.cpp \
    ../../ocv_libs/file/utility.cpp
