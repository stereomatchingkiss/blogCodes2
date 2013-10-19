#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <warpImage/warpUtility.hpp>

#define DRAW_OUTPUT

using ContourType = std::vector<std::vector<cv::Point> >;
using CornersType = std::vector<std::vector<cv::Point> >;

/**
 * @brief find the contours of images
 * @param binary_input binary input image
 * @return the contours
 */
ContourType find_contours(cv::Mat const &binary_input)
{
    ContourType contours;
    cv::findContours(binary_input, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    return contours;
}

/**
 * @brief get the corners of the quadrilaterals
 * @param contours input contours
 * @return the corners of the quadrilaterals
 */
CornersType get_corners(ContourType &contours)
{
    OCV::remove_contours(contours, 100, 50000);
    std::vector<cv::Point> approx;
    CornersType corners;
    for(auto const &data : contours){
        cv::approxPolyDP(data, approx, cv::arcLength(data, true) * 0.02, true);
        if(approx.size() == 4 && cv::isContourConvex(approx)){
            OCV::sort_rect_corners(approx);
            corners.emplace_back(std::move(approx));
        }
    }

    return corners;
}

int main()
{    
    cv::Mat src = cv::imread("/Users/Qt/program/blogsCodes/pic/perspective08.jpg");
    if (src.empty()){
        std::cerr<<"can't open image"<<std::endl;
        return - 1;
    }

    //step 1 : convert the image to binary by otsu threshold
    cv::Mat binary;
    cv::cvtColor(src, binary, CV_BGR2GRAY);
    cv::threshold(binary, binary, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
#ifdef DRAW_OUTPUT
    cv::imshow("otsu", binary);
    cv::imwrite("otsu.jpg", binary);
#endif

    //step 2 : find the contours of binary image
    auto contours = find_contours(binary);
#ifdef DRAW_OUTPUT
    cv::Mat contour_map = cv::Mat::zeros(binary.size(), CV_8U);
    cv::drawContours(contour_map, contours, -1, cv::Scalar(255));
    cv::imshow("contour map", contour_map);
    cv::imwrite("contour_map.jpg", contour_map);
#endif

    //step 3 : find the corners of the quadrilaterals
    auto const corners = get_corners(contours);

#ifdef DRAW_OUTPUT
    cv::Mat src_corners = src.clone();
    cv::Scalar color[] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255} };
    for(size_t i = 0; i != corners.size(); ++i){
        for(auto const &point : corners[i]){
            cv::circle(src_corners, point, 3, color[i % 3]);
        }
    }
    cv::imshow("src corners", src_corners);
    cv::imwrite("corners.jpg", src_corners);
#endif

    //step 4 : correct the perspective distortion of the quadrilateral by warpPerspective
    cv::Mat target(150, 150, src.type());
    std::vector<cv::Point2f> target_points{{0, 0}, {target.cols - 1, 0},
                                           {target.cols - 1, target.rows - 1}, {0, target.rows - 1}};
    std::vector<cv::Point2f> points;
    for(size_t i = 0; i != corners.size(); ++i){
        OCV::convert_points(corners[i], points);
        cv::Mat const trans_mat =cv::getPerspectiveTransform(points, target_points);
        cv::warpPerspective(src, target, trans_mat, target.size());
#ifdef DRAW_OUTPUT
        std::stringstream name;
        name << "target" << std::setw(2) << std::setfill('0') << i <<".jpg";
        cv::imshow("target", target);
        cv::imwrite(name.str(), target);
        cv::waitKey();
#endif
    }

#ifdef DRAW_OUTPUT
    cv::waitKey();
#endif

    return 0;
}
