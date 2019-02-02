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
    float compare_feature(mxnet::cpp::NDArray &lhs, mxnet::cpp::NDArray &rhs) const;
};

#endif // COMPARE_FEATURES_DISTANCE_HPP
