#include "face_detector.hpp"

#include "face_detector_params.hpp"

#include <opencv2/imgproc.hpp>

#include <dlib/opencv.h>
#include <dlib/dnn.h>
#include <dlib/data_io.h>

using namespace dlib;

namespace dlib_tool
{

face_detector::face_detector(face_detector_params const &params) :
    face_aligned_size_(params.face_aligned_size_),
    face_detect_width_(params.face_detect_width_)
{
    deserialize(params.face_detect_model_location_)>>net_;
    deserialize(params.shape_predict_model_location_)>>pose_model_;
}

dlib::matrix<rgb_pixel> face_detector::get_aligned_face(const mmod_rect &rect)
{
    auto shape = pose_model_(img_, rect);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(img_, get_face_chip_details(shape, face_aligned_size_, 0.25), face_chip);
    return face_chip;
}

std::vector<face_detector::face_info> face_detector::forward(const cv::Mat &input)
{
    auto const rects = forward_lazy(input);
    std::vector<face_info> result;
    for(auto const &rect : rects){
        face_info info;
        info.rect_ = rect;
        info.face_aligned_ = get_aligned_face(rect);
        result.emplace_back(std::move(info));
    }

    return result;
}

std::vector<mmod_rect> face_detector::forward_lazy(const cv::Mat &input)
{
    CV_Assert(input.channels() == 3);

    if(input.cols < face_detect_width_){
        double const ratio = face_detect_width_ / static_cast<double>(input.cols);
        cv::resize(input, resize_cache_, {}, ratio, ratio);
    }else{
        resize_cache_ = input;
    }

    img_.set_size(resize_cache_.rows, resize_cache_.cols);
    dlib::assign_image(img_, dlib::cv_image<bgr_pixel>(resize_cache_));

    return net_(img_);
}

}
