#-------------------------------------------------
#
# Project created by QtCreator 2016-01-27T03:08:40
#
#-------------------------------------------------

#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tiny_cnn_test
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += CNN_USE_TBB
DEFINES += CNN_USE_AVX
#DEFINES += CNN_USE_SSE
msvc:QMAKE_CXXFLAGS_RELEASE += /openmp /arch:AVX

include(../../pri/boost.pri)
include(../../pri/cv.pri)
#include(../../pri/rapidjson.pri)
include(../../pri/tbb.pri)
include(../../pri/tiny_cnn.pri)

SOURCES += main.cpp\
    car_benchmark.cpp \
    mnist_benchmark.cpp

HEADERS  += \
    mnist_benchmark.hpp \
    car_benchmark.hpp \
    utility.hpp

FORMS    +=
