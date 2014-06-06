TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

win32{
BOOST_PATH = ../../3rdLibs/boost/boost_1_55_0
BOOST_LIB = $${BOOST_PATH}/lib32-msvc-9.0

INCLUDEPATH += $${BOOST_PATH}

CONFIG(debug, debug|release) {
LIBS += $${BOOST_LIB}/libboost_chrono-vc90-mt-gd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_date_time-vc90-mt-gd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_regex-vc90-mt-gd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-gd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-gd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_timer-vc90-mt-gd-1_55.lib
} else {
LIBS += $${BOOST_LIB}/libboost_chrono-vc90-mt-1_55.lib
LIBS += $${BOOST_LIB}/libboost_date_time-vc90-mt-1_55.lib
LIBS += $${BOOST_LIB}/libboost_regex-vc90-mt-1_55.lib
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-1_55.lib
LIBS += $${BOOST_LIB}/libboost_timer-vc90-mt-1_55.lib
}

}

SOURCES += main.cpp \
    post00.cpp \
    noWork00.cpp \
    dispatch00.cpp

HEADERS += \
    post00.hpp \
    noWork00.hpp \
    dispatch00.hpp

