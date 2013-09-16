#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main()
{
    cv::Mat input = cv::imread("/Users/Qt/program/blogsCodes/pic/childAndDogBinary.png");
    if(input.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return -1;
    }

    cv::Mat const structure_elem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat close_result;
    cv::morphologyEx(input, close_result, cv::MORPH_CLOSE, structure_elem);
    cv::Mat open_result;
    cv::morphologyEx(input, open_result, cv::MORPH_OPEN, structure_elem);

    cv::imwrite("close.png", close_result);
    cv::imwrite("open.png", open_result);

    return 0;
}

