#include "plot_object_detector_bboxes.hpp"

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

void plot_object_detector_bboxes::plot(cv::Mat &inout,
                                       std::vector<mxnet::cpp::NDArray> const &predict_results,
                                       bool normalize)
{
    using namespace mxnet::cpp;

    //1. predict_results get from the output of Executor(executor_->outputs)
    //2. Must Set Context as cpu because we need process data by cpu later
    auto labels = predict_results[0].Copy(Context(kCPU, 0));
    auto scores = predict_results[1].Copy(Context(kCPU, 0));
    auto bboxes = predict_results[2].Copy(Context(kCPU, 0));
    //1. Should call wait because Forward api of Executor is async
    //2. scores and labels could treat as one dimension array
    //3. BBoxes can treat as 2 dimensions array
    bboxes.WaitToRead();
    scores.WaitToRead();
    labels.WaitToRead();

    size_t const num = bboxes.GetShape()[1];
    for(size_t i = 0; i < num; ++i) {
        float const score = scores.At(0, 0, i);
        if (score < thresh_) break;

        size_t const cls_id = static_cast<size_t>(labels.At(0, 0, i));
        auto const color = colors_[cls_id];
        //pt1 : top left; pt2 : bottom right
        cv::Point pt1, pt2;
        //get_points perform normalization
        std::tie(pt1, pt2) = normalize_points(bboxes.At(0, i, 0), bboxes.At(0, i, 1),
                                              bboxes.At(0, i, 2), bboxes.At(0, i, 3),
                                              normalize, cv::Size(inout.cols, inout.rows));
        cv::rectangle(inout, pt1, pt2, color, 2);

        std::string txt;
        if (labels_.size() > cls_id) {
            txt += labels_[cls_id];
        }
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << score;
        txt += " " + ss.str();
        put_label(inout, txt, pt1, color);
    }
}

void plot_object_detector_bboxes::set_normalize_size(const cv::Size &normalize_size) noexcept
{
    normalize_size_ = normalize_size;
    if(normalize_size_.height == 0){
        normalize_size_.height = 1;
    }
    if(normalize_size_.width == 0){
        normalize_size_.width = 1;
    }
}

void plot_object_detector_bboxes::set_thresh(float val) noexcept
{
    thresh_ = val;
}

std::pair<cv::Point, cv::Point> plot_object_detector_bboxes::
normalize_points(float x1, float y1, float x2, float y2, bool normalize, cv::Size const &input_size) const noexcept
{
    if(normalize && (normalize_size_.height != input_size.height || normalize_size_.width != input_size.width)){
        x1 = x1/normalize_size_.width * input_size.width;
        y1 = y1/normalize_size_.height * input_size.height;
        x2 = x2/normalize_size_.width * input_size.width;
        y2 = y2/normalize_size_.height * input_size.height;
        return {cv::Point(static_cast<int>(x1), static_cast<int>(y1)),
                    cv::Point(static_cast<int>(x2), static_cast<int>(y2))};
    }else{
        return {cv::Point(static_cast<int>(x1), static_cast<int>(y1)),
                    cv::Point(static_cast<int>(x2), static_cast<int>(y2))};
    }
}

}
