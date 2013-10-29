#include "plate.hpp"

plate::plate(){
}

plate::plate(cv::Mat &img, cv::Rect const &pos) : plateImg(img), position(pos)
{}

plate::plate(plate &&data) noexcept
{
    *this = std::move(data);
}

plate& plate::operator=(plate &&data) noexcept
{
    chars = std::move(data.chars);
    charsPos = std::move(data.charsPos);
    plateImg = data.plateImg;
    position = data.position;

    return *this;
}

std::string plate::str()
{
    std::string result;

    return result;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/
