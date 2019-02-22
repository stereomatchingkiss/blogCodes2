#ifndef OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP
#define OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP

#include "insight_age_gender_info.hpp"
#include "../../mxnet/generic_predictor.hpp"

namespace ocv{

namespace face{

struct predict_age_gender_functor
{
    std::vector<insight_age_gender_info>
    operator()(const mxnet::cpp::NDArray &features, size_t batch_size) const;
};

using insight_age_gender_predict = mxnet_aux::generic_predictor<insight_age_gender_info, predict_age_gender_functor>;

}

}

#endif // OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP
