#ifndef FACE_DETECTOR_HPP
#define FACE_DETECTOR_HPP

#include <opencv2/core.hpp>

#include <dlib/image_processing/frontal_face_detector.h>

class face_detector
{
public:
    face_detector();

    std::vector<cv::Rect> const& detect(cv::Mat const &input);

private:
    dlib::frontal_face_detector ffd_;    
    std::vector<cv::Rect> regions_;
};

#endif
