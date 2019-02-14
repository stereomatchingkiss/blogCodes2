#ifndef FACE_DETECTOR_HPP
#define FACE_DETECTOR_HPP

#include "face_detector_net.hpp"

#include <dlib/image_processing.h>

#include <opencv2/core.hpp>

#include <vector>

namespace dlib_tool
{

struct face_detector_params;

class face_detector
{
public:
    struct face_info
    {
       dlib::matrix<dlib::rgb_pixel> face_aligned_;
       dlib::mmod_rect rect_;
    };

    explicit face_detector(face_detector_params const &params);

    /**
     * Use this api if you want to aligned the face after forward,
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

    std::vector<face_info> forward(cv::Mat const &input, bool invert_channel = true);
    std::vector<dlib::mmod_rect> forward_lazy(cv::Mat const &input, bool invert_channel = true);

private:
    unsigned long face_aligned_size_;
    int face_detect_width_;
    cv::Mat channel_swap_cache_;
    dlib_dnn_net::face_detect_net net_;
    dlib::matrix<dlib::rgb_pixel> img_;    
    dlib::shape_predictor pose_model_;
    cv::Mat resize_cache_;
};

}

#endif // FACE_DETECTOR_HPP
