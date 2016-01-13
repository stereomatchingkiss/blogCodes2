#include "face_detector.hpp"

#include <opencv2/imgproc.hpp>

#include <dlib/opencv/cv_image.h>

face_detector::face_detector() :
    ffd_(dlib::get_frontal_face_detector())
{

}

std::vector<cv::Rect> const& face_detector::
detect(const cv::Mat &input)
{
    regions_.clear();
    std::vector<dlib::rectangle> dets;
    if(input.type() == CV_8UC3){
        dlib::cv_image<dlib::bgr_pixel> img(input);
        dets = ffd_(img);
    }else{
        dlib::cv_image<unsigned char> img(input);
        dets = ffd_(img);
    }

    for(size_t i = 0; i != dets.size(); ++i){
        regions_.emplace_back(dets[i].left(),
                              dets[i].top(),
                              dets[i].width(),
                              dets[i].height());
    }

    return regions_;
}
