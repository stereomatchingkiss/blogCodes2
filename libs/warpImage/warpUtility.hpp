#ifndef WARP_UTILITY_HPP
#define WARP_UTILITY_HPP

#include <numeric>
#include <vector>

namespace OCV
{

/**
 * @brief   get the mass of the corners
 * @param   corners input corners
 * @return  the centers of the corners
 */
template<typename T>
inline T corners_center(std::vector<T> const &corners)
{
    return std::accumulate(std::begin(corners), std::end(corners), cv::Point(0, 0)) * (1. / corners.size());
}

/**
 *@brief convert the points between cv::Point_<T> and cv::Point_<U>.
 *@param src input points
 *@param dst the points after convert from src
 */
template<typename T, typename U>
void convert_points(std::vector<T> const &src, std::vector<U> &dst)
{
    dst.clear();
    for(auto const &point : src){
        dst.emplace_back(point.x, point.y);
    }
}

/**
 * @brief sort the corners of the rectangle to top left, top right, bottom right, bottom left.
 * users have to make sure the points of the corners are valid(is a rect), else the results
 * are undefined
 * @param corners input corners
 *
 */
template<typename T>
void sort_rect_corners(std::vector<T> &corners)
{
    std::vector<T> top, bot;
    T const center = corners_center(corners);
    for (size_t i = 0; i < corners.size(); i++){
        if (corners[i].y < center.y)
            top.emplace_back(corners[i]);
        else
            bot.emplace_back(corners[i]);
    }

    T tl = top[0].x > top[1].x ? top[1] : top[0];
    T tr = top[0].x > top[1].x ? top[0] : top[1];
    T bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
    T br = bot[0].x > bot[1].x ? bot[0] : bot[1];

    corners.clear();
    corners.emplace_back(tl);
    corners.emplace_back(tr);
    corners.emplace_back(br);
    corners.emplace_back(bl);
}

}

#endif // UTILITY_HPP
