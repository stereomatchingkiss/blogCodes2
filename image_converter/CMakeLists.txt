cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(image_converter CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

INCLUDE_DIRECTORIES(${OCV_PATH})

add_executable(${PROJECT_NAME}
main.cpp
${OCV_PATH}/ocv_libs/core/resize.hpp
${OCV_PATH}/ocv_libs/core/resize.cpp
${OCV_PATH}/ocv_libs/file/utility.hpp
${OCV_PATH}/ocv_libs/file/utility.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv.cmake)

target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_filesystem-vc140-mt-1_59.lib)
target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_program_options-vc140-mt-1_59.lib)
target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_system-vc140-mt-1_59.lib)
