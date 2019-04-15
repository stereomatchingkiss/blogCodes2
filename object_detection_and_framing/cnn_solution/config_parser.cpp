#include "config_parser.hpp"

#include "bg_subtractor_config.hpp"

#include <opencv2/core.hpp>

namespace{

cv::Size get_dilate_kernal_size(cv::FileNode const &input)
{
    return cv::Size(static_cast<int>(input["dilate_kernal_width"].real()),
            static_cast<int>(input["dilate_kernal_height"].real()));
}

cv::Size get_gaussian_kernal_size(cv::FileNode const &input)
{
    return cv::Size(static_cast<int>(input["gaussian_kernal_width"].real()),
            static_cast<int>(input["gaussian_kernal_height"].real()));
}

}

bg_subtractor_config parse_bg_subtractor_config(const std::string &filename)
{
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    auto const bnode = fs["bg_subtract"];
    bg_subtractor_config config;
    config.detect_shadow_ = bnode["detect_shadow"].real() == 0.0 ? false : true;
    config.dilate_kernal_size_ = get_dilate_kernal_size(bnode);
    config.gaussian_kernal_size_ = get_gaussian_kernal_size(bnode);
    config.history_ = static_cast<int>(bnode["history"].real());
    config.min_countour_area_ = bnode["min_contour_area"].real();
    config.resize_width_ = static_cast<int>(bnode["resize_width_to"].real());
    config.threshold_binary_ = bnode["threshold_binary"].real();
    config.thresh_val_ = bnode["threshold_var"].real();
    config.wait_between_frame_ms_ = static_cast<int>(bnode["wait_between_frame_ms"].real());

    return config;
}
