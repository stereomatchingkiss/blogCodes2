#include <exception>
#include <iostream>

#include <utility/utility.hpp>

#include "preprocessFace.hpp"

preprocessFace::preprocessFace(std::string const &face_classifier, std::string const &eye_classifier_1, std::string const &eye_classifier_2)
{
    try{
        classifier_eye_1_.load(eye_classifier_1);
        classifier_eye_2_.load(eye_classifier_2.c_str());
        classifier_face_.load(face_classifier.c_str());
    }catch(std::exception const &ex){
        std::cerr<<"can't load cascade files"<<std::endl;
        std::cerr<<ex.what()<<std::endl;
        std::cerr<<__LINE__<<"\n"<<__FILE__<<"\n"<<__FUNCTION__<<std::endl;
    }
}

cv::Mat preprocessFace::getPreprocessedFace(cv::Mat &src, int desired_face_width, bool do_left_and_right_separately)
{
    // Use square faces.
    int const desired_face_height = desired_face_width;

    // Mark the detected face region and eye search regions as invalid, in case they aren't detected.
    search_left_eye_.width = -1;
    search_right_eye_.width = -1;
    store_left_eye_.x = -1;
    store_right_eye_.x= -1;

    store_face_rect_ = detect_object_.detect_largest_object(src, classifier_face_);
    if(store_face_rect_.width > 0){
        cv::Mat face_img = src(store_face_rect_);    // Get the detected face image.

        // If the input image is not grayscale, then convert the BGR or BGRA color image to grayscale.
        OCV::cvt_to_gray(face_img, gray_);
        detect_both_eyes(face_img);
        if (store_left_eye_.x >= 0 && store_right_eye_.x >= 0) {
            // Make the face image the same size as the training images.

            // Since we found both eyes, lets rotate & scale & translate the face so that the 2 eyes
            // line up perfectly with ideal eye positions. This makes sure that eyes will be horizontal,
            // and not too far left or right of the face, etc.

            // Get the center between the 2 eyes.
            cv::Point2f eyes_center = {(store_left_eye_.x + store_right_eye_.x) * 0.5f, (store_left_eye_.y + store_right_eye_.y) * 0.5f};
            double dy = (store_right_eye_.y - store_left_eye_.y);
            double dx = (store_right_eye_.x - store_left_eye_.x);
            double len = std::sqrt(dx*dx + dy*dy);
            double angle = std::atan2(dy, dx) * 180.0 / CV_PI; // Convert from radians to degrees.
        }
    }

    return {};
}

cv::Rect preprocessFace::get_search_left_eye() const
{
    return search_left_eye_;
}

cv::Rect preprocessFace::get_search_right_eye() const
{
    return search_right_eye_;
}

cv::Rect preprocessFace::get_store_face() const
{
    return store_face_rect_;
}

cv::Rect preprocessFace::get_store_left_eye() const
{
    return store_left_eye_;
}

cv::Rect preprocessFace::get_store_right_eye() const
{
    return store_right_eye_;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/
void preprocessFace::detect_both_eyes(cv::Mat const &face)
{
    // For default eye.xml or eyeglasses.xml: Finds both eyes in roughly 40% of detected faces, but does not detect closed eyes.
    constexpr float EYE_SX = 0.16f;
    constexpr float EYE_SY = 0.26f;
    constexpr float EYE_SW = 0.30f;
    constexpr float EYE_SH = 0.28f;

    int const left_x = cvRound(face.cols * EYE_SX);
    int const top_y = cvRound(face.rows * EYE_SY);
    int const width_x = cvRound(face.cols * EYE_SW);
    int const height_y = cvRound(face.rows * EYE_SH);
    int const right_x = cvRound(face.cols * (1.0 - EYE_SX-EYE_SW) );  // Start of right-eye corner

    search_left_eye_ = {left_x, top_y, width_x, height_y};
    search_right_eye_ = {right_x, top_y, width_x, height_y};

    cv::Mat top_left_of_face = face(search_left_eye_);
    cv::Mat top_right_of_face = face(search_right_eye_);

    store_left_eye_ = detect_object_.detect_largest_object(top_left_of_face, classifier_eye_1_, top_left_of_face.cols);
    store_right_eye_ = detect_object_.detect_largest_object(top_right_of_face, classifier_eye_1_, top_right_of_face.cols);

    // If the eye was not detected, try a different cascade classifier.
    if (store_left_eye_.width <= 0 && !classifier_eye_2_.empty()) {
        store_left_eye_ = detect_object_.detect_largest_object(top_left_of_face, classifier_eye_2_, top_left_of_face.cols);
    }

    // If the eye was not detected, try a different cascade classifier.
    if (store_right_eye_.width <= 0 && !classifier_eye_2_.empty()) {
        store_right_eye_ = detect_object_.detect_largest_object(top_right_of_face, classifier_eye_2_, top_right_of_face.cols);
    }

    point_left_eye_ = get_eye_point(store_left_eye_, left_x, top_y);
    point_right_eye_ = get_eye_point(store_right_eye_, right_x, top_y);
}

cv::Point preprocessFace::get_eye_point(cv::Rect &eye_rect, int x_offset, int y_offset)
{
    if (eye_rect.width > 0) {   // Check if the eye was detected.
        eye_rect.x += x_offset;    // Adjust the left-eye rectangle because the face border was removed.
        eye_rect.y += y_offset;

        return {eye_rect.x + eye_rect.width / 2, eye_rect.y + eye_rect.height / 2};
    }else {
        return {-1, -1};    // Return an invalid point
    }
}
