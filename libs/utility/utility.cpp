#include <cmath>

#include "utility.hpp"

namespace OCV
{

/**
 * @brief remove the contours by contours area(pixels surrounded by the contours)
 * @param contours : the contours
 * @param cmin : contour size lower than cmin will be removed
 * @param cmax : contour size higher than cmax will be removed
 */
void remove_contours(std::vector<std::vector<cv::Point> > &contours, double cmin, double cmax)
{
    auto it = std::partition(std::begin(contours), std::end(contours), [=](std::vector<cv::Point> const &data)
    {
       auto const size = std::abs(cv::contourArea(data));
       return !(size < cmin || size > cmax);
    });
    contours.erase(it, std::end(contours));
}

}
