#include "player_detector.hpp"

#include <opencv2/imgproc.hpp>

player_detector::player_detector(size_t max_player) :
    max_player_(max_player),
    min_area_(700)
{
    bgs_ = cv::createBackgroundSubtractorMOG2();
    //bgs_ = cv::bgsegm::createBackgroundSubtractorGMG(20, 0.7);
    auto *ptr =
            static_cast<cv::BackgroundSubtractorMOG2*>(bgs_.get());
    //ptr->setShadowThreshold(0.7);
    ptr->setShadowValue(0);
}

std::vector<cv::Rect2d> player_detector::
search_simple(const cv::Mat &input)
{
    warm_up(input);
    cv::findContours(fmask_, contours_, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect2d> locations;
    for(auto const &contour : contours_){
        auto const rect = cv::boundingRect(contour);
        if(rect.area() > min_area_){
            locations.emplace_back(rect);
        }
    }

    return locations;
}

std::vector<cv::Rect2d> player_detector::
search(const cv::Mat &input)
{
    std::vector<cv::Rect2d> locations = search_simple(input);
    std::sort(std::begin(locations), std::end(locations),
              [](cv::Rect2d const &lhs, cv::Rect2d const &rhs)
    {
        return lhs.area() > rhs.area();
    });
    locations.resize(std::min(max_player_, locations.size()));

    return locations;
}

void player_detector::set_min_area(double value)
{
    min_area_ = value;
}

void player_detector::warm_up(const cv::Mat &input)
{
    cv::cvtColor(input, gray_img_, CV_BGR2GRAY);
    bgs_->apply(gray_img_, fmask_);
}
