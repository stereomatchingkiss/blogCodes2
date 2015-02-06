include_directories(${HW_HEADER_DIR})

qt5_wrap_cpp(main-win-src ${HW_HEADER_DIR}/mainwindow.h)
qt5_wrap_cpp(whatever-src ${HW_HEADER_DIR}/whatever.h)
qt5_wrap_ui(main-win-ui ${HW_GUI_DIR}/MainWindow.ui)

aux_source_directory(. SRC_LIST)

set(incl_files ${HW_HEADER_DIR}/whatever.h ${HW_HEADER_DIR}/mainwindow.h)
set(moc_files ${main-win-src} ${whatever-src})
set(ui_files ${main-win-ui})
set(qt5_libs Qt5::Core Qt5::Widgets Qt5::Gui)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${moc_files}
${ui_files} ${incl_files})

target_link_libraries(${PROJECT_NAME} ${qt5_libs})
