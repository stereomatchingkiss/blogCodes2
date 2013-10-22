#include "marker.hpp"

#include <opencv2/imgproc/imgproc.hpp>

marker::marker(marker &&data) noexcept
{
    *this = std::move(data);
}

marker& marker::operator=(marker &&data) noexcept
{
    points_ = std::move(data.points_);

    return *this;
}

int marker::getMarkerId(cv::Mat &marker_image,int &n_rotations)
{
    CV_Assert(marker_image.rows == marker_image.cols);
    CV_Assert(marker_image.type() == CV_8UC1);

    cv::Mat grey = marker_image;
    //threshold image
    cv::threshold(grey, grey, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
