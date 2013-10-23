TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += ../../../3rdLibs/boost/boost_1_53_0/

SOURCES += main.cpp \
    exercise00.cpp \
    lexWordCount.cpp

HEADERS += \
    exercise00.hpp \
    lexWordCount.hpp

