#ifndef PREPROCESSFACE_HPP
#define PREPROCESSFACE_HPP

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>

#include "detectObject.hpp"

class preprocess_face
{
public:
    preprocess_face(std::string const &face_classifier, std::string const &eye_classifier_1, std::string const &eye_classifier_2);

    cv::Point get_left_eye_center() const;
    cv::Point get_right_eye_center() const;

    bool get_preprocessed_face(cv::Mat &src, cv::Mat &dst, int desired_face_width, bool do_left_and_right_separately);

    cv::Rect get_search_left_eye() const;
    cv::Rect get_search_right_eye() const;
    cv::Rect get_store_face() const;
    cv::Rect get_store_left_eye() const;
    cv::Rect get_store_right_eye() const;        

private:
    void detect_both_eyes(cv::Mat const &face);

    void equalize_left_and_right_halves();

    void geometric_transform(int desired_face_height, int desired_face_width);
    cv::Point get_eye_center_point(cv::Rect &eye_rect, int x_offset, int y_offset);

private:
    cv::CascadeClassifier classifier_eye_1_;
    cv::CascadeClassifier classifier_eye_2_;
    cv::CascadeClassifier classifier_face_;

    detectObject detect_object_;
    cv::Mat equalize_whole_face_;
    cv::Mat filtered_;
    cv::Mat gray_;
    cv::Mat mask_;

    cv::Point point_left_eye_center_;
    cv::Point point_right_eye_center_;

    cv::Rect search_left_eye_;
    cv::Rect search_right_eye_;
    cv::Rect store_face_rect_;
    cv::Rect store_left_eye_;
    cv::Rect store_right_eye_;

    cv::Mat warp_; //the face after warping(geometrical transformation)
};

#endif // PREPROCESSFACE_HPP
