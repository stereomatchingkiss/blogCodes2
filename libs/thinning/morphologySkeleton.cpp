#include "morphologySkeleton.hpp"

#include <bugLocation.hpp>

namespace OCV
{

/**
 * @brief create skeleton by morphology operations
 * @param input  : input image
 * @param output : output image
 * @param kernel : structure element of the morphology operations
 */
void morphology_skeleton(cv::Mat &input, cv::Mat &output, cv::Mat const &kernel)
{
    if(input.type() != CV_8U){
        throw std::runtime_error(COMMON_DEBUG_MESSAGE + "input.type() != CV_8U");
    }

    if(input.data == output.data){
        output = cv::Mat::zeros(input.size(), CV_8U);
    }else{
        output.create(input.size(), CV_8U);
        output.setTo(0);
    }
    cv::Mat temp;
    cv::Mat eroded;
    bool done;
    do
    {
        cv::erode(input, eroded, kernel);
        cv::dilate(eroded, temp, kernel); // temp = open(img)
        cv::subtract(input, temp, temp);
        cv::bitwise_or(output, temp, output);
        eroded.copyTo(input);

        done = (cv::countNonZero(input) == 0);
    } while (!done);

    input = output;
}

}
