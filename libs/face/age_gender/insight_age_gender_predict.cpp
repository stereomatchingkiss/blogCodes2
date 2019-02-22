#include "insight_age_gender_predict.hpp"

using namespace mxnet::cpp;

namespace ocv{

namespace face{

std::vector<insight_age_gender_info> predict_age_gender_functor::
operator()(const NDArray &features, size_t batch_size) const
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
