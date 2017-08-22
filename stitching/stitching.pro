TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../pri/cv_dev.pri)

LIBS += -L/usr/local/lib -lopencv_calib3d -lopencv_features2d -lopencv_stitching

SOURCES += main.cpp \
    feature_stitch.cpp

HEADERS += \
    feature_stitch.hpp \
    utility.hpp
