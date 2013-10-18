#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <initializer_list>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

inline void mix_channels(cv::Mat const &src, cv::Mat &dst, std::initializer_list<int> from_to)
{
    cv::mixChannels(&src, 1, &dst, 1, std::begin(from_to), from_to.size() / 2);
}

void remove_contours(std::vector<std::vector<cv::Point> > &contours, double cmin, double cmax);

}

#endif // UTILITY_HPP
