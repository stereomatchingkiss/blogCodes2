#-------------------------------------------------
#
# Project created by QtCreator 2020-05-25T01:05:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stream_capture_test
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../../3rdLibs/opencv/opencv_4_3_0/opencv/build/include
LIBS += ../../3rdLibs/opencv/opencv_4_3_0/opencv/build/x64/vc15/lib/opencv_world430.lib

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        frame_capture_controller.cpp \
        frame_capture_opencv_worker.cpp \
        frame_worker_base.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        frame_capture_config.hpp \
        frame_capture_controller.hpp \
        frame_capture_opencv.hpp \
        frame_capture_opencv_worker.hpp \
        frame_worker_base.hpp \
        mainwindow.hpp

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
