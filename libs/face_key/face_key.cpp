#include "face_key.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <mxnet-cpp/MxNetCpp.h>

namespace mxnet_tool{

face_key::face_key(mxnet::cpp::NDArray key) :
    key_(key)
{

}

bool face_key::is_empty_key() const noexcept
{
    return key_.GetData() == nullptr;
}

double face_key::similarity(const face_key &input) const
{
    CV_Assert(key_.GetData() != nullptr && input.key_.GetData() != nullptr);

    cv::Mat_<float> const key1(1, 512, const_cast<float*>(input.key_.GetData()), 0);
    cv::Mat_<float> const key2(1, 512, const_cast<float*>(key_.GetData()), 0);
    auto const denominator = std::sqrt(key1.dot(key1) * key2.dot(key2));
    if(denominator != 0.0){
        return key2.dot(key1) / denominator;
    }

    return 0;
}

}
