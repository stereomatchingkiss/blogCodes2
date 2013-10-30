#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <initializer_list>
#include <numeric>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

void draw_rects(cv::Mat &inout, std::vector<cv::Rect> const &rects);

/**
 * @brief generate points from the rectangle by clockwise order(top left,
 * top right, bottom right, bottom left)
 * @param rect   : input retangle
 * @param points : generated points
 */
template<typename Point>
void get_rect_points(cv::Rect const &rect, Point points[])
{
    points[0].x = rect.x;
    points[0].y = rect.y;
    points[1].x = rect.x + rect.width - 1;
    points[1].y = rect.y;
    points[2].x = points[1].x;
    points[2].y = rect.y + rect.height - 1;
    points[3].x = rect.x;
    points[3].y = points[2].y;
}

inline void mix_channels(cv::Mat const &src, cv::Mat &dst, std::initializer_list<int> from_to)
{
    cv::mixChannels(&src, 1, &dst, 1, std::begin(from_to), from_to.size() / 2);
}

void remove_contours(std::vector<std::vector<cv::Point> > &contours, double cmin, double cmax);

}

#endif // UTILITY_HPP
