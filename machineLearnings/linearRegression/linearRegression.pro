#-------------------------------------------------
#
# Project created by QtCreator 2013-11-07T21:53:17
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = linearRegression
TEMPLATE = app

win32{

DEFINES += WIN_OS

INCLUDEPATH += ../../../3rdLibs/openCV/OpenCV-2.4.5/build/include

#for mingw
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_calib3d245
LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_contrib245
LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_core245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_features2d245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_flann245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_gpu245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_highgui245
LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_imgproc245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_legacy245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_ml245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_nonfree245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_objdetect245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_ocl245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_photo245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_stitching245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_superres245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_ts245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin  -lopencv_video245
#LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_videostab245

#for vc series
#LIBS += -L../../3rdLibs/openCV/OpenCV-2.4.5/build/x86/vc11/lib -lopencv_core245 -lopencv_imgproc245
}
mac{

CONFIG -= app_bundle
CONFIG += qwt

DEFINES += MAC_OS

INCLUDEPATH += /usr/local/include

include ( /usr/local/qwt-6.1.0/features/qwt.prf )

#LIBS += -L/usr/local/lib/ -lopencv_calib3d.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_contrib.2.4.5
LIBS += -L/usr/local/lib/ -lopencv_core.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_features2d.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_flann.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_gpu.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_highgui.2.4.5
LIBS += -L/usr/local/lib/ -lopencv_imgproc.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_legacy.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_ml.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_nonfree.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_objdetect.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_ocl.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_photo.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_stitching.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_superres.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_ts.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_video.2.4.5
#LIBS += -L/usr/local/lib/ -lopencv_videostab.2.4.5
}

INCLUDEPATH += ../../libs
INCLUDEPATH += ../libs
DEFINES += DISABLED_QT

SOURCES += main.cpp \
    ../../libs/qwtHelp/qwtUtility.cpp \
    ../../libs/qwtHelp/simple2DCurve.cpp

HEADERS  += \
    ../../libs/qwtHelp/qwtUtility.hpp \
    ../../libs/qwtHelp/simple2DCurve.hpp

FORMS    +=
