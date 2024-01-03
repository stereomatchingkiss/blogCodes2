cmake_minimum_required(VERSION 3.5)

project(image_clean_up_tool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)
set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set(BOOST_LIBS_PATH ${3RDLIBS_PATH}/boost/boost_1_81_0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Xml)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/build/install)
    #set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build/ncnn-20230816-windows-vs2019/x64/lib/cmake)

    #set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0")
    #find_package(Vulkan REQUIRED)
    #target_link_libraries(target ${Vulkan_LIBRARIES})
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${CMAKE_PREFIX_PATH};")
endif()

set(PROJECT_SOURCES
    main.cpp
    change_wrong_image_suffix.hpp change_wrong_image_suffix.cpp change_wrong_image_suffix.ui
    check_images_state.hpp check_images_state.cpp check_images_state.ui
    cluster_image_viewer.hpp cluster_image_viewer.cpp cluster_image_viewer.ui
    cluster_similar_images.hpp cluster_similar_images.cpp cluster_similar_images.ui
    config/config_utils.hpp config/config_utils.cpp
    convert_label_img_labels.hpp convert_label_img_labels.cpp convert_label_img_labels.ui
    convert_yolo_detect_results.hpp convert_yolo_detect_results.cpp convert_yolo_detect_results.ui
    generate_image_list.hpp generate_image_list.cpp generate_image_list.ui
    image_cropper.hpp image_cropper.cpp image_cropper.ui
    image_mover.hpp image_mover.cpp image_mover.ui
    image_mover_mult.hpp image_mover_mult.cpp image_mover_mult.ui
    labels_check.hpp labels_check.cpp labels_check.ui
    labels_update.hpp labels_update.cpp
    mainwindow.hpp mainwindow.cpp mainwindow.ui
    mask_adjustment_widget.hpp mask_adjustment_widget.cpp mask_adjustment_widget.ui
    parser_label_img.hpp parser_label_img.cpp
    remove_images.hpp remove_images.cpp remove_images.ui
    roi/custom_graphics_view_pixmap.hpp roi/custom_graphics_view_pixmap.cpp
    roi/roi_configure.hpp roi/roi_configure.cpp roi/roi_configure.ui
    roi/roi_configure_data_creator.hpp roi/roi_configure_data_creator.cpp
    roi/roi_view.hpp roi/roi_view.cpp
    roi/rubber_band_base.hpp roi/rubber_band_base.cpp
    roi/rubber_band_points.hpp roi/rubber_band_points.cpp
    roi/rubber_band_rect.hpp roi/rubber_band_rect.cpp
    show_folders_details.hpp show_folders_details.cpp show_folders_details.ui
    split_data.hpp split_data.cpp split_data.ui
    utils/iterator_create.hpp utils/iterator_create.cpp
    utils/montage.hpp utils/montage.cpp
    utils/utility.hpp utils/utility.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}        
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET face_recognition_tasks APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    #find_package(ncnn REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE roi)

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          PRIVATE ${OPENCV_CV_PATH}/x64/vc17/lib/opencv_img_hash480.lib
                          #PRIVATE ncnn
                          PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia                          
                          PRIVATE Qt${QT_VERSION_MAJOR}::Xml
                          )
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
