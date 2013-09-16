#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Mat dilation(cv::Mat const &input)
{
    cv::Mat dilation_result = input.clone();
    //get the structuring of rectangle, size is 5 * 5
    cv::Mat const shape = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::dilate(input, dilation_result, shape);

    return dilation_result;
}

cv::Mat erosion(cv::Mat const &input)
{
    cv::Mat erosion_result = input.clone();
    //get the structuring of rectangle, size is 5 * 5
    cv::Mat const shape = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::erode(input, erosion_result, shape);

    return erosion_result;
}

int main()
{
    cv::Mat input = cv::imread("/Users/Qt/program/blogsCodes/pic/childAndDog2.png");
    if(input.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return -1;
    }

    cv::Mat dilation_result = dilation(input);
    cv::Mat erosion_result = erosion(input);
    cv::Mat dilation_again = dilation(dilation_result);
    cv::Mat erode_again = erosion(erosion_result);

    cv::imwrite("dilate.png", dilation_result);
    cv::imwrite("erode.png", erosion_result);
    cv::imwrite("dilate_again.png", dilation_again);
    cv::imwrite("erode_again.png", erode_again);

    return 0;
}
