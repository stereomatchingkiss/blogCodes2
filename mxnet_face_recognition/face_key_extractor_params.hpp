#ifndef FACE_KEY_EXTRACTOR_PARAMS_HPP
#define FACE_KEY_EXTRACTOR_PARAMS_HPP

#include <mxnet-cpp/MxNetCpp.h>

#include <string>

namespace mxnet_tool{

struct face_key_extractor_params
{
    face_key_extractor_params(std::string model_params,
                              std::string model_symbols,
                              mxnet::cpp::Context context) :
        model_params_(std::move(model_params)),
        model_symbols_(std::move(model_symbols)),
        context_(context)
    {}

    std::string model_params_;
    std::string model_symbols_;
    mxnet::cpp::Context context_;
};

}

#endif // FACE_KEY_EXTRACTOR_PARAMS_HPP
