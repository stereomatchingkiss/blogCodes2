project(cmake00)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BOOST_LIBS "C:/Users/yyyy/Qt/3rdLibs/boost/boost_1_57_0/mingw32_4_9_1")
set(BOOST_LIBS_COMPILER "libboost")
set(BOOST_LIBS_VER "mgw49-mt-s-1_57")
set(BOOST_LIBS_TYPE "a")

include_directories("../../../3rdLibs/boost/boost_1_57_0")

add_executable(${PROJECT_NAME} ${SRC_LIST})

#link to the boost libraries
FOREACH(LIBS thread;system)
  target_link_libraries(${PROJECT_NAME} "${BOOST_LIBS}/${BOOST_LIBS_COMPILER}_${LIBS}-${BOOST_LIBS_VER}.${BOOST_LIBS_TYPE}")
  MESSAGE(${LIBS})
ENDFOREACH()

#target_link_libraries(${PROJECT_NAME} "${BOOST_LIBS}/${BOOST_LIBS_COMPILER}_thread-${BOOST_LIBS_VER}.${BOOST_LIBS_TYPE}")
#target_link_libraries(${PROJECT_NAME} "${BOOST_LIBS}/${BOOST_LIBS_COMPILER}_system-${BOOST_LIBS_VER}.${BOOST_LIBS_TYPE}")

