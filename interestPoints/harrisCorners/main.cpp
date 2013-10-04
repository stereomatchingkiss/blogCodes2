#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "harrisDetector.hpp"

//not a big deal to use global parameter in this small program
std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

int main()
{
    cv::Mat image = cv::imread(Folder + "leafResult00.png", CV_LOAD_IMAGE_GRAYSCALE);
    if(image.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return -1;
    }
    cv::imshow("image", image);

    cv::Mat cornerStrength;
    cv::cornerHarris(image, cornerStrength, 3, 3, 0.01);
    cv::Mat harrisCorners;
    cv::threshold(cornerStrength, harrisCorners, 0.0001, 255, cv::THRESH_BINARY_INV);
    cv::imshow("corners", harrisCorners);

    // Create Harris detector instance
    harrisDetector harris;
    // Compute Harris values
    harris.detect(image);
    // Detect Harris corners
    std::vector<cv::Point> pts;
    harris.get_corners(pts, 0.01);
    cv::Mat image2 = image.clone();
    // Draw Harris corners
    harris.draw_on_image(image2, pts);
    // Display the corners
    cv::imshow("Harris Corners", image2);

    // Compute good features to track
    std::vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(image, corners,
                            200,    // maximum number of corners to be returned
                            0.005,   // quality level
                            10);    // minimum allowed distance between points

    cv::Mat image3 = image.clone();
    for(auto const &data : corners){
        cv::circle(image3, data, 3, cv::Scalar(255, 255, 255), 2);
    }
    cv::imshow("good features", image3);

    cv::waitKey();

    return 0;
}

