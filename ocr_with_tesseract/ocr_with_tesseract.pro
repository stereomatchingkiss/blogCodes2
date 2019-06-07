TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

VCPKG_PATH = ../../3rdLibs/vcpkg/installed/x64-windows
OPENCV_PATH = ../../3rdLibs/opencv/dev/vc2015_64/

WS2_32_PATH = "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64/"

INCLUDEPATH += $${VCPKG_PATH}/include
INCLUDEPATH += $${OPENCV_PATH}/install/include

LIBS += $${OPENCV_PATH}/install/x64/vc14/lib/opencv_world401.lib
LIBS += $${VCPKG_PATH}/lib/leptonica-1.76.0.lib
LIBS += $${VCPKG_PATH}/lib/tesseract40.lib
LIBS += $${WS2_32_PATH}/ws2_32.LIB
#LIBS += $${VCPKG_PATH}/lib/icudt.lib
#LIBS += $${VCPKG_PATH}/lib/icuin.lib
#LIBS += $${VCPKG_PATH}/lib/icuio.lib
#LIBS += $${VCPKG_PATH}/lib/icutu.lib
#LIBS += $${VCPKG_PATH}/lib/icuuc.lib

SOURCES += \
        main.cpp \
    text_detector_east.cpp

HEADERS += \
    text_detector_east.hpp
