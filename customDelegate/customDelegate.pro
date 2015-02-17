#-------------------------------------------------
#
# Project created by QtCreator 2015-02-11T18:14:53
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = customDelegate
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dispatch_delegate.cc \
    user_model.cc \
    drag_drop.cpp \
    custom_table_view.cpp

HEADERS  += mainwindow.h \
    dispatch_delegate.h \
    user_model.h \
    drag_drop.hpp \
    custom_table_view.h

FORMS    += mainwindow.ui \
    drag_drop.ui
