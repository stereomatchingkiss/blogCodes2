#include "hough_lines_polygon.hpp"

#include <ocv_libs/core/attribute.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

namespace{

void diff_files()
{
    cv::Mat map_00 = cv::imread("map00.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat map_01 = cv::imread("map01.jpg", cv::IMREAD_GRAYSCALE);
    std::cout<<map_00.size()<<", "<<map_01.size()<<std::endl;
    cv::resize(map_01, map_01, map_00.size());
    std::cout<<map_00.size()<<", "<<map_01.size()<<std::endl;
    cv::Mat diff = map_00 - map_01;
    cv::imshow("diff", diff);
    cv::waitKey();
}

void map_files()
{
    cv::Mat img = cv::imread("mapping.jpg");
    cv::imshow("img", img);


    cv::Mat binarize;
    cv::cvtColor(img, binarize, CV_BGR2GRAY);
    cv::threshold(binarize, binarize, 0, 255,
                  cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::imshow("binarize", binarize);

    cv::waitKey();
}

}

void mapping_files()
{
    cv::Mat map_01 = cv::imread("map01.jpg");
    cv::Mat binarize;
    cv::cvtColor(map_01, binarize, CV_BGR2GRAY);
    cv::GaussianBlur(map_01, map_01, {5,5}, 3,3);
    cv::threshold(binarize, binarize, 0, 255,
                  cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    auto mrect = cv::getStructuringElement(cv::MORPH_RECT, {3,3});
    cv::erode(binarize, binarize, mrect);
    mrect = cv::getStructuringElement(cv::MORPH_RECT, {7,5});
    cv::morphologyEx(binarize, binarize, cv::MORPH_DILATE, mrect, {-1,-1}, 7);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binarize.clone(), contours, CV_RETR_EXTERNAL,
                     CV_CHAIN_APPROX_SIMPLE);
    for(auto const &ct : contours){
        cv::rectangle(map_01, cv::boundingRect(ct), {255,0,0});
    }

    cv::imshow("draw map", map_01);
    cv::imshow("dilate", binarize);
    cv::waitKey();
}
