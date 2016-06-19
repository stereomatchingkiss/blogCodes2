cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(img_hash_test CXX)

set(CMAKE_BUILD_TYPE Release)
set(CV_PATH C:/Users/yyyy/Qt/3rdLibs/opencv/contribute/opencv_contrib/modules/img_hash)

set(PHASH_PATH C:/Users/yyyy/Qt/blogCodes2/image_hash/pHash-0.9.4)

add_definitions(-DHAVE_IMAGE_HASH)

include_directories(${CV_PATH}/include)
include_directories(${PHASH_PATH})

add_executable(${PROJECT_NAME}
#main.cpp
#hash_samples.cpp
phash_performance.cpp
${CV_PATH}/src/average_hash.cpp
${CV_PATH}/src/block_mean_hash.cpp
${CV_PATH}/src/marr_hildreth_hash.cpp
${CV_PATH}/src/phash.cpp
${CV_PATH}/src/radial_variance_hash.cpp
#${PHASH_PATH}/pHash.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../pri/cv_dev.cmake)
