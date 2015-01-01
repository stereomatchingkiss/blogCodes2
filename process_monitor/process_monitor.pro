#-------------------------------------------------
#
# Project created by QtCreator 2014-12-30T10:08:33
#
#-------------------------------------------------

QT       += core
CONFIG   += c++11 console

QT       -= gui

TARGET = process_monitor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    qprocess_guard.cpp \
    start_qprocess_repeat.cpp \
    process_manager.cpp \
    utility.cpp

HEADERS += \
    qprocess_guard.hpp \
    start_qprocess_repeat.hpp \
    process_manager.hpp \
    utility.hpp
