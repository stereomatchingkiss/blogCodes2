#-------------------------------------------------
#
# Project created by QtCreator 2016-02-04T06:13:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = object_detector
TEMPLATE = app

INCLUDEPATH += ../..
INCLUDEPATH += ../../qt_enhance/gui

include(../../pri/tbb.pri)
include(../../pri/cv.pri)
include(../../pri/boost.pri)
include(../../pri/tiny_cnn.pri)

LIBS += ../../build-qt_enhance-vc2013_64-Release/release/qt_enhance.lib

SOURCES += main.cpp\
        ui/mainwindow.cpp \
    ui/setting_window.cpp \
    ui/general_setting.cpp \
    utility/global.cpp \
    algo/tiny_cnn_car_classifier.cpp \
    algo/cvmat_to_pixmap.cpp \
    ../../ocv_libs/qt/mat_and_qimage.cpp \
    ../../ocv_libs/core/resize.cpp \
    ui/object_classify_setting.cpp

HEADERS  += ui/mainwindow.hpp \
    ui/setting_window.hpp \
    ui/general_setting.hpp \
    utility/global.hpp \
    algo/tiny_cnn_car_classifier.hpp \
    algo/cvmat_to_pixmap.hpp \
    ui/object_classify_setting.hpp

FORMS    += ui/mainwindow.ui \
    ui/setting_window.ui \
    ui/general_setting.ui \
    ui/object_classify_setting.ui

RESOURCES += \
    pics.qrc
