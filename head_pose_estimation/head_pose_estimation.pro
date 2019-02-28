TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DLIB_PATH = ../../3rdLibs/dlib/dlib/build/install
OPENCV_PATH = ../../3rdLibs/opencv/opencv_3_4_2/opencv/build

INCLUDEPATH += ../..
INCLUDEPATH += $${DLIB_PATH}/include
INCLUDEPATH += $${OPENCV_PATH}/include

LIBS += $${DLIB_PATH}/lib/dlib.lib
LIBS += $${OPENCV_PATH}/x64/vc14/lib/opencv_world342.lib

CUDA_LIB = C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.2/lib/x64

LIBS += "$${CUDA_LIB}/cublas.lib"
LIBS += "$${CUDA_LIB}/cuda.lib"
LIBS += "$${CUDA_LIB}/cudart.lib"
LIBS += "$${CUDA_LIB}/cudnn.lib"
LIBS += "$${CUDA_LIB}/curand.lib"
LIBS += "$${CUDA_LIB}/cusolver.lib"

SOURCES += \
        main.cpp \
    ../libs/face/pose/head_pose_estimation.cpp \
    ../libs/face/detect/dlib_cnn_face_detector.cpp

HEADERS += \
    ../libs/face/pose/head_pose_estimation.hpp \
    ../libs/face/detect/dlib_cnn_face_detector.hpp \
    ../libs/face/detect/dlib_cnn_face_detector_params.hpp \
    ../libs/face/detect/dlib_face_detector_net.hpp \
    ../libs/face/pose/head_pose_info.hpp
