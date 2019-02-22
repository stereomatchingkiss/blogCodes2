#include "insight_face_key_extractor.hpp"

#include "../libs/image_format_convert/dlib_to_array.hpp"
#include "../libs/mxnet/generic_predictor.hpp"

#include "insight_face_key.hpp"
#include "insight_face_key_extractor_params.hpp"

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace ocv{

namespace face{

insight_face_key_extractor::insight_face_key_extractor(insight_face_key_extractor_params const &params)
{
    predictor_ = std::make_unique<mxnet_aux::generic_predictor>(params.model_params_,
                                                                params.model_symbols_,
                                                                params.context_,
                                                                params.shape_);
}

insight_face_key_extractor::~insight_face_key_extractor()
{

}

insight_face_key insight_face_key_extractor::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    auto functor = [this](NDArray const &features, size_t batch_size)
    {
        return extract_key(features, batch_size);
    };

    return predictor_->forward<insight_face_key>(input, functor);
}

std::vector<insight_face_key> insight_face_key_extractor::
forward(const std::vector<dlib::matrix<dlib::rgb_pixel> > &input)
{
    auto functor = [this](NDArray const &features, size_t batch_size)
    {
        return extract_key(features, batch_size);
    };
    std::vector<insight_face_key> result;
    predictor_->forward(input, functor, result);

    return result;
}

std::vector<insight_face_key> insight_face_key_extractor::extract_key(const NDArray &features, size_t batch_size) const
{
    std::vector<insight_face_key> result;
    size_t constexpr feature_size = 512;
    Shape const shape(1, feature_size);
    for(size_t i = 0; i != batch_size; ++i){
        NDArray feature(features.GetData() + i * feature_size, shape, Context(kCPU, 0));
        //python scripts perform this step, but I don't find any difference even remove it
        //cv::Mat temp(1, 512, CV_32F, const_cast<mx_float*>(features.GetData()), 0);
        //cv::normalize(temp, temp, 1, 0, cv::NORM_L2);
        result.emplace_back(std::move(feature));
    }
    return result;
}

}

}
