TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

QT += core

OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build

INCLUDEPATH += $${OPENCV_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib

SOURCES += \
        main.cpp \
    ../../ocv_libs/core/montage.cpp \
    ../libs/cv_format_parser/json_utility.cpp

HEADERS += \
    ../../ocv_libs/core/montage.hpp \
    ../libs/cv_format_parser/json_utility.hpp
