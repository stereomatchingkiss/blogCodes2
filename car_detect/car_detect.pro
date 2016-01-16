#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T10:51:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = car_detect
TEMPLATE = app

include(../../pri/cv.pri)

SOURCES += main.cpp\
       mainwindow.cpp \
    object_classifier.cpp

HEADERS  += mainwindow.hpp \
    object_classifier.hpp

FORMS    += mainwindow.ui
