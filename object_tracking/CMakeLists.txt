cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(object_tracker CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_definitions(-DDLIB_JPEG_SUPPORT)

#INCLUDE_DIRECTORIES(${OCV_PATH})

add_executable(${PROJECT_NAME}
main.cpp
correlation_trackers.hpp
correlation_trackers.cpp
object_tracking_test.hpp
object_tracking_test.cpp)

#include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv_dev.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/dlib.cmake)

#target_link_libraries(${PROJECT_NAME}
#${BOOST_ROOT}/lib64-msvc-14.0/libboost_program_options-vc140-mt-1_59.lib)
