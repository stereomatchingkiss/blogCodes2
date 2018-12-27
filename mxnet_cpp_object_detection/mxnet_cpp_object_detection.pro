TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build
MXNET_PATH = ../../3rdLibs/incubator-mxnet/build/install

INCLUDEPATH += $${OPENCV_PATH}/include
INCLUDEPATH += $${MXNET_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib
LIBS += $${MXNET_PATH}/lib/libmxnet.lib

SOURCES += \
        main.cpp \
    common.cpp \
    object_detector.cpp \
    plot_object_detector_bboxes.cpp \
    object_detector_filter.cpp

HEADERS += \
    common.hpp \
    object_detector.hpp \
    plot_object_detector_bboxes.hpp \
    object_detector_filter.hpp

