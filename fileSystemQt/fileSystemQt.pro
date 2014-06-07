TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
#CONFIG -= qt
CONFIG += c++11

QT       += concurrent core gui

SOURCES += main.cpp \
    moveLibsAndDll.cpp

HEADERS += \
    moveLibsAndDLL.hpp

