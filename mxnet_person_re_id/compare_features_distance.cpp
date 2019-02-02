#include "compare_features_distance.hpp"

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

float cosine_similarity::compare_feature(mxnet::cpp::NDArray &lhs, mxnet::cpp::NDArray &rhs) const
{
    auto const numerator = element_wise_multiply_sum(lhs, rhs);
    lhs *= lhs;
    rhs *= rhs;

    auto const denominator = std::sqrt(ndarray_sum(lhs)) * std::sqrt(ndarray_sum(rhs));

    return denominator != 0.0f ? numerator / denominator : 0.0f;
}
