#include "compare_features_distance.hpp"

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace{

float element_wise_multiply_sum(mxnet::cpp::NDArray const &lhs, mxnet::cpp::NDArray const &rhs, size_t size = 2048)
{
    float sum = 0;
    auto *lhs_data = lhs.GetData();
    auto *rhs_data = rhs.GetData();
    for(size_t i = 0; i != size; ++i){
        sum += lhs_data[i] * rhs_data[i];
    }

    return sum;
}

}

float cosine_similarity::compare_feature(mxnet::cpp::NDArray const &lhs,
                                         mxnet::cpp::NDArray const &rhs,
                                         size_t size) const
{
    auto const numerator = element_wise_multiply_sum(lhs, rhs, size);
    auto const denom_lhs = element_wise_multiply_sum(lhs, lhs, size);
    auto const denom_rhs = element_wise_multiply_sum(rhs, rhs, size);

    auto const denominator = std::sqrt(denom_lhs * denom_rhs);

    return denominator != 0.0f ? numerator / denominator : 0.0f;
}

float cosine_similarity::compare_feature(const cv::Mat_<float> &lhs, const cv::Mat_<float> &rhs) const
{
    cv::multiply(lhs, rhs, numerator_);
    cv::pow(lhs, 2, lhs_pow_);
    cv::pow(rhs, 2, rhs_pow_);

    auto const numerator = cv::sum(numerator_)[0];
    auto const denom_lhs = cv::sum(lhs_pow_)[0];
    auto const denom_rhs = cv::sum(rhs_pow_)[0];

    auto const denominator = std::sqrt(denom_lhs * denom_rhs);

    return static_cast<float>(denominator != 0.0 ? numerator / denominator : 0.0);
}
