#ifndef FACE_KEY_EXTRACTOR_PARAMS_HPP
#define FACE_KEY_EXTRACTOR_PARAMS_HPP

#include <mxnet-cpp/MxNetCpp.h>

#include <string>

namespace ocv{

namespace face{

struct face_key_extractor_params
{
    face_key_extractor_params(std::string model_params,
                              std::string model_symbols,
                              mxnet::cpp::Context const &context,
                              mxnet::cpp::Shape const &shape = mxnet::cpp::Shape(4, 3, 112, 112)) :
        context_(context),
        model_params_(std::move(model_params)),
        model_symbols_(std::move(model_symbols)),        
        shape_(shape)
    {}

    mxnet::cpp::Context context_;
    std::string model_params_;
    std::string model_symbols_;    
    mxnet::cpp::Shape shape_;
};

}

}

#endif // FACE_KEY_EXTRACTOR_PARAMS_HPP
