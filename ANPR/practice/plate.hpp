#ifndef PLATE_HPP
#define PLATE_HPP

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

struct plate
{    
    plate();
    plate(cv::Mat &img, cv::Rect const &pos);
    std::string str();

    std::vector<char> chars;
    std::vector<cv::Rect> charsPos;
    cv::Mat plateImg;
    cv::Rect position;
};

#endif // PLATE_HPP
