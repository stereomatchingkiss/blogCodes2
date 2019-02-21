#ifndef OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_PARAMS_HPP
#define OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_PARAMS_HPP

#include <mxnet-cpp/MxNetCpp.h>

#include <string>

namespace ocv{

namespace face{

struct insight_age_gender_predict_params
{
    insight_age_gender_predict_params(std::string model_params,
                                      std::string model_symbols,
                                      mxnet::cpp::Context const &context,
                                      mxnet::cpp::Shape const &shape = mxnet::cpp::Shape(4, 3, 112, 112)) :
        context_(context),
        model_params_(std::move(model_params)),
        model_symbols_(std::move(model_symbols)),
        shape_(shape)
    {}

    mxnet::cpp::Context const context_;
    std::string const model_params_;
    std::string const model_symbols_;
    mxnet::cpp::Shape const shape_;
};

}

}

#endif // OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_PARAMS_HPP
