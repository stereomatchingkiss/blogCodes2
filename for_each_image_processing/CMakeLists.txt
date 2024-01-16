cmake_minimum_required(VERSION 3.5)

project(for_each_image_processing LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)
set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libs)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/opencv_4_8_0/opencv/build)
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${CMAKE_PREFIX_PATH};")
endif()

add_executable(${PROJECT_NAME} main.cpp)

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          )
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
