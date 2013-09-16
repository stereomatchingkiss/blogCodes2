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
    dilation_result = dilation(dilation_result);
    erosion_result = erosion(erosion_result);

    cv::imshow("dilate", dilation_result);
    cv::imshow("erode", erosion_result);
    cv::imshow("original", input);

    cv::imwrite("dilate.png", dilation_result);
    cv::imwrite("erode.png", erosion_result);

    /*cv::cvtColor(input, input, CV_BGR2GRAY);
    cv::threshold(input, input, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    cv::imshow("", input);
    cv::imwrite("/Users/Qt/program/blogsCodes/pic/childAndDog2.png", input);
    cv::waitKey();*/

    return 0;
}
