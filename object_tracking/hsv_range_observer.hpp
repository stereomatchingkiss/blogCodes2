#ifndef HSV_RANGE_OBSERVER_HPP
#define HSV_RANGE_OBSERVER_HPP

#include <opencv2/core.hpp>

#include <string>

class hsv_range_observer
{
public:
    explicit hsv_range_observer(std::string win_name);
    void apply(cv::Mat const &input, cv::Mat &output);

private:
    static void on_hue_low(int v, void *ptr);
    static void on_sat_low(int v, void *ptr);
    static void on_val_low(int v, void *ptr);

    static void on_hue_up(int v, void *ptr);
    static void on_sat_up(int v, void *ptr);
    static void on_val_up(int v, void *ptr);

    cv::Mat hsv_;
    int hue_low_;
    int hue_up_;
    cv::Mat input_;
    cv::Mat *output_;
    int sat_low_;
    int sat_up_;
    int val_low_;
    int val_up_;
    std::string win_name_;
    void process(cv::Mat const &input);
};

#endif
