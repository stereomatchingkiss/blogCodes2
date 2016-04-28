TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../..

msvc:QMAKE_CXXFLAGS_RELEASE += /arch:AVX
msvc:QMAKE_CXXFLAGS_RELEASE += /openmp

#DEFINES += ARMA_USE_CXX11
DEFINES += ARMA_NO_DEBUG
#DEFINES += ARMA_USE_HDF5

include(../../pri/arma.pri)
include(../../pri/blas.pri)
include(../../pri/boost.pri)
include(../../pri/cv_dev.pri)
#include(../../pri/hdf5.pri)
include(../../pri/lapack.pri)
include(../../pri/rapidjson.pri)
include(../../pri/tbb.pri)

SOURCES += main.cpp \
    cbir_bovw.cpp \
    ../../ocv_libs/cbir/features_indexer.cpp \
    ../../ocv_libs/cbir/f2d_descriptor.cpp \
    ../../ocv_libs/file/utility.cpp \
    ../../ocv_libs/core/crop_images.cpp \
    ../../ocv_libs/core/montage.cpp

HEADERS += \
    cbir_bovw.hpp
