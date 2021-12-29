QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32{

OPENCV_PATH = ../../3rdLibs/opencv/dev/opencv/build_cpu/install

INCLUDEPATH += $${OPENCV_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc15/lib/opencv_world430.lib

}

SOURCES += \
    color_filter_widget.cpp \    
    main.cpp \
    mainwindow.cpp

HEADERS += \
    color_filter_widget.hpp \    
    mainwindow.hpp

FORMS += \
    color_filter_widget.ui \    
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
