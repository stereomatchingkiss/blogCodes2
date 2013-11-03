#include "utility.hpp"

namespace OCV
{

void draw_rect(cv::Mat &inout, cv::Rect const &rect, cv::Scalar const &color)
{
    cv::Point2f points[4];
    get_rect_points(rect, points);
    cv::line(inout, {points[0].x, points[0].y}, {points[1].x, points[1].y}, color, 2);
    cv::line(inout, {points[1].x, points[1].y}, {points[2].x, points[2].y}, color, 2);
    cv::line(inout, {points[2].x, points[2].y}, {points[3].x, points[3].y}, color, 2);
    cv::line(inout, {points[3].x, points[3].y}, {points[0].x, points[0].y}, color, 2);
}

void draw_rects(cv::Mat &inout, std::vector<cv::Rect> const &rects, cv::Scalar const &color)
{
    cv::Point2f points[4];
    for(auto const &rect : rects){
        get_rect_points(rect, points);
        cv::line(inout, {points[0].x, points[0].y}, {points[1].x, points[1].y}, color, 2);
        cv::line(inout, {points[1].x, points[1].y}, {points[2].x, points[2].y}, color, 2);
        cv::line(inout, {points[2].x, points[2].y}, {points[3].x, points[3].y}, color, 2);
        cv::line(inout, {points[3].x, points[3].y}, {points[0].x, points[0].y}, color, 2);
    }
}

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
       auto const size = cv::contourArea(data);
       return !(size < cmin || size > cmax);
    });
    contours.erase(it, std::end(contours));
}

}
