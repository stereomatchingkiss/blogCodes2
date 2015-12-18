#ifndef MORPHOLOGY_LOCALIZER_HPP
#define MORPHOLOGY_LOCALIZER_HPP

#include <opencv2/core.hpp>

#include <vector>

class segment_character
{
public:
    using Countours = std::vector<std::vector<cv::Point>>;

    /**
     * @param min_char_width minimum width of the characters
     * @param min_char_num minimum number of the license plate characters
     * @param max_char_num maximum number of the license plate characters
     */
    segment_character(size_t min_char_width,
                      size_t min_char_num,
                      size_t max_char_num);

    /**
     * detect the characters of license plate
     * @param input input image
     * @param contours contours of candidate plates
     * @return true if able to detect characters and vice versa
     */
    bool detect_characters(cv::Mat const &input,
                           Countours const &contours);

    void set_show_debug_message(bool value);

private:
    void generate_bird_eyes_view(cv::Mat const &input, Countours const &contours);

    bool debug_ = false;
    std::vector<cv::Mat> plate_;
    std::vector<cv::Mat> hsv_;
    cv::Mat intensity_; //v channel of hsv
    cv::Mat threshold_;

    size_t min_char_width_ = 40;
    size_t min_char_num_ = 6;
    size_t max_char_num_ = 8;
};

#endif
