#include "correlation_trackers.hpp"

#include <dlib/opencv/cv_image.h>

#include <opencv2/imgproc.hpp>

void correlation_tracker::add_track(const cv::Mat &input,
                                    const cv::Rect &roi)
{
    using namespace dlib;

    if(input.channels() != 1){
        cv::cvtColor(input, img_, CV_BGR2GRAY);
    }else{
        img_ = input;
    }

    tracker_type tracker;
    cv_image<unsigned char> gray_img(img_);
    tracker.start_track(gray_img,
                        centered_rect(point(roi.x + roi.width/2,
                                            roi.y + roi.height/2),
                                      roi.width,
                                      roi.height));
    trackers_.emplace_back(tracker);
}

void correlation_tracker::clear()
{
    trackers_.clear();
}

bool correlation_tracker::empty() const
{
    return trackers_.empty();
}

cv::Rect correlation_tracker::get_position(size_t target) const
{
    if(target < trackers_.size()){
        dlib::rectangle const position =
                trackers_[target].get_position();
        return cv::Rect(position.left(),
                        position.top(),
                        position.width(),
                        position.height());
    }

    return {};
}

std::vector<cv::Rect> correlation_tracker::get_position() const
{
    std::vector<cv::Rect> positions;
    for(size_t i = 0; i != trackers_.size(); ++i){
        positions.emplace_back(get_position(i));
    }

    return positions;
}

size_t correlation_tracker::get_track_size() const
{
    return trackers_.size();
}

void correlation_tracker::update(const cv::Mat &input)
{
    for(auto &&tracker : trackers_){
        if(input.channels() != 1){
            cv::cvtColor(input, img_, CV_BGR2GRAY);
        }else{
            img_ = input;
        }
        dlib::cv_image<unsigned char> gray_img(img_);
        tracker.update(gray_img);
    }
}
