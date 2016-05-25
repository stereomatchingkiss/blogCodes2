cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(forum_quest CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_definitions(-DDLIB_JPEG_SUPPORT)
add_definitions(-DDLIB_PNG_SUPPORT)

INCLUDE_DIRECTORIES(${OCV_PATH})

add_executable(${PROJECT_NAME}
main.cpp
${OCV_PATH}/ocv_libs/core/attribute.cpp
hough_lines_polygon.hpp
hough_lines_polygon.cpp
so_obj_detect_00.hpp
so_obj_detect_00.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv_dev.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/dlib.cmake)
