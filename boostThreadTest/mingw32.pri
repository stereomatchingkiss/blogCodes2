QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
BOOST_LIB = $${BOOST_PATH}/mingw32_4_8_2

CONFIG(debug, debug|release) {

LIBS += $${BOOST_LIB}/libboost_system-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_thread-mgw48-mt-sd-1_55.a

} else {

LIBS += $${BOOST_LIB}/libboost_system-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_thread-mgw48-mt-s-1_55.a
}
