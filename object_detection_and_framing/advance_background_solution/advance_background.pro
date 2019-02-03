TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../../../3rdLibs/opencv/dev/vc2015_64/

INCLUDEPATH += $${OPENCV_PATH}/install/include
LIBS += $${OPENCV_PATH}/install/x64/vc14/lib/opencv_world401.lib

SOURCES += \
        main.cpp \
    ../fps_estimator.cpp

HEADERS += \
    ../fps_estimator.hpp
