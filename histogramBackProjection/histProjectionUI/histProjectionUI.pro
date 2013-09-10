#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T22:39:09
#
#-------------------------------------------------

CONFIG   += c++11
QT       += core gui

win32{

DEFINES += WIN_OS

INCLUDEPATH += ../../../3rdLibs/openCV/OpenCV-2.4.5/build/include

#for mingw
LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_core245 -lopencv_imgproc245

#for vc series
#LIBS += -L../../3rdLibs/openCV/OpenCV-2.4.5/build/x86/vc11/lib -lopencv_core245 -lopencv_imgproc245
}
mac{

DEFINES += MAC_OS

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib/ -lopencv_core.2.4.5  -lopencv_imgproc.2.4.5
LIBS += -L/usr/local/lib/ -lopencv_highgui.2.4.5
}

INCLUDEPATH += ../../libs
INCLUDEPATH += ../histProjectionNoUI

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = histProjectionUI
TEMPLATE = app


SOURCES += main.cpp \
    ../../libs/openCVToQt.cpp \
    ../../libs/imageAlgorithm.cpp \
    ../histProjectionNoUI/histBackProjectNoUI.cpp \
    histBackProjectUI.cpp

HEADERS  += \
    ../../libs/bugLocation.hpp \
    ../../libs/openCVToQt.hpp \
    ../../libs/basicImageAlgo.hpp \
    ../../libs/imageAlgorithm.hpp \
    ../histProjectionNoUI/histBackProjectNoUI.hpp \
    histBackProjectUI.hpp

FORMS    += \
    histbackprojectui.ui
