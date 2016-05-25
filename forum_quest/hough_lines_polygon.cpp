#include "hough_lines_polygon.hpp"

#include <ocv_libs/core/attribute.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

namespace{

/**
 * @brief create skeleton by morphology operations
 * @param input  : input image
 * @param output : output image
 * @param kernel : structure element of the morphology operations
 */
void morphology_skeleton(cv::Mat &input, cv::Mat &output,
                         cv::Mat const &kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)))
{
    if(input.type() != CV_8U){
        throw std::runtime_error("input.type() != CV_8U");
    }

    if(input.data == output.data){
        output = cv::Mat::zeros(input.size(), CV_8U);
    }else{
        output.create(input.size(), CV_8U);
        output.setTo(0);
    }
    cv::Mat temp;
    cv::Mat eroded;
    bool done;
    do{
        cv::erode(input, eroded, kernel);
        cv::dilate(eroded, temp, kernel); // temp = open(img)
        cv::subtract(input, temp, temp);
        cv::bitwise_or(output, temp, output);
        eroded.copyTo(input);

        done = (cv::countNonZero(input) == 0);
    }while(!done);

    input = output;
}

void detect_by_contour()
{
    //Following comments are written for non c++ programmers
    auto img = cv::imread("../forum_quest/data/yd8pA.png");
    if(img.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat gray_img;
    cv::cvtColor(img, gray_img, CV_BGR2GRAY);
    cv::Mat thin_img;
    //make your lines as thin as possible
    morphology_skeleton(gray_img, thin_img);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thin_img, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    //remove contour if the area less than 100
    auto it = std::remove_if(std::begin(contours), std::end(contours),
                   [](std::vector<cv::Point> const &a)
    {
        return cv::boundingRect(a).area() < 100;
    });
    //remove_if move unwanted elements to the backyard of the containers
    //you need to call the erase function of the containers to remove
    //unwanted elements
    contours.erase(it, std::end(contours));

    //contour_analyzer is a class used to print out statistic info
    //of the contour
    ocv::contour_analyzer analyzer;
    //print_contour_attribute_name print out the attribute names
    //of the contours as following
    //CArea   |   BArea   | Perimeter |   Aspect  |   Extent  |  Solidity |  PolySize
    ocv::print_contour_attribute_name(std::cout);
    for(size_t i = 0; i != contours.size(); ++i){
        cv::drawContours(img, contours, static_cast<int>(i), {0,255,0}, 2);        
        std::cout<<analyzer.analyze(contours[i], 0.1);
        cv::imshow("img", img);
        cv::waitKey();
    }
    cv::imwrite("polygon.jpg", img);
}

}

void hough_lines_polygon_detection()
{    
    detect_by_contour();
}
