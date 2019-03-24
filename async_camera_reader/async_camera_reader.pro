TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../../3rdLibs/opencv/opencv_4_0_1/opencv/build

INCLUDEPATH += $${OPENCV_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc15/lib/opencv_world401.lib

INCLUDEPATH += ../../

SOURCES += \
        main.cpp

HEADERS += \
    ../../ocv_libs/camera/async_opencv_video_capture.hpp


