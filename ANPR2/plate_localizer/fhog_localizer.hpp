#ifndef FHOG_LOCALIZER_HPP
#define FHOG_LOCALIZER_HPP

#include <opencv2/core.hpp>

#include <dlib/image_processing.h>
#include <dlib/svm_threaded.h>

#include <memory>
#include <vector>

/**
 * Localize possible locations of the license plate
 * based on object detector of DLIB
 */

class fhog_localizer
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    fhog_localizer();

    contours_type const& get_contours() const;

    cv::Mat& get_resize_input();
    cv::Mat const& get_resize_input() const;

    /**
     * Find possible region of the license plates
     * @param input input image
     * @param regions regions of license plates
     */
    void localize_plate(cv::Mat const &input);

    void set_show_debug_message(bool value);

private:
    using image_scanner_type =
    dlib::scan_fhog_pyramid<dlib::pyramid_down<8>>;

    using plate_detector_type =
    dlib::object_detector<image_scanner_type>;

    contours_type  contours_;
    bool debug_ = false;
    //use shared ptr to wrap it because
    //plate_detector_type forbid copy
    std::shared_ptr<plate_detector_type> detector_;
    cv::Mat gray_mat_;
    cv::Mat resize_mat_;
};

#endif
