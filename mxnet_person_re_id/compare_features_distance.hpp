#ifndef COMPARE_FEATURES_DISTANCE_HPP
#define COMPARE_FEATURES_DISTANCE_HPP

namespace mxnet
{

namespace cpp
{

class NDArray;

}

}

class cosine_similarity
{
public:
    float compare_feature(mxnet::cpp::NDArray const &lhs, mxnet::cpp::NDArray const &rhs) const;
};

#endif // COMPARE_FEATURES_DISTANCE_HPP
