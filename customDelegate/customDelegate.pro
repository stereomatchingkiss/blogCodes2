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
    dispatch_delegate.cpp \
    user_model.cpp \
    drag_drop.cpp \
    custom_table_view.cpp \
    tracktableRow.cpp

HEADERS  += mainwindow.hpp \
    dispatch_delegate.hpp \
    user_model.hpp \
    drag_drop.hpp \
    custom_table_view.hpp \
    trackTableRow.hpp

FORMS    += mainwindow.ui \
    drag_drop.ui
