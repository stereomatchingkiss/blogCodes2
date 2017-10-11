TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../pri/boost.pri)
include(../../pri/cv_dev.pri)

INCLUDEPATH += ../..

SOURCES += main.cpp \
    #cotton_ball.cpp \
    hough_lines_polygon.cpp \
    mapping_files.cpp \
    so_obj_detect_00.cpp \
    ../../ocv_libs/utility/hsv_range_observer.cpp \
    ../../ocv_libs/core/attribute.cpp \
    marble_slab.cpp \
    extract_cow_number.cpp

HEADERS += \
    #cotton_ball.hpp \
    hough_lines_polygon.hpp \
    mapping_files.hpp \
    so_obj_detect_00.hpp \
    marble_slab.hpp \
    extract_cow_number.hpp
