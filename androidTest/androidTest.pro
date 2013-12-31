# Add more folders to ship with the application, here
folder_01.source = qml/androidTest
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

#QMAKE_CXXFLAGS += -std=c++0x
CONFIG   += c++11 console
QT += quick androidextras

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    qtquick2applicationviewer/qtquick2applicationviewer.cpp \
    simpleJNI.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
#include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
#qtcAddDeployment()

OTHER_FILES += \
    qml/androidTest/main.qml \
    android/AndroidManifest.xml \
    android/src/adv/appBucksAdv.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

HEADERS += \
    qtquick2applicationviewer/qtquick2applicationviewer.h \
    simpleJNI.hpp
