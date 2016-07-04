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
    cv::Mat const map_01 = cv::imread("map01.jpg");
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
    auto rit = std::remove_if(std::begin(contours), std::end(contours),
                   [](auto const &ct)
    {
        auto const rect = cv::boundingRect(ct);
        return rect.height > rect.width;
    });
    contours.erase(rit, std::end(contours));

    cv::Mat draw_map = map_01.clone();
    for(auto const &ct : contours){
        //get the information of each rect
        cv::Mat temp = map_01.clone();
        std::cout<<cv::boundingRect(ct)<<std::endl;
        cv::rectangle(temp, cv::boundingRect(ct), {255,0,0});
        cv::imshow("temp", temp);
        cv::waitKey();
        cv::destroyAllWindows();

        //draw_map is used to show all of the rect at once
        cv::rectangle(draw_map, cv::boundingRect(ct), {255,0,0});
    }

    cv::imshow("draw map", draw_map);
    cv::imshow("dilate", binarize);
    cv::imwrite("draw_map.jpg", draw_map);
    cv::waitKey();
}
