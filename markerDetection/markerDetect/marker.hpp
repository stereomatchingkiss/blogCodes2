#ifndef MARKER_HPP
#define MARKER_HPP

#include <vector>

#include <opencv2/core/core.hpp>

class marker
{
public:
    marker() = default;
    marker(marker &&data) noexcept;
    marker& operator=(marker &&data) noexcept;

    static int getMarkerId(cv::Mat &marker_image,int &n_rotations);

public:
    std::vector<cv::Point2f> points_;
};

#endif // MARKER_HPP
