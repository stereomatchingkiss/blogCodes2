#include "hsv_range_observer.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

hsv_range_observer::hsv_range_observer(std::string win_name) :
    hue_low_(0),
    hue_up_(179),
    sat_low_(0),
    sat_up_(255),
    val_low_(0),
    val_up_(255),
    win_name_(std::move(win_name))
{
    cv::namedWindow(win_name_);

    cv::createTrackbar("hue_low", win_name_,
                       &hue_low_, 179, on_hue_low, this);
    cv::createTrackbar("hue_up", win_name_,
                       &hue_up_, 179, on_hue_up, this);

    cv::createTrackbar("sat_low", win_name_,
                       &sat_low_, 255, on_sat_low, this);
    cv::createTrackbar("sat_up", win_name_,
                       &sat_up_, 255, on_sat_up, this);

    cv::createTrackbar("val_low", win_name_,
                       &val_low_, 255, on_val_low, this);
    cv::createTrackbar("val_up", win_name_,
                       &val_up_, 255, on_val_up, this);
}

void hsv_range_observer::process(cv::Mat const &input)
{
    cv::cvtColor(input, *output_, CV_BGR2HSV);
    auto const hrange = std::minmax(hue_low_, hue_up_);
    auto const srange = std::minmax(sat_low_, sat_up_);
    auto const vrange = std::minmax(val_low_, val_up_);
    cv::inRange(*output_,
                cv::Scalar(hrange.first, srange.first, vrange.first),
                cv::Scalar(hrange.second, srange.second, vrange.second),
                *output_);
}

void hsv_range_observer::apply(cv::Mat const &input, cv::Mat &output)
{
    output_ = &output;
    input_ = input;
    process(input);
}

void hsv_range_observer::on_hue_up(int v, void *ptr)
{
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->hue_up_ = v;
    my_ptr->process(my_ptr->input_);
}

void hsv_range_observer::on_sat_up(int v, void *ptr)
{
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->sat_up_ = v;
    my_ptr->process(my_ptr->input_);
}

void hsv_range_observer::on_val_up(int v, void *ptr)
{
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->val_up_ = v;
    my_ptr->process(my_ptr->input_);
}

void hsv_range_observer::on_hue_low(int v, void *ptr)
{
    //cv::cvtColor(input, output_, CV_BGR2HSV);
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->hue_low_ = v;
    my_ptr->process(my_ptr->input_);
}

void hsv_range_observer::on_sat_low(int v, void *ptr)
{
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->sat_low_ = v;
    my_ptr->process(my_ptr->input_);
}

void hsv_range_observer::on_val_low(int v, void *ptr)
{
    auto *my_ptr = static_cast<hsv_range_observer*>(ptr);
    my_ptr->val_low_ = v;
    my_ptr->process(my_ptr->input_);
}
