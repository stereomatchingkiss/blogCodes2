#-------------------------------------------------
#
# Project created by QtCreator 2016-01-20T12:32:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = download_manager
TEMPLATE = app

include(../../pri/boost.pri)

SOURCES += main.cpp \
    download_manager_ui.cpp \
    download_manager.cpp \
    download_item.cpp \
    download_model.cpp \
    download_info.cpp

HEADERS  += \
    download_manager_ui.hpp \
    download_manager.hpp \
    download_item.hpp \
    download_model.hpp \
    download_info.hpp

FORMS    += \
    download_manager_ui.ui

RESOURCES += \
    pics.qrc
