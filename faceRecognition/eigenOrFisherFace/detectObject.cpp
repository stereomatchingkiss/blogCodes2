#include <algorithm>

#include <opencv2/imgproc/imgproc.hpp>

#include <utility/utility.hpp>

#include "detectObject.hpp"

detectObject::detectObject() :
    flags_(cv::CASCADE_FIND_BIGGEST_OBJECT),
    min_feature_size_{20, 20},
    min_neighbors_{4},
    search_scale_factor_{1.1}
{
}

/**
 * @brief search for just a single object in the image, such as the largest face, storing the result into 'largestObject'.
 * Can use Haar cascades or LBP cascades for Face Detection, or even eye, mouth, or car detection.
 * Input is temporarily shrunk to 'scaled_width' for much faster detection, since 200 is enough to find faces.
 * @param img : input img
 * @param cascade : the cascade, could be LBP or harr
 * @param scaled_width : desired width of the face
 * @return the rect which represent the position and size of the face in img
 */
cv::Rect detectObject::detect_largest_object(cv::Mat const &img, cv::CascadeClassifier &cascade, int scaled_width)
{       
    // Only search for just 1 object (the biggest in the image).
    flags_ = cv::CASCADE_FIND_BIGGEST_OBJECT;
    // Smallest object size.
    min_feature_size_ = cv::Size(20, 20);
    // How detailed should the search be. Must be larger than 1.0.
    search_scale_factor_ = 1.1f;
    // How much the detections should be filtered out. This should depend on how bad false detections are to your system.
    // minNeighbors=2 means lots of good+bad detections, and minNeighbors=6 means only good detections are given but some are missed.
    min_neighbors_ = 4;

    // Perform Object or Face Detection, looking for just 1 object (the biggest in the image).
    detect_objects_custom(img, cascade, std::abs(scaled_width));

    return !objects_.empty() ? objects_[0] : cv::Rect(-1,-1,-1,-1);
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

/**
 * @brief Search for objects such as faces in the image using the given parameters
   Can use Haar cascades or LBP cascades for Face Detection, or even eye, mouth, or car detection.
   Input is temporarily shrunk to 'scaled_width' for much faster detection, since 200 is enough to find faces.
 * @param img : input img
 * @param cascade : the cascade, could be LBP or harr
 * @param scaled_width : desired width of the face
 */
void detectObject::detect_objects_custom(cv::Mat const &img, cv::CascadeClassifier &cascade, int scaled_width)
{
    // If the input image is not grayscale, then convert the BGR or BGRA color image to grayscale.
    cv::Mat temp = img;
    OCV::cvt_to_gray(temp, gray_, true);

    // Possibly shrink the image, to run much faster.
    float const scale = img.cols / static_cast<float>(scaled_width);
    if (img.cols > scaled_width) {
        // Shrink the image while keeping the same aspect ratio.
        int const scaled_height = cvRound(img.rows / scale);
        cv::resize(gray_, shrink_input_, cv::Size(scaled_width, scaled_height));
    }else {
        // Access the input image directly, since it is already small.        
        gray_.copyTo(shrink_input_);
    }

    // Standardize the brightness and contrast to improve dark images.
    cv::Mat equalized_input = shrink_input_;
    cv::equalizeHist(shrink_input_, equalized_input);

    // Detect objects in the small grayscale image.
    cascade.detectMultiScale(equalized_input, objects_, search_scale_factor_, min_neighbors_, flags_, min_feature_size_);

    // Enlarge the results if the image was temporarily shrunk before detection.
    if (img.cols > scaled_width) {
        for (auto &objects : objects_) {
            objects.x = cvRound(objects.x * scale);
            objects.y = cvRound(objects.y * scale);
            objects.width = cvRound(objects.width * scale);
            objects.height = cvRound(objects.height * scale);
        }
    }

    // Make sure the object is completely within the image, in case it was on a border.
    for (auto &objects : objects_) {
        if (objects.x < 0){
            objects.x = 0;
        }
        if (objects.y < 0){
            objects.y = 0;
        }
        if (objects.x + objects.width > img.cols){
            objects.x = img.cols - objects.width;
        }
        if (objects.y + objects.height > img.rows){
            objects.y = img.rows - objects.height;
        }
    }
}


