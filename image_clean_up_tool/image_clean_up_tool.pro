QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

OPENCV_PATH = ../../3rdLibs/opencv/dev/opencv/build_cpu

INCLUDEPATH += ../../3rdLibs
INCLUDEPATH += $${OPENCV_PATH}/install/include

LIBS += $${OPENCV_PATH}/install/x64/vc15/lib/opencv_img_hash430.lib
LIBS += $${OPENCV_PATH}/install/x64/vc15/lib/opencv_world430.lib

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    convert_label_img_labels.cpp \
    convert_yolo_detect_results.cpp \
    generate_image_list.cpp \
    image_mover.cpp \
    labels_check.cpp \
    main.cpp \
    mainwindow.cpp \
    parser_label_img.cpp \
    remove_images.cpp \
    split_data.cpp \
    utils/iterator_create.cpp

HEADERS += \
    convert_label_img_labels.hpp \
    convert_yolo_detect_results.hpp \
    generate_image_list.hpp \
    image_mover.hpp \
    labels_check.hpp \
    mainwindow.hpp \
    parser_label_img.hpp \
    remove_images.hpp \
    split_data.hpp \
    utils/iterator_create.hpp

FORMS += \
    convert_label_img_labels.ui \
    convert_yolo_detect_results.ui \
    generate_image_list.ui \
    image_mover.ui \
    labels_check.ui \
    mainwindow.ui \
    remove_images.ui \
    split_data.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
