TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../../3rdLibs/boost/boost_1_53_0/

INCLUDEPATH += ../../libs

SOURCES += main.cpp \
    exercise00.cpp \
    lexWordCount.cpp \
    simpleDelimiter.cpp \
    stackOverflowQ00.cpp

HEADERS += \
    exercise00.hpp \
    lexWordCount.hpp \
    simpleDelimiter.hpp \
    ../../libs/timeEstimate.hpp \
    ../../libs/timeElapsed.hpp \
    stackOverflowQ00.hpp

