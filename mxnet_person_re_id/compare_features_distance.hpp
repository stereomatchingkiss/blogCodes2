#ifndef COMPARE_FEATURES_DISTANCE_HPP
#define COMPARE_FEATURES_DISTANCE_HPP

#include <opencv2/core.hpp>

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
    float compare_feature(mxnet::cpp::NDArray const &lhs, mxnet::cpp::NDArray const &rhs, size_t size = 2048) const;
    float compare_feature(cv::Mat_<float> const &lhs, cv::Mat_<float> const &rhs);

private:
    cv::Mat_<float> lhs_pow_;
    cv::Mat_<float> numerator_;
    cv::Mat_<float> rhs_pow_;
};

#endif // COMPARE_FEATURES_DISTANCE_HPP
