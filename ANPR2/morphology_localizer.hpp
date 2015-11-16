#ifndef MORPHOLOGY_LOCALIZER_HPP
#define MORPHOLOGY_LOCALIZER_HPP

#include <opencv2/core.hpp>

/**
 * Localize possible locations of the license plate
 */
class morphology_localizer
{
public:
    morphology_localizer();

    /**
     * Find possible region of the license plates
     * @param input input image
     * @param regions regions of license plates
     */
    void localize_plate(cv::Mat const &input,
                        std::vector<cv::Rect> &regions);

    void set_show_debug_message(bool value);

private:
    void filter_background();
    void find_plate_contours();
    void morphology_filter();
    void preprocess(cv::Mat const &input);
    void sobel_filter();

    cv::Mat binary_input_;
    bool debug_ = false;
    cv::Mat hsv_;
    cv::Mat intensity_;
    std::vector<std::vector<cv::Point>>  contours_;
    cv::Mat resize_input_;
    std::vector<cv::Mat> split_hsv_;
};

#endif
