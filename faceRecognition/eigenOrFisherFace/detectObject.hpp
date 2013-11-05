#ifndef DETECTOBJECT_HPP
#define DETECTOBJECT_HPP

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

class detectObject
{
public:
    detectObject();

    cv::Rect detect_largest_object(cv::Mat const &img, cv::CascadeClassifier &cascade, int scaled_width = 320);

private:
    void detect_objects_custom(cv::Mat const &img, cv::CascadeClassifier &cascade, int scaled_width);

private:
    int flags_;
    cv::Mat gray_;

    cv::Size min_feature_size_; //Smallest object size.
    int min_neighbors_;  //Parameter specifying how many neighbors each candidate rectangle should have to retain it.
                         //How much the detections should be filtered out. This should depend on how bad false
                         //detections are to your system. minNeighbors=2 means lots of good+bad detections, and
                         //minNeighbors=6 means only good detections are given but some are missed.

    std::vector<cv::Rect> objects_; //Vector of rectangles where each rectangle contains the detected object.

    cv::Mat shrink_input_;
    float search_scale_factor_; //How detailed should the search be. Must be larger than 1.0.
                                //Parameter specifying how much the image size is reduced at each image scale
};

#endif // DETECTOBJECT_HPP
