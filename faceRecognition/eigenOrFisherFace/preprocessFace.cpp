#include <exception>
#include <iostream>

#include <utility/utility.hpp>

#include "preprocessFace.hpp"

/**
 * @brief constructor
 * @param face_classifier : classifier for whole face
 * @param eye_classifier_1 : classifier for eye 1(ex : eye with glasses)
 * @param eye_classifier_2 : classifier for eye 2(ex : eye without glasses)
 */
preprocess_face::preprocess_face(std::string const &face_classifier, std::string const &eye_classifier_1, std::string const &eye_classifier_2)
{
    try{
        classifier_eye_1_.load(eye_classifier_1);
        classifier_eye_2_.load(eye_classifier_2);
        classifier_face_.load(face_classifier);
    }catch(std::exception const &ex){
        std::cerr<<"can't load cascade files"<<std::endl;
        std::cerr<<ex.what()<<std::endl;
        std::cerr<<__LINE__<<"\n"<<__FILE__<<"\n"<<__FUNCTION__<<std::endl;
    }
}

cv::Point preprocess_face::get_left_eye_center() const
{
    return point_left_eye_center_;
}

cv::Point preprocess_face::get_right_eye_center() const
{
    return point_right_eye_center_;
}

/**
 * @brief Create a grayscale face image that has a standard size and contrast & brightness.
 * "src" should be a copy of the whole color camera frame, so that it can draw the eye positions onto.
 * If 'do_left_and_right_separately' is true, it will process left & right sides seperately,
 *  so that if there is a strong light on one side but not the other, it will still look OK.
 * Performs Face Preprocessing as a combination of:
 *  - geometrical scaling, rotation and translation using Eye Detection,
 *  - smoothing away image noise using a Bilateral Filter,
 *  - standardize the brightness on both left and right sides of the face independently using separated Histogram Equalization,
 *  - removal of background and hair using an Elliptical Mask.
 * @param src : input face
 * @param dst : face after perprocess
 * @param desired_face_width : desire width of face
 * @param do_left_and_right_separately : true : apply more sophisticated post processing on face;
 * false : simple post processing
 * @return true : face detected; false : detect nothing
 */
bool preprocess_face::get_preprocessed_face(cv::Mat &src, cv::Mat &dst, int desired_face_width, bool do_left_and_right_separately)
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
        OCV::cvt_to_gray(face_img, gray_, true);
        detect_both_eyes(face_img);
        if (store_left_eye_.x >= 0 && store_right_eye_.x >= 0){
            // Make the face image the same size as the training images.

            // Since we found both eyes, lets rotate & scale & translate the face so that the 2 eyes
            // line up perfectly with ideal eye positions. This makes sure that eyes will be horizontal,
            // and not too far left or right of the face, etc.
            geometric_transform(desired_face_height, desired_face_width);

            // Give the image a standard brightness and contrast, in case it was too dark or had low contrast.
            if (!do_left_and_right_separately) {
                // Do it on the whole face.
                cv::equalizeHist(warp_, warp_);
            }else {
                // Do it seperately for the left and right sides of the face.
                equalize_left_and_right_halves();
            }

            // Use the "Bilateral Filter" to reduce pixel noise by smoothing the image, but keeping the sharp edges in the face.
            filtered_.create(warp_.size(), warp_.type());
            cv::bilateralFilter(warp_, filtered_, 0, 20.0, 2.0);

            // Filter out the corners of the face, since we mainly just care about the middle parts.
            // Draw a filled ellipse in the middle of the face-sized image.
            mask_.create(warp_.size(), CV_8U); // Start with an empty mask.
            mask_.setTo(0);
            constexpr double FACE_ELLIPSE_CY = 0.40;
            constexpr double FACE_ELLIPSE_H = 0.80;         // Controls how tall the face mask is.
            constexpr double FACE_ELLIPSE_W = 0.50;         // Should be atleast 0.5
            cv::Point const face_center = cv::Point( desired_face_width / 2, cvRound(desired_face_height * FACE_ELLIPSE_CY) );
            cv::Size const size = cv::Size( cvRound(desired_face_width * FACE_ELLIPSE_W), cvRound(desired_face_height * FACE_ELLIPSE_H) );
            cv::ellipse(mask_, face_center, size, 0, 0, 360, cv::Scalar(255), CV_FILLED);
            //imshow("mask", mask_);

            // Use the mask, to remove outside pixels.
            dst.create(warp_.size(), warp_.type()); // Clear the output image to a default gray.
            dst.setTo(128);

            // Apply the elliptical mask on the face.
            filtered_.copyTo(dst, mask_);  // Copies non-masked pixels from filtered to dstImg.

            return true;
        }
    }

    return false;
}

cv::Rect preprocess_face::get_search_left_eye() const
{
    return search_left_eye_;
}

cv::Rect preprocess_face::get_search_right_eye() const
{
    return search_right_eye_;
}

cv::Rect preprocess_face::get_store_face() const
{
    return store_face_rect_;
}

cv::Rect preprocess_face::get_store_left_eye() const
{
    return store_left_eye_;
}

cv::Rect preprocess_face::get_store_right_eye() const
{
    return store_right_eye_;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

/**
 * @brief Search for both eyes within the given face image.
 * @param face : input face
 */
void preprocess_face::detect_both_eyes(cv::Mat const &face)
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

    point_left_eye_center_ = get_eye_center_point(store_left_eye_, left_x, top_y);
    point_right_eye_center_ = get_eye_center_point(store_right_eye_, right_x, top_y);
}

/**
 *@brief Histogram Equalize seperately for the left and right sides of the face.
 *
 */
void preprocess_face::equalize_left_and_right_halves()
{
    // It is common that there is stronger light from one half of the face than the other. In that case,
    // if you simply did histogram equalization on the whole face then it would make one half dark and
    // one half bright. So we will do histogram equalization separately on each face half, so they will
    // both look similar on average. But this would cause a sharp edge in the middle of the face, because
    // the left half and right half would be suddenly different. So we also histogram equalize the whole
    // image, and in the middle part we blend the 3 images together for a smooth brightness transition.

    int const w = warp_.cols;
    int const h = warp_.rows;

    // 1) First, equalize the whole face.
    cv::equalizeHist(warp_, equalize_whole_face_);

    // 2) Equalize the left half and the right half of the face separately.
    int const midX = w / 2;
    cv::Mat left_side = warp_(cv::Rect(0, 0,  midX, h));
    cv::Mat right_side = warp_(cv::Rect(midX, 0, w - midX, h));
    cv::equalizeHist(left_side, left_side);
    cv::equalizeHist(right_side, right_side);

    // 3) Combine the left half and right half and whole face together, so that it has a smooth transition.
    for (int y = 0; y < h; ++y){
        auto left_side_ptr = left_side.ptr<uchar>(y);
        auto right_side_ptr = right_side.ptr<uchar>(y);
        auto whole_face_ptr = equalize_whole_face_.ptr<uchar>(y);
        for (int x = 0; x < w; ++x){
            int v;
            if (x < w / 4){          // Left 25%: just use the left face.
                v = left_side_ptr[x];
            }else if (x < w * 2 / 4){   // Mid-left 25%: blend the left face & whole face.
                int const lv = left_side_ptr[x];
                int const wv = whole_face_ptr[x];
                // Blend more of the whole face as it moves further right along the face.
                float const f = (x - w * 1 / 4) / static_cast<float>(w * 0.25f);
                v = cvRound( (1.0f - f) * lv + (f) * wv );                
            }else if (x < w * 3 / 4){   // Mid-right 25%: blend the right face & whole face.
                int const rv = right_side_ptr[x];
                int const wv = whole_face_ptr[x];
                // Blend more of the right-side face as it moves further right along the face.
                float const f = (x - w * 2 / 4) / static_cast<float>(w * 0.25f);
                v = cvRound((1.0f - f) * wv + (f) * rv);
            }else{                  // Right 25%: just use the right face.
                v = right_side_ptr[x];
            }
            warp_.at<uchar>(y, x) = v;

        }// end x loop
    }//end y loop
}

/**
 * @brief Since we found both eyes, lets rotate & scale & translate the face so that the 2 eyes
 *  line up perfectly with ideal eye positions. This makes sure that eyes will be horizontal,
 *  and not too far left or right of the face, etc.
 */
void preprocess_face::geometric_transform(int desired_face_height, int desired_face_width)
{
    // Get the center between the 2 eyes.
    cv::Point2f const eyes_center = {(store_left_eye_.x + store_right_eye_.x) * 0.5, (store_left_eye_.y + store_right_eye_.y) * 0.5};
    double const dy = (store_right_eye_.y - store_left_eye_.y);
    double const dx = (store_right_eye_.x - store_left_eye_.x);
    double const eye_len = std::sqrt(dx * dx + dy * dy);
    double const angle = std::atan2(dy, dx) * 180.0 / CV_PI; // Convert from radians to degrees.

    // Hand measurements shown that the left eye center should ideally be at roughly (0.19, 0.14) of a scaled face image.
    constexpr double DESIRED_LEFT_EYE_X = 0.16;
    constexpr double DESIRED_RIGHT_EYE_X = 1.0 - DESIRED_LEFT_EYE_X;
    // Get the amount we need to scale the image to be the desired fixed size we want.
    double const desired_eye_len = (DESIRED_RIGHT_EYE_X - DESIRED_LEFT_EYE_X) * desired_face_width;
    double const scale = desired_eye_len / eye_len;
    // Get the transformation matrix for rotating and scaling the face to the desired angle & size.
    cv::Mat rot_mat = cv::getRotationMatrix2D(eyes_center, angle, scale);
    // Shift the center of the eyes to be the desired center between the eyes.
    constexpr double DESIRED_LEFT_EYE_Y = 0.14;
    rot_mat.at<double>(0, 2) += desired_face_width * 0.5 - eyes_center.x;
    rot_mat.at<double>(1, 2) += desired_face_height * DESIRED_LEFT_EYE_Y - eyes_center.y;

    warp_.create(desired_face_height, desired_face_width, CV_8U);
    warp_.setTo(128);
    cv::warpAffine(gray_, warp_, rot_mat, warp_.size());
}

cv::Point preprocess_face::get_eye_center_point(cv::Rect &eye_rect, int x_offset, int y_offset)
{
    if (eye_rect.width > 0) {
        // Adjust the left-eye rectangle because the face border was removed.
        eye_rect.x += x_offset;
        eye_rect.y += y_offset;

        return {eye_rect.x + eye_rect.width / 2, eye_rect.y + eye_rect.height / 2};
     }else{
       return {-1, -1};    // Return an invalid point
     }
}
