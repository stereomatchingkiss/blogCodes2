TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

OPENCV_PATH = ../../../3rdLibs/opencv/dev/vc2015_64/

INCLUDEPATH += $${OPENCV_PATH}/install/include
LIBS += $${OPENCV_PATH}/install/x64/vc14/lib/opencv_world401.lib

SOURCES += \
        main.cpp \    
    ssd_detector.cpp \
    ../fps_estimator.cpp \
    yolov3_detector.cpp

HEADERS += \    
    ssd_detector.hpp \
    ../fps_estimator.hpp \
    yolov3_detector.hpp \
    coco_item_type.hpp \
    voc_item_type.hpp
