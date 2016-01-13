cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(FACE_RECOGNITION CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-mavx")

add_definitions(-DDLIB_JPEG_SUPPORT)
add_definitions(-DNOMINMAX)

INCLUDE_DIRECTORIES(${OCV_PATH})

add_executable(${PROJECT_NAME}
face_capture.hpp
face_capture.cpp
face_detector.hpp
face_detector.cpp
face_recognition.hpp
face_recognition.cpp
main.cpp
${OCV_PATH}/ocv_libs/core/resize.hpp
${OCV_PATH}/ocv_libs/core/resize.cpp
${OCV_PATH}/ocv_libs/file/utility.hpp
${OCV_PATH}/ocv_libs/file/utility.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/dlib.cmake)

target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_filesystem-vc140-mt-1_59.lib)
target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_program_options-vc140-mt-1_59.lib)
target_link_libraries(${PROJECT_NAME}
${BOOST_ROOT}/lib64-msvc-14.0/libboost_system-vc140-mt-1_59.lib)
