TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

win32{

DEFINES += WIN_OS

INCLUDEPATH += ../../../3rdLibs/openCV/OpenCV-2.4.5/build/include

#for mingw
LIBS += -L../../../3rdLibs/openCV/OpenCV-2.4.5/builded/bin -lopencv_core245 -lopencv_imgproc245

#for vc series
#LIBS += -L../../3rdLibs/openCV/OpenCV-2.4.5/build/x86/vc11/lib -lopencv_core245 -lopencv_imgproc245
}
mac{

CONFIG -= app_bundle

DEFINES += MAC_OS

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib/ -lopencv_core.2.4.5  -lopencv_imgproc.2.4.5
LIBS += -L/usr/local/lib/ -lopencv_highgui.2.4.5 -lopencv_video.2.4.5
LIBS += -L/usr/local/lib/ -lopencv_features2d.2.4.5
}

SOURCES += main.cpp

