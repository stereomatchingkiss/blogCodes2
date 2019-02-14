#ifndef FACE_REG_INFO_HPP
#define FACE_REG_INFO_HPP

#include <opencv2/core.hpp>

#include <string>

struct face_reg_info
{
    double confident_ = -1.0;
    std::string id_;
    cv::Rect roi_;
};

#endif // FACE_REG_INFO_HPP
