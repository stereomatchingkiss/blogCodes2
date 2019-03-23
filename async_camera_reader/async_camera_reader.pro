TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../../3rdLibs/opencv/opencv_4_0_1/opencv/build
#MXNET_PATH = ../../3rdLibs/incubator-mxnet/build/install

INCLUDEPATH += $${OPENCV_PATH}/include
#INCLUDEPATH += $${MXNET_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc15/lib/opencv_world401.lib
#LIBS += $${MXNET_PATH}/lib/libmxnet.lib

INCLUDEPATH += ../../3rdLibs/boost/boost_1_69_0/
INCLUDEPATH += ../../

SOURCES += \
        main.cpp \
    ../../ocv_libs/camera/async_opencv_video_capture.cpp

HEADERS += \
    ../../ocv_libs/camera/async_opencv_video_capture.hpp


