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
    if(input.type() == CV_8UC3){
        cv::cvtColor(input, gray_, CV_BGR2GRAY);
    }else{
        gray_ = input;
    }

    dlib::cv_image<unsigned char> img(gray_);
    std::vector<dlib::rectangle> const dets =
            ffd_(img);

    for(size_t i = 0; i != dets.size(); ++i){
        regions_.emplace_back(dets[i].left(),
                              dets[i].top(),
                              dets[i].width(),
                              dets[i].height());
    }

    return regions_;
}
