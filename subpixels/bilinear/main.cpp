#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

float get_sub_pix(cv::Mat const &img, cv::Point2f const &pt)
{
    int x = static_cast<int>(pt.x);
    int y = static_cast<int>(pt.y);

    int x0 = cv::borderInterpolate(x,   img.cols, cv::BORDER_REPLICATE);
    int x1 = cv::borderInterpolate(x+1, img.cols, cv::BORDER_REPLICATE);
    int y0 = cv::borderInterpolate(y,   img.rows, cv::BORDER_REPLICATE);
    int y1 = cv::borderInterpolate(y+1, img.rows, cv::BORDER_REPLICATE);

    float a = pt.x - static_cast<float>(x);
    float c = pt.y - static_cast<float>(y);

    float x1_interpolate = (img.at<uchar>(y0, x0) * (1.0 - a) + img.at<uchar>(y0, x1) * a);
    float x2_interpolate = (img.at<uchar>(y1, x0) * (1.0 - a) + img.at<uchar>(y1, x1) * a);
    float target = x1_interpolate * (1.0 - c) + x2_interpolate * c;

    return target;
}


int main()
{
    cv::Mat_<uchar> mat = (cv::Mat_<uchar>(2, 2) <<
                          91, 210,
                          162, 95);

    float result = get_sub_pix(mat, cv::Point2f(0.5, 0.2));
    std::cout<<result<<std::endl; //print 146.1

    return 0;
}

