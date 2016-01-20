TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

include(../../pri/boost.pri)

SOURCES += main.cpp \
    employee.cpp \
    employee_sequence.cpp \
    modify_example.cpp

#include(deployment.pri)
#qtcAddDeployment()

HEADERS += \
    employee.hpp \
    employee_sequence.hpp \
    modify_example.hpp

