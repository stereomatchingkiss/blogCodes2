TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += DLIB_USE_CUDA
DEFINES += DLIB_JPEG_SUPPORT
DEFINES += DLIB_PNG_SUPPORT

INCLUDEPATH += ../../../..
INCLUDEPATH += ../../../../3rdLibs/rapidjson/include

linux-g++ {

DEFINES += DLIB_USE_BLAS
DEFINES += DLIB_USE_LAPACK

LIBS += -L/usr/local/cuda-8.0/lib64 -lcuda -lcudart -lcurand -lcublas -lcudnn
LIBS += -L/usr/lib -llapack -lblas

}

win32-msvc2015{

QMAKE_CXXFLAGS_RELEASE += /openmp /arch:AVX
QMAKE_CFLAGS_RELEASE += /openmp /arch:AVX

CUDA_LIB = C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64

LIBS += "$${CUDA_LIB}/cublas.lib"
LIBS += "$${CUDA_LIB}/cuda.lib"
LIBS += "$${CUDA_LIB}/cudart.lib"
LIBS += "$${CUDA_LIB}/cudnn.lib"
LIBS += "$${CUDA_LIB}/curand.lib"

}

include(../../../../pri/dlibs.pri)

SOURCES += main.cpp \        
    ../../general_functions/dlib/utility.cpp \
    read_input_data.cpp \
    feature_extract_trainer.cpp \    
    ../../general_functions/dlib/feature_extractor_resnet34.cpp \
    ../../../../ocv_libs/dlib/get_img_info.cpp \
    setting_reader.cpp

HEADERS += \
    ../../../../ocv_libs/dlib/get_img_info.hpp \
    ../../../../ocv_libs/ml/utility/split_train_test.hpp \
    ../../../../ocv_libs/ml/utility/shuffle_data.hpp \
    ../../../../ocv_libs/dlib/cv_to_dlib.hpp \
    ../../general_functions/dlib/random_crop.hpp \
    ../../general_functions/dlib/utility.hpp \
    read_input_data.hpp \
    print_result.hpp \
    feature_extract_trainer.hpp \
    transfer_net.hpp \    
    ../../general_functions/dlib/resnet34.hpp \
    ../../general_functions/dlib/feature_extractor_resnet34.hpp \
    setting_reader.hpp \
    global_var.hpp
