#include "utility.hpp"

namespace OCV
{

/**
 *@brief convert the image to gray image
 *  construct and return the result directly
 *  determine appropriate color space conversion code at runtime
 *
 *@param src :input image
 *@param dst : output
 *@param copy : true : copy the data of src to dst if the channel of src is CV_8U or CV_8S;
 *false : src and dst share the same data if the channel of src is CV_8U or CV_8S
 *@param flag : CV_BGR2GRAY or CV_RGB2GRAY and so on
 *@param dstCn : number of channels in the destination image; if the parameter is 0,
 *  the number of the channels is derived automatically.
 *
 *@return
 * will return an empty Mat if cannot or don't need to convert to gray scale
 */
void cvt_to_gray(cv::Mat &src, cv::Mat &dst, bool copy, int dstCn, int flag)
{
    if(src.type() == CV_8UC3 || src.type() == CV_8SC3){
        cv::cvtColor(src, dst, flag, dstCn);
    }else if(src.type() == CV_8UC4 || src.type() == CV_8SC4){
        cv::cvtColor(src, dst, flag, dstCn);
    }else if(src.type() == CV_8U || src.type() == CV_8S){
        if(copy){
            src.copyTo(dst);
        }else{
            dst = src;
        }
    }
}

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
    for(auto const &rect : rects){
        draw_rect(inout, rect, color);
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
