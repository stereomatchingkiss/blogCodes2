TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../pri/cv_dev.pri)

SOURCES += main.cpp \
    feature_stitch.cpp

HEADERS += \
    feature_stitch.hpp \
    utility.hpp
