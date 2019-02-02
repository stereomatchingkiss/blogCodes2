#include "compare_features_distance.hpp"

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace{

float ndarray_sum(const mxnet::cpp::NDArray &input, size_t size = 2048)
{
    float sum = 0;
    auto *data = input.GetData();
    for(size_t i = 0; i != size; ++i){
        sum += data[i];
    }

    return sum;
}

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

float cosine_similarity::compare_feature(mxnet::cpp::NDArray const &lhs, mxnet::cpp::NDArray const &rhs) const
{
    auto const numerator = element_wise_multiply_sum(lhs, rhs);
    auto const denom_lhs = std::sqrt(element_wise_multiply_sum(lhs, lhs));
    auto const denom_rhs = std::sqrt(element_wise_multiply_sum(rhs, rhs));

    auto const denominator = denom_lhs * denom_rhs;

    return denominator != 0.0f ? numerator / denominator : 0.0f;
}
