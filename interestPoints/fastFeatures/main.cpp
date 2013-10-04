#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//not a big deal to use global parameter in this small program
std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

int main()
{
    cv::Mat image = cv::imread(Folder + "church01.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if(image.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return -1;
    }
    cv::imshow("image", image);

    std::vector<cv::KeyPoint> keyPoints;
    cv::FastFeatureDetector fast(40);
    fast.detect(image, keyPoints);

    cv::drawKeypoints(image, keyPoints, image, cv::Scalar(255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

    cv::imshow("Fast features", image);
    cv::waitKey();

    return 0;
}

