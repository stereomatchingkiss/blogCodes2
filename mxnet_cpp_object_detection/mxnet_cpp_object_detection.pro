TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../

OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build
MXNET_PATH = ../../3rdLibs/mxnet/build_cpu_1_3_1/install

INCLUDEPATH += $${OPENCV_PATH}/include
INCLUDEPATH += $${MXNET_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib
LIBS += $${MXNET_PATH}/lib/libmxnet.lib

SOURCES += \
        main.cpp \    
    object_detector.cpp \
    plot_object_detector_bboxes.cpp \
    object_detector_filter.cpp \
    ../libs/mxnet/common.cpp \
    ../object_detection_and_framing/fps_estimator.cpp

HEADERS += \    
    object_detector.hpp \
    plot_object_detector_bboxes.hpp \
    object_detector_filter.hpp \
    ../libs/mxnet/common.hpp \
    ../object_detection_and_framing/fps_estimator.hpp

