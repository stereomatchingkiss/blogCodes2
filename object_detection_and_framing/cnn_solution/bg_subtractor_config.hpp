#ifndef BG_SUBTRACTOR_CONFIG_HPP
#define BG_SUBTRACTOR_CONFIG_HPP

#include <opencv2/core.hpp>

struct bg_subtractor_config
{
    double threshold_binary_ = 30;
    bool detect_shadow_ = false;
    cv::Size dilate_kernal_size_ = cv::Size(3, 3);
    cv::Size gaussian_kernal_size_ = cv::Size(21, 21);
    int history_ = 60;
    double min_countour_area_ = 8000;
    cv::Size resize_to_;
    int resize_width_ = 0;
    double thresh_val_ = 30;
    double var_threshold_ = 32;
    int wait_between_frame_ms_ = 1;
};

#endif // BG_SUBTRACTOR_CONFIG_HPP
