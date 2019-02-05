#include "plot_object_detector_bboxes.hpp"

#include "object_detector_filter.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iomanip>
#include <random>
#include <sstream>

namespace viz {

namespace{

cv::Scalar hsv_2_bgr(cv::Scalar hsv) {
    cv::Mat from(1, 1, CV_32FC3, hsv);
    cv::Mat to;
    cv::cvtColor(from, to, cv::COLOR_HSV2BGR);
    auto pixel = to.at<cv::Vec3f>(0, 0);
    unsigned char b = static_cast<unsigned char>(pixel[0] * 255);
    unsigned char g = static_cast<unsigned char>(pixel[1] * 255);
    unsigned char r = static_cast<unsigned char>(pixel[2] * 255);
    return cv::Scalar(b, g, r);
}

void put_label(cv::Mat &im, std::string const &label, cv::Point const &orig, cv::Scalar const &color) {
    int constexpr fontface = cv::FONT_HERSHEY_DUPLEX;
    double constexpr scale = 0.5;
    int constexpr thickness = 1;
    int baseline = 0;
    double constexpr alpha = 0.6;

    cv::Size const text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    // make sure roi inside image region
    cv::Rect blend_rect = cv::Rect(orig + cv::Point(0, baseline),
                                   orig + cv::Point(text.width, -text.height)) & cv::Rect(0, 0, im.cols, im.rows);
    cv::Mat const roi = im(blend_rect);
    cv::Mat const blend(roi.size(), CV_8UC3, color);
    cv::addWeighted(blend, alpha, roi, 1.0 - alpha, 0.0, roi);
    cv::putText(im, label, orig, fontface, scale, cv::Scalar(255, 255, 255), thickness, 8);
}

}

std::vector<cv::Scalar> generate_colors(size_t target_size)
{
    std::vector<cv::Scalar> results(target_size);
    for(size_t i = 0; i != target_size; ++i){
        double const temp_hue = static_cast<double>(i) / target_size;
        results[i] = hsv_2_bgr(cv::Scalar(temp_hue * 255.0, 0.75, 0.95));
    }

    return results;
}

plot_object_detector_bboxes::plot_object_detector_bboxes(std::vector<std::string> labels, float thresh) :
    colors_(generate_colors(labels.size())),
    labels_(std::move(labels)),
    thresh_(thresh)
{

}

plot_object_detector_bboxes::plot_object_detector_bboxes(std::vector<std::string> labels,
                                                         std::vector<cv::Scalar> colors,
                                                         float thresh) :
    colors_(std::move(colors)),
    labels_(std::move(labels)),
    thresh_(thresh)
{

}

plot_object_detector_bboxes::~plot_object_detector_bboxes()
{

}

void plot_object_detector_bboxes::plot(cv::Mat &inout,
                                       std::vector<mxnet::cpp::NDArray> const &predict_results)
{
    using namespace mxnet::cpp;

    if(!obj_filter_){
        std::vector<object_detector_filter::item_type> types_to_detect;
        types_to_detect.emplace_back(object_detector_filter::item_type::person);
        obj_filter_ = std::make_unique<object_detector_filter>(types_to_detect, process_size_, thresh_);
    }
    auto const &results = obj_filter_->filter(predict_results, cv::Size(inout.cols, inout.rows));    
    for(auto const &res : results){
        auto const label = static_cast<size_t>(res.item_);
        cv::rectangle(inout, res.roi_, colors_[static_cast<size_t>(res.item_)], 2);

        std::string txt;
        txt += labels_[label];
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << res.confidence_;
        txt += " " + ss.str();
        put_label(inout, txt, res.roi_.tl(), colors_[label]);
    }
    cv::putText(inout, std::to_string(results.size()) + " people",
                cv::Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
}

void plot_object_detector_bboxes::set_process_size_of_detector(const cv::Size &process_size) noexcept
{
    process_size_ = process_size;
    if(process_size_.height == 0){
        process_size_.height = 1;
    }
    if(process_size_.width == 0){
        process_size_.width = 1;
    }
}

void plot_object_detector_bboxes::set_thresh(float val) noexcept
{
    thresh_ = val;
}

}
