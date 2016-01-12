#include "croatia_general_plate_recognizer.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>

croatia_general_plate_recognizer::croatia_general_plate_recognizer() :
    pattern_("\\w{2}\\d{3,4}\\w{1,2}")
{

}

std::string croatia_general_plate_recognizer::fit(const std::string &pattern) const
{
    for(auto it = std::sregex_iterator(std::begin(pattern),
                                       std::end(pattern), pattern_);
        it != std::sregex_iterator(); ++it){
        std::cout << "  " << it->str() << '\n';
    }

    return {};
}

void croatia_general_plate_recognizer::
sort_contour(contours_type &contours) const
{
    std::sort(std::begin(contours), std::end(contours),
              [](auto const &lhs, auto const &rhs)
    {
        return cv::boundingRect(lhs).x < cv::boundingRect(rhs).x;
    });
}
