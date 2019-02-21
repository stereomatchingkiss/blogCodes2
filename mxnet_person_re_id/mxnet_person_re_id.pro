TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../

OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build
MXNET_PATH = ../../3rdLibs/incubator-mxnet/build/install

INCLUDEPATH += $${OPENCV_PATH}/include
INCLUDEPATH += $${MXNET_PATH}/include

LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib
LIBS += $${MXNET_PATH}/lib/libmxnet.lib

SOURCES += \
        main.cpp \    
    ../libs/mxnet/common.cpp \
    ../mxnet_cpp_object_detection/object_detector.cpp \
    ../mxnet_cpp_object_detection/object_detector_filter.cpp \    
    compare_features_distance.cpp \
    re_id_db.cpp \
    visitor_identify.cpp \
    person_feautres_extractor.cpp \
    ../libs/image_format_convert/opencv_to_ndarray.cpp

HEADERS += \    
    ../libs/mxnet/common.hpp \
    ../mxnet_cpp_object_detection/object_detector.hpp \
    ../mxnet_cpp_object_detection/object_detector_filter.hpp \    
    compare_features_distance.hpp \
    re_id_db.hpp \
    visitor_identify.hpp \
    person_feautres_extractor.hpp \
    ../libs/image_format_convert/opencv_to_ndarray.hpp
