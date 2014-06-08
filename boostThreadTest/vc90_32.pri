BOOST_LIB = $${BOOST_PATH}/lib32-msvc-9.0

CONFIG(debug, debug|release) {
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-sgd-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-sgd-1_55.lib
} else {
LIBS += $${BOOST_LIB}/libboost_system-vc90-mt-s-1_55.lib
LIBS += $${BOOST_LIB}/libboost_thread-vc90-mt-s-1_55.lib
}

