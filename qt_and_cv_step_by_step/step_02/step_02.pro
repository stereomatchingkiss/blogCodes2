TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += ../../../3rdLibs/opencv/opencv_3_4_2/opencv/build/include

LIBS += ../../../3rdLibs/opencv/opencv_3_4_2/opencv/build/x64/vc14/lib/opencv_world342.lib
