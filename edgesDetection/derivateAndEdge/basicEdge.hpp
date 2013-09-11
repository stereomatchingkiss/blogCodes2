#ifndef BASICEDGE_HPP
#define BASICEDGE_HPP

#include <opencv2/core/core.hpp>

class basicEdge
{
public:
    basicEdge();

    void run(cv::Mat const &src = cv::Mat());

private:
    cv::Mat input_;

    cv::Mat const sobel_x_filter_;
    cv::Mat const sobel_y_filter_;
};

#endif // BASICEDGE_HPP
