#ifndef OCV_FACE_DLIB_CNN_FACE_DETECTOR_PARAMS_HPP
#define OCV_FACE_DLIB_CNN_FACE_DETECTOR_PARAMS_HPP

#include <string>

namespace ocv
{

namespace face{

struct dlib_cnn_face_detector_params
{
    dlib_cnn_face_detector_params(std::string face_detect_model_location,
                                  std::string shape_predict_model_location,
                                  double detect_confidence = 0.6) :
        detect_confidence_(detect_confidence),
        face_detect_model_location_(std::move(face_detect_model_location)),
        shape_predict_model_location_(std::move(shape_predict_model_location))
    {}
    ~dlib_cnn_face_detector_params(){}

    double detect_confidence_;
    std::string face_detect_model_location_;
    std::string shape_predict_model_location_;
    int face_detect_width_ = 640;
    unsigned long face_aligned_size_ = 112;
};

}

}

#endif // OCV_FACE_DLIB_CNN_FACE_DETECTOR_PARAMS_HPP
