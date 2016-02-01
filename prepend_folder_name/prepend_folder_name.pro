CONFIG += c++11

TARGET = prepend_folder_name
CONFIG += console
CONFIG -= app_bundle

include(../../pri/boost.pri)
INCLUDEPATH += ../..

TEMPLATE = app

SOURCES += main.cpp \
    ../../ocv_libs/cmd/command_prompt_utility.cpp \
    ../../ocv_libs/file/utility.cpp
