cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(object_tracker CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_definitions(-DDLIB_JPEG_SUPPORT)
add_definitions(-DDLIB_PNG_SUPPORT)

INCLUDE_DIRECTORIES(${OCV_PATH})

add_executable(${PROJECT_NAME}
main.cpp
correlation_trackers.hpp
correlation_trackers.cpp
object_tracking_test.hpp
object_tracking_test.cpp
fixed_size_trackers.hpp
fixed_size_trackers.cpp
CppMT/CMT.cpp
CppMT/common.cpp
CppMT/Consensus.cpp
CppMT/Fusion.cpp
CppMT/Matcher.cpp
CppMT/Tracker.cpp
CppMT/fastcluster/fastcluster.cpp
)

#target_link_libraries(${PROJECT_NAME} "C:/Users/yyyy/Qt/blogCodes2/object_tracking/CppMT/vc2013_64/Release/cmt.lib")

#include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv_dev.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/dlib.cmake)
