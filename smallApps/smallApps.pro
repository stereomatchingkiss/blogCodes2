TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static

SOURCES += main.cpp \
    processtestResult.cpp

HEADERS += \
    processtestResult.hpp

