#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <imageAlgorithm.hpp>

//not a big deal to use global parameter or
//global typedef in this small program

typedef std::vector<std::vector<cv::Point> > ContoursType;

std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

int contours_method(std::initializer_list<std::string> number)
{
    for(auto const &num : number){
        cv::Mat color_image = cv::imread(Folder + "leaf" + num + ".png");
        if(color_image.empty()){
            std::cerr<<"input is empty"<<std::endl;
            return -1;
        }
        cv::imshow("image", color_image);

        cv::Mat image;
        cv::cvtColor(color_image, image, CV_BGR2GRAY);

        cv::Mat binary;
        cv::threshold(image, binary, 140, 255, cv::THRESH_BINARY);
        cv::imwrite("binaryOrigin" + num + ".png", binary);

        auto const structure = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::erode(binary, binary, structure);
        cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, structure);
        cv::imshow("binary", binary);
        cv::imwrite("binaryMorph" + num + ".png", binary);

        ContoursType contours;
        cv::findContours(binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        cv::Mat mask(image.size(), CV_8U, cv::Scalar(255));
        cv::drawContours(mask, contours, -1, cv::Scalar(0), CV_FILLED);
        cv::imwrite("rawContours" + num + ".png", mask);

        OCV::remove_contours(contours, 10000, 50000);
        mask.setTo(255);
        cv::drawContours(mask, contours, -1, cv::Scalar(0), CV_FILLED);

        cv::Mat result;
        mask = ~mask; //invert mask
        cv::imwrite("finalMask" + num + ".png", mask);
        color_image.copyTo(result, mask);

        cv::imshow("result", result);
        cv::imwrite("result" + num + ".png", result);

        cv::waitKey();
    }

    return 0;
}

int main()
{
    try{
        contours_method({"00", "01", "02"});
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}

