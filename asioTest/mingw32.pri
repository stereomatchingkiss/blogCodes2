QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static

CONFIG(debug, debug|release) {
LIBS += -lws2_32
LIBS += $${BOOST_LIB}/libboost_chrono-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_date_time-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_regex-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_system-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_thread-mgw48-mt-sd-1_55.a
LIBS += $${BOOST_LIB}/libboost_timer-mgw48-mt-sd-1_55.a

} else {

BOOST_LIB = $${BOOST_PATH}/mingw32_4_8_2

LIBS += -lws2_32
LIBS += $${BOOST_LIB}/libboost_chrono-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_date_time-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_regex-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_system-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_thread-mgw48-mt-s-1_55.a
LIBS += $${BOOST_LIB}/libboost_timer-mgw48-mt-s-1_55.a
}
