#ifndef PRUNE_ILLEGAL_CHARS_HPP
#define PRUNE_ILLEGAL_CHARS_HPP

#include <opencv2/core.hpp>

#include <vector>

class prune_illegal_chars
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    explicit prune_illegal_chars(size_t max_char_num = 6);

    void prune(cv::Mat const &plate,
               contours_type &contours);

    void prune(cv::Mat const &plate,
               size_t max_char_num,
               contours_type &contours);

    void set_show_debug_message(bool value);

private:
    void prune_fix_num_plate(size_t max_char_num,
                             contours_type &contours);

    void show_prune_results(cv::Mat const &plate,
                            contours_type const &contours) const;

    bool debug_ = false;

    size_t max_char_num_;

    std::vector<std::pair<int,contour_type>> diff_;
    cv::Mat_<int> dims_;
};

#endif
