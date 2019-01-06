QT -= gui
QT += xml

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../../libs/
INCLUDEPATH += ../../

OPENCV_PATH = ../../../3rdLibs/opencv/opencv_3_4_2/opencv/build
MXNET_PATH = ../../../3rdLibs/incubator-mxnet/build/install

INCLUDEPATH += $${OPENCV_PATH}/include
INCLUDEPATH += $${MXNET_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib
LIBS += $${MXNET_PATH}/lib/libmxnet.lib

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    ../../libs/cv_format_parser/label_image_parser.cpp \
    ../../libs/cv_format_generator/label_image_generator.cpp \
    ../../libs/cv_format_parser/kaggle_face_detection_parser.cpp \
    ../../libs/cv_format_parser/json_utility.cpp \
    ../../mxnet_cpp_object_detection/object_detector.cpp \
    ../../mxnet_cpp_object_detection/object_detector_filter.cpp \
    ../../mxnet_cpp_object_detection/common.cpp \    
    generate_labels_from_kaggle_dataset.cpp \
    general_settings_param_parser.cpp \
    person_detector.cpp \
    ../../libs/cv_format_generator/mxnet_lst_generator.cpp \
    ../../libs/cv_format_generator/generate_lst_from_label_image.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../libs/cv_format_parser/label_image_parser.hpp \
    ../../libs/cv_format_generator/label_image_generator.hpp \
    ../../libs/cv_format_parser/kaggle_face_detection_parser.hpp \
    ../../libs/cv_format_parser/json_utility.hpp \
    ../../mxnet_cpp_object_detection/object_detector.hpp \
    ../../mxnet_cpp_object_detection/object_detector_filter.hpp \
    ../../mxnet_cpp_object_detection/common.hpp \
    generate_labels_from_kaggle_dataset.hpp \
    general_settings_param_parser.hpp \
    person_detector.hpp \
    ../../libs/cv_format_generator/mxnet_lst_generator.hpp \
    ../../libs/cv_format_generator/generate_lst_from_label_image.hpp
