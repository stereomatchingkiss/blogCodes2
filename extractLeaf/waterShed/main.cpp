#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <imageAlgorithm.hpp>

//not a big deal to use global parameter or
//global typedef in this small program

using ContoursType = std::vector<std::vector<cv::Point> >;

std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

/**
 * @brief   segment all of the leafs
 * @param   the number of the images want to segment
 * @return  the leafs
 */
std::vector<cv::Mat> contours_method(std::initializer_list<std::string> number)
{
    std::vector<cv::Mat> results;
    for(auto const &num : number){
        cv::Mat color_image = cv::imread(Folder + "leaf" + num + ".png");
        if(color_image.empty()){
            std::cerr<<"input is empty"<<std::endl;
            return results;
        }

        cv::Mat image;
        cv::cvtColor(color_image, image, CV_BGR2GRAY);

        //step 1 : binarize the image
        cv::Mat binary;
        cv::threshold(image, binary, 140, 255, cv::THRESH_BINARY);

        //step 2 : remove small objects and noise
        auto const structure = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::Mat fore_ground;
        cv::erode(binary, fore_ground, structure, cv::Point(-1, -1), 4);

        //step 3 : find possible foreground(set as 255)
        ContoursType contours;
        cv::findContours(fore_ground, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        OCV::remove_contours(contours, 8000, 50000);
        fore_ground.setTo(0);
        cv::drawContours(fore_ground, contours, -1, cv::Scalar(255), CV_FILLED);
        cv::imwrite("foregroundFinal" + num + ".png", fore_ground);

        //step 4 : find possible background(set as 128)
        //         0 represent uncertain pixels
        cv::Mat back_ground;
        cv::dilate(binary, back_ground, structure, cv::Point(-1, -1), 4);
        cv::threshold(back_ground, back_ground, 1, 128, cv::THRESH_BINARY_INV);
        cv::imwrite("backGround" + num + ".png", back_ground);

        //step 5 : create markers and mask
        cv::Mat markers = back_ground + fore_ground;
        cv::imwrite("markers" + num + ".png", markers);
        cv::Mat mask;
        markers.convertTo(mask, CV_32S);
        //step 6 : apply watershed
        cv::watershed(color_image, mask);
        mask.convertTo(mask, CV_8U);
        cv::threshold(mask, mask, 150, 255, CV_THRESH_BINARY);
        cv::imwrite("mask" + num + ".png", mask);

        //step 7 : final results
        cv::Mat result;
        cv::imwrite("finalMask" + num + ".png", mask);
        color_image.copyTo(result, mask);
        cv::imwrite("result" + num + ".png", result);

        results.push_back(result);
    }

    return results;
}

/**
 * @brief cut the leafs to single leaf
 * @param input  input image
 * @param name   name of the image want to be saved
 */
void cut_to_single_leaf_simple(cv::Mat const &input, std::string const &name)
{
    cv::Mat mask;
    //step 1 : convert the input to gray image
    cv::cvtColor(input, mask, CV_BGR2GRAY);
    //step 2 : binarize it
    cv::threshold(mask, mask, 128, 255, CV_THRESH_BINARY);
    //step 3 : do serious erosion
    cv::erode(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)), cv::Point(-1, -1), 5);
    //step 4 : find and remove the outliers contours
    ContoursType contours;
    cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    OCV::remove_contours(contours, 500, 120000);
    //step 5 : process the contours one by one
    cv::Mat result;
    for(size_t i = 0; i != contours.size(); ++i){
        mask.setTo(0);
        cv::drawContours(mask, contours, i, cv::Scalar(255), CV_FILLED);
        //step 6 : dilate and close the contours
        cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)), cv::Point(-1, -1), 6);
        cv::morphologyEx(mask, mask, CV_MOP_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        //step 7 : generate result
        input.copyTo(result, mask);
        cv::imwrite(name + std::to_string(i) + ".png", result);
        cv::imshow("", result);
        cv::waitKey();
        result.setTo(0);
    }
}

int main()
{
    std::vector<cv::Mat> results = contours_method({"00", "01", "02"});
    cut_to_single_leaf_simple(results[1], "singleLeaf01_");

    return 0;
}

