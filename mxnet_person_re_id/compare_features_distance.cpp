#include "compare_features_distance.hpp"

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

float cosine_similarity::compare_feature(mxnet::cpp::NDArray const &lhs,
                                         mxnet::cpp::NDArray const &rhs,
                                         size_t size) const
{
    cv::Mat_<float> const key1(1, static_cast<int>(size), const_cast<float*>(lhs.GetData()), 0);
    cv::Mat_<float> const key2(1, static_cast<int>(size), const_cast<float*>(rhs.GetData()), 0);

    return compare_feature(lhs, rhs);
}

float cosine_similarity::compare_feature(const cv::Mat_<float> &lhs, const cv::Mat_<float> &rhs)
{
    auto const denominator = std::sqrt(lhs.dot(lhs) * rhs.dot(rhs));
    if(denominator != 0.0){
        return static_cast<float>(lhs.dot(rhs) / denominator);
    }

    return 0;
}
