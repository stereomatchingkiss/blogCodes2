#ifndef FEATURES_UTILITY_HPP
#define FEATURES_UTILITY_HPP

#include <opencv2/core.hpp>

#include <vector>

class features_utility
{
public:
    using key_point_vec = std::vector<cv::KeyPoint>;

    features_utility();

    std::tuple<key_point_vec, key_point_vec, cv::Mat> find_homography(cv::Mat const &train, cv::Mat const &query);
    cv::Mat draw_match_points(cv::Mat const &train, cv::Mat const &query,
                              key_point_vec const &kpts_1, key_point_vec const &kpts_2,
                              cv::Mat const &homography);
};

#endif // FEATURES_UTILITY_HPP
