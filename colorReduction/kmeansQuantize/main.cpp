#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kmeansSegment.hpp"

void separate_adaptive_threshold(cv::Mat const &src)
{
    cv::Mat result = src.clone();

    cv::adaptiveThreshold(result, result, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 7);
    cv::imshow("adaptive gaussian", result);
    cv::imwrite("adativeGaussian.jpg", result);

    cv::adaptiveThreshold(result, result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 7, 7);
    cv::imshow("adaptive mean", result);
    cv::imwrite("adaptiveMean.jpg", result);

    cv::waitKey();
}

void sepearte_by_kmeans(cv::Mat const &src)
{
    kmeansSegment kmeans(3);
    cv::Mat result = kmeans.segment(src);

    cv::Mat binary;
    cv::Canny(result, binary, 30, 90);

    cv::imshow("original", src);
    cv::imshow("binary", binary);
    cv::imshow( "clustered image", result);
    cv::imwrite("kmeans_binary.jpg", binary);
    cv::imwrite("kmeans.jpg", result);
    cv::waitKey();
}

void separate_by_otsu(cv::Mat const &src)
{
    cv::Mat result = src.clone();

    cv::threshold(result, result, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    cv::imshow("otsu", result);
    cv::imwrite("otsu.jpg", result);
    cv::waitKey();
}

int main()
{
    cv::Mat src = cv::imread("/Users/Qt/program/blogsCodes/pic/perspective05.jpg");   
    if(src.empty()){
        std::cerr<<"can't read the image"<<std::endl;
        return -1;
    }

    cv::Mat binary;
    cv::cvtColor(src, binary, CV_BGR2GRAY);

    separate_adaptive_threshold(binary);
    sepearte_by_kmeans(src);
    separate_by_otsu(binary);

    return 0;
}

