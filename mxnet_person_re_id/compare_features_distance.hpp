#ifndef COMPARE_FEATURES_DISTANCE_HPP
#define COMPARE_FEATURES_DISTANCE_HPP

#include <mxnet-cpp/MxNetCpp.h>

class cosine_similarity
{
public:
    float compare_feature(mxnet::cpp::NDArray &lhs, mxnet::cpp::NDArray &rhs) const;
};

#endif // COMPARE_FEATURES_DISTANCE_HPP
