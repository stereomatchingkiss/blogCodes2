TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32{
GSTREAMER_PATH = ../../3rdLibs/gstreamer/1.0/x86_64

INCLUDEPATH += $${GSTREAMER_PATH}/include/gstreamer-1.0
INCLUDEPATH += $${GSTREAMER_PATH}/include/glib-2.0
INCLUDEPATH += $${GSTREAMER_PATH}/lib/glib-2.0/include

LIBS += $${GSTREAMER_PATH}/lib/gobject-2.0.lib
LIBS += $${GSTREAMER_PATH}/lib/glib-2.0.lib
LIBS += $${GSTREAMER_PATH}/lib/gstreamer-1.0.lib
}



SOURCES += \
        dynamic_hello_world.cpp \
        gstreamer_concepts.cpp \
        gstreamer_memory.cpp \
        hello_world.cpp \
        main.cpp

HEADERS += \
    dynamic_hello_world.hpp \
    gstreamer_concepts.hpp \
    gstreamer_memory.hpp \
    hello_world.hpp
