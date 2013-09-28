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

        //binarize the image
        cv::Mat binary;
        cv::threshold(image, binary, 140, 255, cv::THRESH_BINARY);

        //remove small objects and noise
        auto const structure = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::Mat fore_ground;
        cv::erode(binary, fore_ground, structure, cv::Point(-1, -1), 4);
        //fill the holes of foreground
        //cv::morphologyEx(fore_ground, fore_ground, cv::MORPH_CLOSE, structure);

        //possible foreground(set as 255)
        ContoursType contours;
        cv::findContours(fore_ground, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        OCV::remove_contours(contours, 8000, 50000);
        fore_ground.setTo(0);
        cv::drawContours(fore_ground, contours, -1, cv::Scalar(255), CV_FILLED);
        cv::imshow("foregroundFinal", fore_ground);
        cv::imwrite("foregroundFinal" + num + ".png", fore_ground);

        //possible background(set as 128)
        //0 represent uncertain pixels
        cv::Mat back_ground;
        cv::dilate(binary, back_ground, structure, cv::Point(-1, -1), 4);
        cv::threshold(back_ground, back_ground, 1, 128, cv::THRESH_BINARY_INV);
        cv::imshow("backGround", back_ground);
        cv::imwrite("backGround" + num + ".png", back_ground);

        //create markers and mask
        cv::Mat markers = back_ground + fore_ground;
        cv::imshow("markers", markers);
        cv::imwrite("markers" + num + ".png", markers);
        cv::Mat mask;
        markers.convertTo(mask, CV_32S);
        cv::watershed(color_image, mask);
        mask.convertTo(mask, CV_8U);
        cv::threshold(mask, mask, 150, 255, CV_THRESH_BINARY);
        cv::imshow("mask", mask);
        cv::imwrite("mask" + num + ".png", mask);

        //final results
        cv::Mat result;
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
    contours_method({"00", "01", "02"});

    return 0;
}

