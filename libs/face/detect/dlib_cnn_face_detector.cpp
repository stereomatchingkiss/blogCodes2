#include "dlib_cnn_face_detector.hpp"

#include "dlib_cnn_face_detector_params.hpp"

#include <opencv2/imgproc.hpp>

#include <dlib/opencv.h>
#include <dlib/dnn.h>
#include <dlib/data_io.h>

using namespace dlib;

namespace ocv
{

namespace face{

dlib_cnn_face_detector::dlib_cnn_face_detector(dlib_cnn_face_detector_params const &params) :
    detect_confidence_(params.detect_confidence_),
    face_aligned_size_(params.face_aligned_size_),
    face_detect_width_(params.face_detect_width_)
{
    deserialize(params.face_detect_model_location_)>>net_;
    deserialize(params.shape_predict_model_location_)>>pose_model_;
}

dlib_cnn_face_detector::~dlib_cnn_face_detector()
{

}

dlib::matrix<rgb_pixel> dlib_cnn_face_detector::get_aligned_face(const mmod_rect &rect)
{
    auto shape = pose_model_(img_, rect);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(img_, get_face_chip_details(shape, face_aligned_size_, 0.25), face_chip);
    return face_chip;
}

dlib_cnn_face_detector::face_info dlib_cnn_face_detector::forward(const cv::Mat &input)
{
    auto rects = forward_lazy(input);
    face_info result;
    for(auto &rect : rects){
        if(rect.detection_confidence >= detect_confidence_){
            result.face_aligned_.emplace_back(get_aligned_face(rect));
            if(ratio_ != 1.0){
                rect.rect.set_left(static_cast<long>(rect.rect.left() / ratio_));
                rect.rect.set_top(static_cast<long>(rect.rect.top() / ratio_));
                rect.rect.set_bottom(static_cast<long>(rect.rect.bottom() / ratio_));
                rect.rect.set_right(static_cast<long>(rect.rect.right() / ratio_));
            }
            result.rect_.emplace_back(rect);
        }
    }

    return result;
}

std::vector<mmod_rect> dlib_cnn_face_detector::forward_lazy(const cv::Mat &input)
{
    CV_Assert(input.channels() == 3);

    if(input.cols != face_detect_width_){
        ratio_ = face_detect_width_ / static_cast<double>(input.cols);
        cv::resize(input, resize_cache_, {}, ratio_, ratio_);
    }else{
        ratio_ = 1.0;
        resize_cache_ = input;
    }

    img_.set_size(resize_cache_.rows, resize_cache_.cols);
    dlib::assign_image(img_, dlib::cv_image<bgr_pixel>(resize_cache_));

    return net_(img_);
}

}

}
