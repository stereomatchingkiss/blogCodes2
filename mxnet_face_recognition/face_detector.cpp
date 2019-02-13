#include "face_detector.hpp"

#include <opencv2/imgproc.hpp>

#include <dlib/opencv.h>
#include <dlib/dnn.h>
#include <dlib/data_io.h>

using namespace dlib;

namespace dlib_tool
{

face_detector::face_detector(const std::string &face_detect_model_location,
                             const std::string &shape_predict_model_location,
                             int face_detect_width,
                             int face_aligned_size) :
    face_aligned_size_(face_aligned_size),
    face_detect_width_(face_detect_width)
{
    deserialize(face_detect_model_location)>>net_;
    deserialize(shape_predict_model_location)>>pose_model_;
}

dlib::matrix<rgb_pixel> face_detector::get_aligned_face(const mmod_rect &rect)
{
    auto shape = pose_model_(img_, rect);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(img_, get_face_chip_details(shape, face_aligned_size_, 0.25), face_chip);
    return face_chip;
}

std::vector<face_detector::face_info> face_detector::forward(const cv::Mat &input, bool invert_channel)
{
    auto const rects = forward_lazy(input, invert_channel);
    std::vector<face_info> result;
    for(auto const &rect : rects){
        face_info info;
        info.rect_ = rect;
        info.face_aligned_ = get_aligned_face(rect);
        result.emplace_back(std::move(info));
    }

    return result;
}

std::vector<mmod_rect> face_detector::forward_lazy(const cv::Mat &input, bool invert_channel)
{
    CV_Assert(input.channels() == 3);

    if(invert_channel){
        cv::cvtColor(input, channel_swap_cache_, CV_BGR2RGB);
    }else{
        channel_swap_cache_ = input;
    }
    if(channel_swap_cache_.cols < face_detect_width_){
        double const ratio = face_detect_width_ / static_cast<double>(channel_swap_cache_.cols);
        cv::resize(channel_swap_cache_, resize_cache_, {}, ratio, ratio);
    }else{
        resize_cache_ = channel_swap_cache_;
    }

    img_.set_size(resize_cache_.rows, resize_cache_.cols);
    dlib::assign_image(img_, dlib::cv_image<rgb_pixel>(resize_cache_));

    return net_(img_);
}

}
