TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DLIB_PATH = ../../3rdLibs/dlib/dlib/build/install
MXNET_PATH = ../../3rdLibs/incubator-mxnet/build/install
OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build

INCLUDEPATH += $${DLIB_PATH}/include
INCLUDEPATH += $${MXNET_PATH}/include
INCLUDEPATH += $${OPENCV_PATH}/include

CUDA_LIB = C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.2/lib/x64

LIBS += "$${CUDA_LIB}/cublas.lib"
LIBS += "$${CUDA_LIB}/cuda.lib"
LIBS += "$${CUDA_LIB}/cudart.lib"
LIBS += "$${CUDA_LIB}/cudnn.lib"
LIBS += "$${CUDA_LIB}/curand.lib"
LIBS += "$${CUDA_LIB}/cusolver.lib"

LIBS += $${DLIB_PATH}/lib/dlib.lib
LIBS += $${MXNET_PATH}/lib/libmxnet.lib
LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib

SOURCES += \
        main.cpp \
    common.cpp \
    face_detector.cpp \
    face_key_extractor.cpp \
    face_key.cpp

HEADERS += \
    common.hpp \
    face_detector.hpp \
    face_detector_net.hpp \
    face_key_extractor.hpp \
    face_key.hpp
