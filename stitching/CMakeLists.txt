cmake_minimum_required(VERSION 3.5)

project(stitching LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/build/install)       

    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${CMAKE_PREFIX_PATH};")
endif()

add_executable(${PROJECT_NAME} main.cpp
               feature_stitch.hpp
			   feature_stitch.cpp
               utility.hpp
            )

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)        

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}                          
                          )
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
