TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../opencv

INCLUDEPATH += $${OPENCV_PATH}/include
LIBS += $${OPENCV_PATH}/lib/opencv_world400.lib

#OPENCV_PATH = ../../../3rdLibs/opencv/opencv_4_0_0/opencv/build

#INCLUDEPATH += $${OPENCV_PATH}/include
#LIBS += $${OPENCV_PATH}/x64/vc15/lib/opencv_world400.lib

SOURCES += \
        main.cpp \
    names_creator.cpp \
    rcnn_instance_segment.cpp \
    ssd_detector.cpp \
    ../fps_estimator.cpp \
    yolov3_detector.cpp \
    bg_subtractor.cpp \
    config_parser.cpp

HEADERS += \
    names_creator.hpp \
    rcnn_instance_segment.hpp \
    ssd_detector.hpp \
    ../fps_estimator.hpp \
    yolov3_detector.hpp \
    coco_item_type.hpp \
    voc_item_type.hpp \
    bg_subtractor.hpp \
    config_parser.hpp \
    bg_subtractor_config.hpp
