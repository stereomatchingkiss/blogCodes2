#ifndef OCV_FACE_DLIB_CNN_FACE_DETECTOR_HPP
#define OCV_FACE_DLIB_CNN_FACE_DETECTOR_HPP

#include "dlib_face_detector_net.hpp"

#include <dlib/image_processing.h>

#include <opencv2/core.hpp>

#include <vector>

namespace ocv
{

namespace face{

struct dlib_cnn_face_detector_params;

class dlib_cnn_face_detector
{
public:
    struct face_info
    {
       std::vector<dlib::matrix<dlib::rgb_pixel>> face_aligned_;
       std::vector<dlib::mmod_rect> rect_;
       std::vector<dlib::full_object_detection> shapes_;
    };

    explicit dlib_cnn_face_detector(dlib_cnn_face_detector_params const &params);
    ~dlib_cnn_face_detector();
    dlib_cnn_face_detector(dlib_cnn_face_detector&&) = default;
    dlib_cnn_face_detector& operator=(dlib_cnn_face_detector&&) = default;

    /**
     * Detect and align faces
     * @param input Should be bgr channels
     * @return roi of faces and faces after aligned
     */
    face_info forward(cv::Mat const &input);
    /**
     * Detect faces without alignment
     * @param input Should be bgr channels
     * @return roi of faces
     */
    std::vector<dlib::mmod_rect> forward_lazy(cv::Mat const &input);

    /**
     * Use this api if you want to aligned the face after forward_lazy
     * @return Face after aligned
     * @warning forward or forward_lazy must be called before you call this api
     * @code
     * face_detector fd(...);
     * cv::Mat const img = cv::imread("some_img.jpg");
     * auto const &face_roi = fd.forward_lazy(img);
     * //Aligned all of the faces capture after forward_lazy
     * for(auto const &roi : face_roi){
     *     auto const face = fd.get_aligned_face(roi);
     * }
     * @endcode
     */
    dlib::matrix<dlib::rgb_pixel> get_aligned_face(dlib::mmod_rect const &rect);
    std::pair<dlib::matrix<dlib::rgb_pixel>, dlib::full_object_detection>
    get_aligned_face_and_shape(dlib::mmod_rect const &rect);

private:
    double detect_confidence_;
    unsigned long face_aligned_size_;
    int face_detect_width_;
    ocv::face::dlib_net::face_detect_net net_;
    dlib::matrix<dlib::rgb_pixel> img_;    
    dlib::shape_predictor pose_model_;
    double ratio_ = 1.0;
    cv::Mat resize_cache_;
};

}

}

#endif // OCV_FACE_DLIB_CNN_FACE_DETECTOR_HPP
