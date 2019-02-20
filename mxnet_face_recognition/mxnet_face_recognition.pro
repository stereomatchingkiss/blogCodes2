TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

QT += core

DLIB_PATH = ../../3rdLibs/dlib/dlib/build/install
MXNET_PATH = ../../3rdLibs/incubator-mxnet/build/install
OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build

INCLUDEPATH += ../..
INCLUDEPATH += $${BOOST_PATH}
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
    ../libs/mxnet/common.cpp \
    face_reg_db.cpp \
    face_recognition.cpp \            
    ../libs/face_detect/dlib_cnn_face_detector.cpp \
    ../libs/face_key/insight_face_key.cpp \
    ../libs/face_key/insight_face_key_extractor.cpp

HEADERS += \
    ../libs/mxnet/common.hpp \
    face_reg_db.hpp \
    face_recognition.hpp \        
    face_reg_info.hpp \
    ../libs/image_format_convert/dlib_to_array.hpp \    
    ../libs/face_detect/dlib_cnn_face_detector.hpp \
    ../libs/face_detect/dlib_cnn_face_detector_params.hpp \
    ../libs/face_detect/dlib_face_detector_net.hpp \
    ../libs/face_key/insight_face_key.hpp \
    ../libs/face_key/insight_face_key_extractor.hpp \
    ../libs/face_key/insight_face_key_extractor_params.hpp
