TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../../../../pri/cv_dev.pri)

LIBS += -L/usr/local/lib -lopencv_dnn

SOURCES += main.cpp
