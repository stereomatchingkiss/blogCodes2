#include "insight_face_key_extractor.hpp"

using namespace mxnet::cpp;

namespace ocv{

namespace face{

std::vector<insight_face_key> extract_face_key_functor::operator()(const NDArray &features, size_t batch_size) const
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
