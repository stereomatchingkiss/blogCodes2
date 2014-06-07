CONFIG(debug, debug|release) {
BOOST_LIB = $${BOOST_PATH}/lib32-msvc-9.0
LIBS += $${BOOST_LIB}/libboost_chrono-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_date_time-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_regex-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_timer-vc90-mt-sgd-1_55.lib

} else {
BOOST_LIB = $${BOOST_PATH}/lib32-msvc-9.0
LIBS += $${BOOST_LIB}/libboost_chrono-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_date_time-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_regex-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_timer-vc90-mt-s-1_55.lib
}

