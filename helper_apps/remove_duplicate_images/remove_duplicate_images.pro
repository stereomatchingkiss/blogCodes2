TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += DLIB_USE_CUDA

include(../../pri/boost.pri)
include(../../pri/dlibs.pri)
include(../../pri/cv_dev.pri)

INCLUDEPATH += ../../similar_vision/core

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem

SOURCES += main.cpp

HEADERS += \
    ../../similar_vision/core/vp_tree.hpp
