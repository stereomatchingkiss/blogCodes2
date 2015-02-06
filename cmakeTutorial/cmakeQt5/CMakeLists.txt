project(cmakeQt5)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(Qt5Dir "C:/Users/yyyy/Qt/3rdLibs/Qt/Qt5.4.0/5.4/mingw491_32")
set(HW_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/incl)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${Qt5Dir}")
set(CMAKE_BUILD_TYPE "release")

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtWidget_localtion Qt5::Widgets LOCATION)
get_target_property(QtGui Qt5::Gui LOCATION)

set(HW_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/incl)
set(HW_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gui)

include_directories(${HW_HEADER_DIR})

subdirs(src)
