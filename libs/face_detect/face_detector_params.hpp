#ifndef FACE_DETECTOR_PARAMS_HPP
#define FACE_DETECTOR_PARAMS_HPP

#include <string>

namespace ocv
{

namespace face{

struct face_detector_params
{
    face_detector_params(std::string face_detect_model_location,
                         std::string shape_predict_model_location) :
        face_detect_model_location_(std::move(face_detect_model_location)),
        shape_predict_model_location_(std::move(shape_predict_model_location))
    {}

    std::string face_detect_model_location_;
    std::string shape_predict_model_location_;
    int face_detect_width_ = 640;
    unsigned long face_aligned_size_ = 112;
};

}

}

#endif // FACE_DETECTOR_PARAMS_HPP
