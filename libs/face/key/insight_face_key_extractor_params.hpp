#ifndef OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_PARAMS_HPP
#define OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_PARAMS_HPP

#include <mxnet-cpp/MxNetCpp.h>

#include <string>

namespace ocv{

namespace face{

struct insight_face_key_extractor_params
{
    insight_face_key_extractor_params(std::string model_params,
                                      std::string model_symbols,
                                      mxnet::cpp::Context const &context,
                                      mxnet::cpp::Shape const &shape = mxnet::cpp::Shape(4, 3, 112, 112)) :
        context_(context),
        model_params_(std::move(model_params)),
        model_symbols_(std::move(model_symbols)),
        shape_(shape)
    {}
    ~insight_face_key_extractor_params(){}

    mxnet::cpp::Context context_;
    std::string model_params_;
    std::string model_symbols_;
    mxnet::cpp::Shape shape_;
};

}

}

#endif // OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_PARAMS_HPP
