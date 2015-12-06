#ifndef MORPHOLOGY_LOCALIZER_HPP
#define MORPHOLOGY_LOCALIZER_HPP

#include <opencv2/core.hpp>

/**
 * Localize possible locations of the license plate
 * based on tophat morphology and other basic skills
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
    void binarize_image();
    void create_light_input();
    void find_plate_contours();    
    void preprocess(cv::Mat const &input);
    void remove_noise();
    void reveal_dark_area();

    cv::Mat binary_input_;    
    cv::Mat blur_input_;
    std::vector<std::vector<cv::Point>>  contours_;
    bool debug_ = false;
    cv::Mat gradient_input_;
    cv::Mat gray_input_;
    cv::Mat hsv_;
    cv::Mat intensity_;
    cv::Mat light_input_;
    cv::Mat morphology_input_;
    cv::Mat resize_input_;
    std::vector<cv::Mat> split_hsv_;
    cv::Mat tophat_kernel_;
};

#endif
