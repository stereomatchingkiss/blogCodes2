#include "insight_age_gender_predict.hpp"

#include "insight_age_gender_predict_params.hpp"

#include "../libs/image_format_convert/dlib_to_array.hpp"
#include "../libs/mxnet/generic_predictor.hpp"

using namespace mxnet::cpp;

namespace ocv{

namespace face{

insight_age_gender_predict::insight_age_gender_predict(insight_age_gender_predict_params params)
{
    predictor_ = std::make_unique<mxnet_aux::generic_predictor>(params.model_params_,
                                                                params.model_symbols_,
                                                                params.context_,
                                                                params.shape_);
}

insight_age_gender_predict::~insight_age_gender_predict()
{

}

insight_age_gender_info insight_age_gender_predict::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    auto functor = [this](NDArray const &features, size_t batch_size)
    {
        return predict_age_gender(features, batch_size);
    };

    return predictor_->forward<insight_age_gender_info>(input, functor);
}

std::vector<insight_age_gender_info> insight_age_gender_predict::
forward(const std::vector<dlib::matrix<dlib::rgb_pixel> > &input)
{    
    auto functor = [this](NDArray const &features, size_t batch_size)
    {
        return predict_age_gender(features, batch_size);
    };
    std::vector<insight_age_gender_info> result;
    predictor_->forward(input, functor, result);

    return result;
}

std::vector<insight_age_gender_info> insight_age_gender_predict::predict_age_gender(const NDArray &features,
                                                                                    size_t batch_size) const
{
    std::vector<insight_age_gender_info> result;
    int constexpr features_size = 202;
    for(size_t i = 0; i != batch_size; ++i){
        auto const *ptr = features.GetData() + i * features_size;
        insight_age_gender_info info;
        info.gender_ = ptr[0] > ptr[1] ? gender_info::female_ : gender_info::male_;
        for(int i = 2; i < features_size; i += 2){
            if(ptr[i + 1] > ptr[i]){
                info.age_ += 1;
            }
        }
        result.emplace_back(info);
    }
    return result;
}

}

}
