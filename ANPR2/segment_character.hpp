#ifndef SEGMENT_CHARACTER_HPP
#define SEGMENT_CHARACTER_HPP

#include <opencv2/core.hpp>

#include <vector>

class segment_character
{
public:
    using Countour = std::vector<cv::Point>;

    segment_character() = default;

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
                           Countour const &contours);

    void set_min_char_width(size_t value);
    void set_min_char_num(size_t value);
    void set_max_char_num(size_t value);
    void set_show_debug_message(bool value);

private:
    void binarize_plate();
    void generate_bird_eyes_view(cv::Mat const &input, Countour const &contour);
    //void split_character();

    bool debug_ = false;
    cv::Mat plate_;
    std::vector<cv::Mat> hsv_split_;
    cv::Mat hsv_;
    cv::Mat intensity_; //v channel of hsv
    cv::Mat threshold_;

    size_t min_char_width_ = 40;
    size_t min_char_num_ = 6;
    size_t max_char_num_ = 8;
};

#endif
