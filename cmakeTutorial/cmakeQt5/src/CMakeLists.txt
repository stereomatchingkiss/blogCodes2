include_directories(${HW_HEADER_DIR})

qt5_wrap_cpp(main-win-src ${HW_HEADER_DIR}/mainwindow.h)
qt5_wrap_cpp(whatever-src ${HW_HEADER_DIR}/whatever.h)
qt5_wrap_ui(main-win-ui ${HW_GUI_DIR}/MainWindow.ui)

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${main-win-src}
${main-win-ui} ${whatever-src})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui)
