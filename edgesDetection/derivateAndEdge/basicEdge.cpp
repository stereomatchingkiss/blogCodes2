#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "basicEdge.hpp"

basicEdge::basicEdge() :
    sobel_x_filter_{(cv::Mat_<float>(3, 3) << -1, 0, 1,
                                              -2, 0, 2,
                                              -1, 0, 1)},
    sobel_y_filter_{(cv::Mat_<float>(3, 3) << -1, -2, -1,
                                               0,  0,  0,
                                               1,  2,  1)}
{        
    input_ = cv::imread("/Users/Qt/program/blogsCodes/pic/uncompr.tif");
    if(input_.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return;
    }
}

void basicEdge::run(cv::Mat const &src)
{
    if(!src.empty()){
        input_ = src;
    }

    cv::Mat dst;

    cv::filter2D(input_, dst, CV_8U, sobel_x_filter_);
    cv::imwrite("sobel_x1.png", dst);
    cv::filter2D(input_, dst, CV_8U, sobel_y_filter_);
    cv::imwrite("sobel_y1.png", dst);

    cv::Sobel(input_, dst, CV_8U, 1, 1);
    cv::imwrite("sobel_x1_y1.png", dst);
}
