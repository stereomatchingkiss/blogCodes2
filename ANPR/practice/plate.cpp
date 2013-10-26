#include "plate.hpp"

plate::plate(){
}

plate::plate(cv::Mat &img, const cv::Rect &pos) : plateImg(img), position(pos)
{}

std::string plate::str()
{
    std::string result;

    return result;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/
