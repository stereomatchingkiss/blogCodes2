cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(object_tracker CXX)

set(CMAKE_BUILD_TYPE Release)
set(OCV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_definitions(-DDLIB_JPEG_SUPPORT)
add_definitions(-DDLIB_PNG_SUPPORT)

INCLUDE_DIRECTORIES(${OCV_PATH})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/pugixml.cmake)

add_definitions(-DCNN_USE_TBB)
add_definitions(-DCNN_USE_AVX)
#DEFINES += CNN_USE_SSE
#msvc:QMAKE_CXXFLAGS_RELEASE += /openmp /arch:AVX


add_executable(${PROJECT_NAME}
${PUGIXML_FORMAT_ADD_TO_EXECUTEABLE}
${OCV_PATH}/ocv_libs/file/utility.cpp
main.cpp
read_dataset.hpp
read_dataset.cpp
correlation_trackers.hpp
correlation_trackers.cpp
object_tracking_test.hpp
object_tracking_test.cpp
fixed_size_trackers.hpp
fixed_size_trackers.cpp
get_samples.hpp
get_samples.cpp
hsv_range_observer.hpp
hsv_range_observer.cpp
player_detector.hpp
player_detector.cpp
read_stanford40_pose.hpp
read_stanford40_pose.cpp
read_usc_pedestrian.hpp
read_usc_pedestrian.cpp
CppMT/CMT.cpp
CppMT/common.cpp
CppMT/Consensus.cpp
CppMT/Fusion.cpp
CppMT/Matcher.cpp
CppMT/Tracker.cpp
CppMT/fastcluster/fastcluster.cpp
)

#target_link_libraries(${PROJECT_NAME} "C:/Users/yyyy/Qt/blogCodes2/object_tracking/CppMT/vc2013_64/Release/cmt.lib")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv_dev.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/dlib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/mlpack.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/tbb.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/tiny_cnn.cmake)
